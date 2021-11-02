//
// Created by xflajs00 on 02.11.2021.
//

#include "RandomCircleParticleGenerator.h"
#include <utils/rand.h>

namespace pf::physarum {

RandomCircleParticleGenerator::RandomCircleParticleGenerator(const glm::ivec2 &size) : size(size) {}

std::vector<Particle> RandomCircleParticleGenerator::generateParticles(std::size_t count) {
  auto result = std::vector<Particle>{};
  const auto center = glm::vec2{size / 2};
  for (std::size_t i = 0; i < count; ++i) {
    const auto pos = center + randomUnitCircle() * static_cast<float>(size.y) * .15f;
    const auto angle = random(0.f, 1.f) * std::numbers::pi_v<float> * 2.f;
    result.emplace_back(pos, angle);
  }
  return result;
}

}// namespace pf::physarum