//
// Created by xflajs00 on 30.10.2021.
//

#include "PhysarumSimulator.h"
#include <algorithm>
#include <glm/geometric.hpp>
#include <pf_common/RAII.h>
#include <range/v3/algorithm/max.hpp>
#include <range/v3/numeric/accumulate.hpp>
#include <simulation/generators/InwardCircleParticleGenerator.h>
#include <simulation/generators/RandomCircleParticleGenerator.h>
#include <simulation/generators/UniformParticleGenerator.h>

namespace pf::physarum {

void PhysarumSimulator::simulate(float currentTime, float deltaTime) {
  if (mouseInteractionActive && interactionConfig.interactionType == MouseInteraction::Emit) {
    auto gen = physarum::PointParticleGenerator{attractorPosition};
    std::vector<Particle> particles;
    if (interactionConfig.interactedSpecies == -1) {
      for (int i = 0; i < simSpeciesSettings.size(); ++i) {
        std::ranges::copy(gen.generateParticles(interactionConfig.particleCount, i), std::back_inserter(particles));
      }
    } else {
      particles = gen.generateParticles(interactionConfig.particleCount, interactionConfig.interactedSpecies);
    }
    addParticles(std::span{particles});
  }
  simulateProgram->use();
  simulateProgram->set1f("deltaT", deltaTime);
  simulateProgram->set1f("time", currentTime);

  int interactionType;
  switch (interactionConfig.interactionType) {
    case MouseInteraction::Emit: interactionType = 0; break;
    default: interactionType = static_cast<int>(interactionConfig.interactionType); break;
  }

  simulateProgram->set1i("mouseInteractionType", interactionType);
  simulateProgram->set1i("mouseInteractionActive", mouseInteractionActive ? 1 : 0);
  simulateProgram->set2fv("mousePosition", &attractorPosition[0]);
  simulateProgram->set1f("mouseIntDistance", interactionConfig.distance);
  simulateProgram->set1f("mouseIntPower", interactionConfig.power);
  simulateProgram->set1i("interactedSpeciesId", interactionConfig.interactedSpecies);
  simulateProgram->set1i("speciesCount", simSpeciesSettings.size());
  simulateProgram->set1ui("totalParticleCount", totalParticleCount);

  particleBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
  trailTexture->bindImage(1);
  speciesSettingsBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 2);
  speciesInteractionBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 3);
  simulateProgram->dispatch(totalParticleCount / 64 + 1, 1, 1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  diffuseTrailProgram->use();
  diffuseTrailProgram->set1f("deltaT", deltaTime);
  int drawType;
  switch (interactionConfig.interactionType) {
    case MouseInteraction::Draw: drawType = 1; break;
    case MouseInteraction::Erase: drawType = 2; break;
    default: drawType = 0; break;
  }
  if (!mouseInteractionActive) { drawType = 0; }
  diffuseTrailProgram->set1i("mouseDrawType", drawType);
  diffuseTrailProgram->set2fv("mousePosition", &attractorPosition[0]);
  diffuseTrailProgram->set1f("mouseDrawDistance", interactionConfig.distance);
  diffuseTrailProgram->set1f("mouseDrawPower", interactionConfig.power);
  diffuseTrailProgram->set1i("mouseDrawSpecies", interactionConfig.interactedSpecies);
  diffuseTrailProgram->set1i("mouseDrawFalloff", interactionConfig.enableDrawFalloff ? 1 : 0);
  trailTexture->bindImage(0);
  trailDiffuseTexture->bindImage(1);
  speciesDiffuseSettingsBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 2);
  diffuseTrailProgram->dispatch(textureSize.x / 8, textureSize.y / 8, diffuseSpeciesSettings.size());
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
  std::swap(trailTexture, trailDiffuseTexture);
}

const std::shared_ptr<Buffer> &PhysarumSimulator::getParticleBuffer() const { return particleBuffer; }

const std::shared_ptr<Texture> &PhysarumSimulator::getTrailTexture() const { return trailTexture; }

