//
// Created by xflajs00 on 30.10.2021.
//

#ifndef PHYSARUMSIM_SRC_SIMULATION_PHYSARUMSIMULATOR_H
#define PHYSARUMSIM_SRC_SIMULATION_PHYSARUMSIMULATOR_H

#include "Particle.h"
#include "SimConfig.h"
#include "generators/PointParticleGenerator.h"
#include "generators/RandomParticleGenerator.h"
#include <algorithm>
#include <filesystem>
#include <geGL/Buffer.h>
#include <geGL/Program.h>
#include <geGL/Shader.h>
#include <geGL/Texture.h>
#include <glm/ext/vector_uint2.hpp>
#include <range/v3/range/conversion.hpp>
#include <ranges>
#include <utils/files.h>

namespace pf::physarum {

constexpr inline static auto EXTRA_PARTICLE_ALLOC = 10000;

namespace details {
struct SpeciesShaderSettings {
  int particlesOffset;
  float senseAngle;
  float senseDistance;
  float turnSpeed;
  int particleCount;
  float movementSpeed;
  float trailWeight;
  float maxTrailValue;
  int sensorSize;
  float maxSteerRandomness;
  SpeciesShaderSettings() = default;
  SpeciesShaderSettings(const PopulationConfig &src);
};
struct SpeciesShaderDiffuseSettings {
  int kernelSize;
  float diffuseRate;
  float decayRate;
  int filterType;
  SpeciesShaderDiffuseSettings() = default;
  SpeciesShaderDiffuseSettings(const PopulationConfig &src);
};
struct SpeciesShaderInteractionSettings {
  int type;
  float factor;
};
}// namespace details

class PhysarumSimulator {
 public:
  explicit PhysarumSimulator(const std::filesystem::path &shaderDir, glm::uvec2 textureSize)
      : textureSize(textureSize) {

    const auto simShaderSrc = readFile(shaderDir / "physarum_sim.comp");
    if (!simShaderSrc.has_value()) { throw std::runtime_error("Could not load 'physarum_sim.comp'"); }
    simulateShader = std::make_shared<Shader>(GL_COMPUTE_SHADER, simShaderSrc.value());
    simulateProgram = std::make_shared<Program>(simulateShader);

    const auto diffuseTrailShaderSrc = readFile(shaderDir / "diffuse_trail.comp");
    if (!diffuseTrailShaderSrc.has_value()) { throw std::runtime_error("Could not load 'diffuse_trail.comp'"); }
    diffuseTrailShader = std::make_shared<Shader>(GL_COMPUTE_SHADER, diffuseTrailShaderSrc.value());
    diffuseTrailProgram = std::make_shared<Program>(diffuseTrailShader);
  }

  void initialize(const std::vector<PopulationConfig> &populations);
  void updateConfig(const PopulationConfig &config, std::size_t index);

  void simulate(float currentTime, float deltaTime);

  [[nodiscard]] const std::shared_ptr<Buffer> &getParticleBuffer() const;
  [[nodiscard]] const std::shared_ptr<Texture> &getTrailTexture() const;

  void setAttractorPosition(const glm::vec2 &attractorPosition);
  void setMouseInteractionActive(bool mouseInteractionActive);
  void setInteractionConfig(const InteractionConfig &interactionConfig);

  void addParticles(std::span<Particle> particles);

  [[nodiscard]] std::size_t getParticleCount() const {
    return totalParticleCount;
  }

 private:
  glm::uvec2 textureSize;

  glm::vec2 attractorPosition{};
  bool mouseInteractionActive = false;

  std::shared_ptr<Buffer> particleBuffer;
  std::shared_ptr<Texture> trailTexture;
  std::shared_ptr<Texture> trailDiffuseTexture;
  std::shared_ptr<Buffer> speciesSettingsBuffer;
  std::shared_ptr<Buffer> speciesDiffuseSettingsBuffer;
  std::shared_ptr<Buffer> speciesInteractionBuffer;

  std::shared_ptr<Shader> simulateShader;
  std::shared_ptr<Program> simulateProgram;

  std::shared_ptr<Shader> diffuseTrailShader;
  std::shared_ptr<Program> diffuseTrailProgram;

  std::vector<details::SpeciesShaderSettings> simSpeciesSettings;
  std::vector<details::SpeciesShaderDiffuseSettings> diffuseSpeciesSettings;
  std::vector<details::SpeciesShaderInteractionSettings> speciesInteractionSettings;
  int totalParticleCount;

  InteractionConfig interactionConfig;
  std::size_t currentParticleCapacity;
};

}// namespace pf::physarum

#endif//PHYSARUMSIM_SRC_SIMULATION_PHYSARUMSIMULATOR_H
