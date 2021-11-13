//
// Created by xflajs00 on 01.11.2021.
//

#include "RandomParticleGenerator.h"
#include <numbers>
#include <utils/rand.h>

namespace pf::physarum {

RandomParticleGenerator::RandomParticleGenerator(glm::uvec2 imageSize) : size(imageSize) {}

std::vector<Particle> RandomParticleGenerator::generateParticles(std::size_t count) {
  auto result = std::vector<Particle>{};
  for (std::size_t i = 0; i < count; ++i) {
    const auto rndAngle = fastRandom(0.f, 1.f) * std::numbers::pi_v<float> * 2.f;
    result.emplace_back(
        glm::vec2{fastRandom(0.f, static_cast<float>(size.x)), fastRandom(0.f, static_cast<float>(size.y))}, rndAngle);
  }
  return result;
}

}// namespace pf::physarum