void PhysarumSimulator::initialize(const std::vector<PopulationConfig> &populations) {
  simSpeciesSettings.clear();
  diffuseSpeciesSettings.clear();
  speciesInteractionSettings.clear();

  std::vector<Particle> allParticles;
  totalParticleCount = ranges::accumulate(populations, 0, std::plus<>{}, &PopulationConfig::particleCount);
  allParticles.reserve(totalParticleCount);
  const auto populationsCount = populations.size();
  std::unique_ptr<ParticleGenerator> generator = nullptr;
  std::vector<int> speciesParticleOffsets;
  std::uint32_t speciesID = 0;
  std::ranges::for_each(populations, [&](const PopulationConfig &population) {
    details::SpeciesShaderSettings simSettings{population};
    simSettings.particlesOffset = allParticles.size();
    simSpeciesSettings.emplace_back(simSettings);
    details::SpeciesShaderDiffuseSettings diffSettings{population};
    diffuseSpeciesSettings.emplace_back(diffSettings);
    std::ranges::for_each(population.speciesInteractions, [&](const SpeciesInteractionConfig &c) {
      speciesInteractionSettings.emplace_back(static_cast<int>(c.interactionType), c.factor);
    });

    switch (population.particleStart) {
      case ParticleStart::Random: generator = std::make_unique<RandomParticleGenerator>(textureSize); break;
      case ParticleStart::Point: generator = std::make_unique<PointParticleGenerator>(textureSize / 2u); break;
      case ParticleStart::InwardCircle: generator = std::make_unique<InwardCircleParticleGenerator>(textureSize); break;
      case ParticleStart::RandomCircle: generator = std::make_unique<RandomCircleParticleGenerator>(textureSize); break;
      case ParticleStart::Uniform:
        generator = std::make_unique<UniformParticleGenerator>(textureSize, population.senseDistance);
        break;// TODO: step
    }
    const auto particles = generator->generateParticles(population.particleCount, speciesID);
    std::ranges::copy(particles, std::back_inserter(allParticles));
    ++speciesID;
  });
  currentParticleCapacity = allParticles.size() + EXTRA_PARTICLE_ALLOC;
  particleBuffer = std::make_shared<Buffer>(currentParticleCapacity * sizeof(Particle), nullptr, GL_DYNAMIC_DRAW);
  particleBuffer->setData(allParticles);

  // TODO: fix zero init
  trailTexture =
      std::make_shared<Texture>(GL_TEXTURE_2D_ARRAY, GL_R32F, 0, textureSize.x, textureSize.y, populationsCount);
  trailTexture->texParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  trailTexture->texParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  trailTexture->clear(0, GL_R32F, GL_FLOAT);

  trailDiffuseTexture =
      std::make_shared<Texture>(GL_TEXTURE_2D_ARRAY, GL_R32F, 0, textureSize.x, textureSize.y, populationsCount);
  trailDiffuseTexture->texParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  trailDiffuseTexture->texParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  trailDiffuseTexture->clear(0, GL_R32F, GL_FLOAT);

  speciesSettingsBuffer = std::make_shared<Buffer>(sizeof(details::SpeciesShaderSettings) * simSpeciesSettings.size(),
                                                   simSpeciesSettings.data());
  speciesDiffuseSettingsBuffer = std::make_shared<Buffer>(
      sizeof(details::SpeciesShaderDiffuseSettings) * diffuseSpeciesSettings.size(), diffuseSpeciesSettings.data());

  speciesInteractionBuffer =
      std::make_shared<Buffer>(sizeof(details::SpeciesShaderInteractionSettings) * speciesInteractionSettings.size(),
                               speciesInteractionSettings.data());
}

