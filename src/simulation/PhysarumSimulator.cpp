//
// Created by xflajs00 on 30.10.2021.
//

#include "PhysarumSimulator.h"
#include <glm/geometric.hpp>
#include <pf_common/RAII.h>
#include <simulation/generators/InwardCircleParticleGenerator.h>
#include <simulation/generators/RandomCircleParticleGenerator.h>

namespace pf::physarum {

void PhysarumSimulator::simulate(float currentTime, float deltaTime) {
  simulateProgram->use();
  simulateProgram->set1f("deltaT", deltaTime);
  simulateProgram->set1f("time", currentTime);
  simulateProgram->set1i("enableAttractor", attractorEnabled ? 1 : 0);
  simulateProgram->set2fv("attractorPosition", &attractorPosition[0]);
  particleBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
  trailTexture->bindImage(1);
  speciesSettingsBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 2);
  simulateProgram->dispatch(config.particleCount / 64 + 1, 1, 1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  diffuseTrailProgram->use();
  diffuseTrailProgram->set1f("deltaT", deltaTime);
  diffuseTrailProgram->set1i("kernelSize", config.blurKernelSize);
  diffuseTrailProgram->set1f("diffuseRate", config.diffuseRate);
  diffuseTrailProgram->set1f("decayRate", config.decayRate);
  diffuseTrailProgram->set1i("filterType", static_cast<int>(config.filterType));
  trailTexture->bindImage(0);
  trailDiffuseTexture->bindImage(1);
  diffuseTrailProgram->dispatch(textureSize.x / 8, textureSize.y / 8, 1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
  std::swap(trailTexture, trailDiffuseTexture);
}

const std::shared_ptr<Buffer> &PhysarumSimulator::getParticleBuffer() const {
  return particleBuffer;
}

const std::shared_ptr<Texture> &PhysarumSimulator::getTrailTexture() const {
  return trailTexture;
}

const PopulationConfig &PhysarumSimulator::getConfig() const {
  return config;
}

void PhysarumSimulator::setConfig(const PopulationConfig &config) {
  PhysarumSimulator::config = config;
  const auto shaderConfig = details::SpeciesShaderSettings{config};
  speciesSettingsBuffer->setData(&shaderConfig, sizeof(details::SpeciesShaderSettings));
}

void PhysarumSimulator::reinit(const PopulationConfig &config) {
  std::unique_ptr<ParticleGenerator> generator = nullptr;
  switch (config.particleStart) {
    case ParticleStart::Random: generator = std::make_unique<RandomParticleGenerator>(textureSize); break;
    case ParticleStart::Point: generator = std::make_unique<PointParticleGenerator>(textureSize / 2u); break;
    case ParticleStart::InwardCircle: generator = std::make_unique<InwardCircleParticleGenerator>(textureSize); break;
    case ParticleStart::RandomCircle: generator = std::make_unique<RandomCircleParticleGenerator>(textureSize); break;
  }
  const auto particles = generator->generateParticles(config.particleCount);
  particleBuffer = std::make_shared<Buffer>(particles.size() * sizeof(Particle), particles.data());

  std::vector<float> empty(textureSize.x * textureSize.y, 0);
  trailTexture->setData3D(empty.data());
  //trailTexture->setData2D(empty.data());
  const auto shaderConfig = details::SpeciesShaderSettings{config};
  speciesSettingsBuffer = std::make_shared<Buffer>(sizeof(details::SpeciesShaderSettings), &shaderConfig);
}

void PhysarumSimulator::restart(const PopulationConfig &config) {
  setConfig(config);
  reinit(config);
}

void PhysarumSimulator::setAttractorPosition(const glm::vec2 &attractorPosition) {
  PhysarumSimulator::attractorPosition = attractorPosition;
}

void PhysarumSimulator::setAttractorEnabled(bool attractorEnabled) {
  PhysarumSimulator::attractorEnabled = attractorEnabled;
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
}

}// namespace pf::physarum