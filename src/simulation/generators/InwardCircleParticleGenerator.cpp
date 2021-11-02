//
// Created by xflajs00 on 02.11.2021.
//

#include "InwardCircleParticleGenerator.h"
#include <cmath>
#include <glm/geometric.hpp>
#include <utils/rand.h>

namespace pf::physarum {

InwardCircleParticleGenerator::InwardCircleParticleGenerator(const glm::ivec2 &size) : size(size) {}

std::vector<Particle> pf::physarum::InwardCircleParticleGenerator::generateParticles(std::size_t count) {
  auto result = std::vector<Particle>{};
  const auto center = glm::vec2{size / 2};
  for (std::size_t i = 0; i < count; ++i) {
    const auto pos = center + randomUnitCircle() * static_cast<float>(size.y) * .5f;
    const auto angle = std::atan2(glm::normalize(center - pos).y, glm::normalize(center - pos).x);

    result.emplace_back(pos, angle);
  }
  return result;
}

}