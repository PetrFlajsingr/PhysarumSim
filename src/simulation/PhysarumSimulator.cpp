//
// Created by xflajs00 on 30.10.2021.
//

#include "PhysarumSimulator.h"
#include <glm/geometric.hpp>
#include <pf_common/RAII.h>
#include <simulation/generators/InwardCircleParticleGenerator.h>
#include <simulation/generators/RandomCircleParticleGenerator.h>

void pf::physarum::PhysarumSimulator::simulate(float currentTime, float deltaTime) {
  simulateProgram->use();
  simulateProgram->set1f("deltaT", deltaTime);
  simulateProgram->set1f("time", currentTime);
  simulateProgram->set1f("senseAngle", config.senseAngle);
  simulateProgram->set1f("senseDistance", config.senseDistance);
  simulateProgram->set1f("turnSpeed", config.turnSpeed);
  simulateProgram->set1f("movementSpeed", config.movementSpeed);
  simulateProgram->set1f("trailWeight", config.trailWeight);
  simulateProgram->set1f("maxTrailValue", config.maxTrailValue);
  simulateProgram->set1i("particleCount", static_cast<int>(particleCount));
  simulateProgram->set1i("sensorSize", config.sensorSize);
  trailTexture->bindImage(1);
  particleBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
  simulateProgram->dispatch(particleCount / 64 + 1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  diffuseTrailProgram->use();
  diffuseTrailProgram->set1f("deltaT", deltaTime);
  diffuseTrailProgram->set1i("kernelSize", config.blurKernelSize);
  diffuseTrailProgram->set1f("diffuseRate", config.diffuseRate);
  diffuseTrailProgram->set1f("decayRate", config.decayRate);
  trailTexture->bindImage(0);
  trailDiffuseTexture->bindImage(1);
  diffuseTrailProgram->dispatch(textureSize.x / 8, textureSize.y / 8);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
  std::swap(trailTexture, trailDiffuseTexture);
}

uint32_t pf::physarum::PhysarumSimulator::getParticleCount() const {
  return particleCount;
}

const std::shared_ptr<Buffer> &pf::physarum::PhysarumSimulator::getParticleBuffer() const {
  return particleBuffer;
}

const std::shared_ptr<Texture> &pf::physarum::PhysarumSimulator::getTrailTexture() const {
  return trailTexture;
}

const pf::physarum::SimConfig &pf::physarum::PhysarumSimulator::getConfig() const {
  return config;
}

void pf::physarum::PhysarumSimulator::setConfig(const pf::physarum::SimConfig &config) {
  PhysarumSimulator::config = config;
}

void pf::physarum::PhysarumSimulator::reinit(const pf::physarum::SimConfig &config) {
  particleCount = config.particleCount;
  std::unique_ptr<ParticleGenerator> generator = nullptr;
  switch (config.particleStart) {
    case ParticleStart::Random: generator = std::make_unique<RandomParticleGenerator>(textureSize); break;
    case ParticleStart::Point: generator = std::make_unique<PointParticleGenerator>(textureSize / 2u); break;
    case ParticleStart::InwardCircle: generator = std::make_unique<InwardCircleParticleGenerator>(textureSize); break;
    case ParticleStart::RandomCircle: generator = std::make_unique<RandomCircleParticleGenerator>(textureSize); break;
  }
  const auto particles = generator->generateParticles(particleCount);
  particleBuffer = std::make_shared<Buffer>(particles.size() * sizeof(Particle), particles.data());

  std::vector<float> empty(textureSize.x * textureSize.y, 0);
  trailTexture->setData2D(empty.data());
}

void pf::physarum::PhysarumSimulator::restart(const pf::physarum::SimConfig &config) {
  setConfig(config);
  reinit(config);
}

void pf::physarum::PhysarumSimulator::attractParticlesToPoint(glm::vec2 point) {
  // TODO: move to shader
  const auto voidPtr = particleBuffer->map();
  RAII unmap{[&] { particleBuffer->unmap();}};

  const auto particlePtr = reinterpret_cast<Particle*>(voidPtr);
  auto particleSpan = std::span{particlePtr, particleCount};
  for (auto &particle : particleSpan) {
    const auto distance = glm::distance(particle.position, point);
    if (distance > attractorDist) { continue; }
    const auto power = (1.f - distance / attractorDist) * 1.f;
    const auto direction = glm::normalize(point - particle.position);
    const auto posDelta = direction * power;
    particle.position += posDelta;
  }
}