void PhysarumSimulator::updateConfig(const PopulationConfig &config, std::size_t index) {
  const auto simSettingData = speciesSettingsBuffer->map();
  auto unmapSim = RAII{[&] { speciesSettingsBuffer->unmap(); }};
  auto simSettings =
      std::span{reinterpret_cast<details::SpeciesShaderSettings *>(simSettingData), simSpeciesSettings.size()};
  details::SpeciesShaderSettings newSimSettings{config};
  newSimSettings.particleCount = simSpeciesSettings[index].particleCount;
  newSimSettings.particlesOffset = simSpeciesSettings[index].particlesOffset;
  simSpeciesSettings[index] = newSimSettings;
  simSettings[index] = newSimSettings;

  const auto diffSettingData = speciesDiffuseSettingsBuffer->map();
  auto unmapDiff = RAII{[&] { speciesDiffuseSettingsBuffer->unmap(); }};
  auto diffSettings = std::span{reinterpret_cast<details::SpeciesShaderDiffuseSettings *>(diffSettingData),
                                diffuseSpeciesSettings.size()};
  details::SpeciesShaderDiffuseSettings newDiffSettings{config};
  diffuseSpeciesSettings[index] = newDiffSettings;
  diffSettings[index] = newDiffSettings;

  const auto interactionData = speciesInteractionBuffer->map();
  auto unmapInteraction = RAII{[&] { speciesInteractionBuffer->unmap(); }};
  auto interSettings = std::span{reinterpret_cast<details::SpeciesShaderInteractionSettings *>(interactionData),
                                 simSpeciesSettings.size() * simSpeciesSettings.size()};
  for (std::size_t i = 0; i < config.speciesInteractions.size(); ++i) {
    details::SpeciesShaderInteractionSettings newSettings{
        static_cast<int>(config.speciesInteractions[i].interactionType), config.speciesInteractions[i].factor};
    interSettings[simSpeciesSettings.size() * index + i] = newSettings;
  }
}

void PhysarumSimulator::setAttractorPosition(const glm::vec2 &attractorPosition) {
  PhysarumSimulator::attractorPosition = attractorPosition;
}

void PhysarumSimulator::setInteractionConfig(const InteractionConfig &interactionConfig) {
  PhysarumSimulator::interactionConfig = interactionConfig;
}

void PhysarumSimulator::setMouseInteractionActive(bool mouseInteractionActive) {
  PhysarumSimulator::mouseInteractionActive = mouseInteractionActive;
}

void PhysarumSimulator::addParticles(std::span<Particle> particles) {
  if (totalParticleCount + particles.size() >= currentParticleCapacity) {
    auto oldBuffer = std::move(particleBuffer);
    const auto newBufferCapacity = totalParticleCount + particles.size() + EXTRA_PARTICLE_ALLOC;
    auto newBuffer = std::make_shared<Buffer>(newBufferCapacity * sizeof(Particle), nullptr, GL_DYNAMIC_DRAW);
    glCopyNamedBufferSubData(oldBuffer->getId(), newBuffer->getId(), 0, 0, totalParticleCount * sizeof(Particle));
    particleBuffer = std::move(newBuffer);
    currentParticleCapacity = newBufferCapacity;
  }
  glNamedBufferSubData(particleBuffer->getId(), totalParticleCount * sizeof(Particle),
                       particles.size() * sizeof(Particle), particles.data());
}

details::SpeciesShaderSettings::SpeciesShaderSettings(const PopulationConfig &src) {
  senseAngle = src.senseAngle;
  senseDistance = src.senseDistance;
  turnSpeed = src.turnSpeed;
  particleCount = src.particleCount;
  movementSpeed = src.movementSpeed;
  trailWeight = src.trailWeight;
  maxTrailValue = src.maxTrailValue;
  sensorSize = src.sensorSize;
  maxSteerRandomness = src.maxSteerRandomness;
}

details::SpeciesShaderDiffuseSettings::SpeciesShaderDiffuseSettings(const PopulationConfig &src) {
  kernelSize = src.blurKernelSize;
  diffuseRate = src.diffuseRate;
  decayRate = src.decayRate;
  filterType = static_cast<int>(src.filterType);
}
}// namespace pf::physarum