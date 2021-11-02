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

class PhysarumSimulator {
 public:
  explicit PhysarumSimulator(const SimConfig &config,
                             const std::filesystem::path &shaderDir,
                             glm::uvec2 textureSize) : config(config),
                                                       textureSize(textureSize) {
    trailTexture = std::make_shared<Texture>(GL_TEXTURE_2D, GL_R32F, 0, textureSize.x, textureSize.y);
    trailTexture->texParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    trailTexture->texParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    const auto simShaderSrc = readFile(shaderDir / "physarum_sim.comp");
    if (!simShaderSrc.has_value()) {
      throw std::runtime_error("Could not load 'physarum_sim.comp'");
    }
    simulateShader = std::make_shared<Shader>(GL_COMPUTE_SHADER, simShaderSrc.value());
    simulateProgram = std::make_shared<Program>(simulateShader);

    const auto diffuseTrailShaderSrc = readFile(shaderDir / "diffuse_trail.comp");
    if (!diffuseTrailShaderSrc.has_value()) {
      throw std::runtime_error("Could not load 'diffuse_trail.comp'");
    }
    diffuseTrailShader = std::make_shared<Shader>(GL_COMPUTE_SHADER, diffuseTrailShaderSrc.value());
    diffuseTrailProgram = std::make_shared<Program>(diffuseTrailShader);

    reinit(config);
  }

  void simulate(float currentTime, float deltaTime);

  [[nodiscard]] uint32_t getParticleCount() const;
  [[nodiscard]] const std::shared_ptr<Buffer> &getParticleBuffer() const;
  [[nodiscard]] const std::shared_ptr<Texture> &getTrailTexture() const;

  [[nodiscard]] const SimConfig &getConfig() const;
  void setConfig(const SimConfig &config);

  void restart(const SimConfig &config);

  void attractParticlesToPoint(glm::vec2 point);

 private:
  void reinit(const SimConfig &config);

  SimConfig config;
  std::uint32_t particleCount;
  glm::uvec2 textureSize;
  float attractorDist = 500.f;

  std::shared_ptr<Buffer> particleBuffer;
  std::shared_ptr<Texture> trailTexture;

  // display in renderer - render trail texture to another texture and present on a quad

  // select most pheromone in 3 dirs
  // set direction toward that - limited by turnAngle
  // move in direction
  // save pheromone to trail
  std::shared_ptr<Shader> simulateShader;
  std::shared_ptr<Program> simulateProgram;
  // TODO: blur the trail - for now median filter, other filters later (blur)
  std::shared_ptr<Shader> diffuseTrailShader;
  std::shared_ptr<Program> diffuseTrailProgram;
};

}// namespace pf::physarum

#endif//PHYSARUMSIM_SRC_SIMULATION_PHYSARUMSIMULATOR_H
