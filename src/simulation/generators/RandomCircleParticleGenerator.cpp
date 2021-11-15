//
// Created by xflajs00 on 02.11.2021.
//

#include "RandomCircleParticleGenerator.h"
#include <utils/rand.h>
#include <glm/common.hpp>

namespace pf::physarum {

RandomCircleParticleGenerator::RandomCircleParticleGenerator(const glm::ivec2 &size) : size(size) {}

std::vector<Particle> RandomCircleParticleGenerator::generateParticles(std::size_t count) {
  auto result = std::vector<Particle>{};
  const auto center = glm::vec2{size / 2};
  const auto radius = glm::min(static_cast<float>(size.x), static_cast<float>(size.y)) * 0.4f;
  for (std::size_t i = 0; i < count; ++i) {
    const auto pos = center + fastRandomUnitCircle() * radius;
    const auto angle = fastRandom(0.f, 1.f) * std::numbers::pi_v<float> * 2.f;
    result.emplace_back(pos, angle);
  }
  return result;
}

}// namespace pf::physarum