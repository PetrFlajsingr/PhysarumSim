//
// Created by xflajs00 on 01.11.2021.
//

#include "PointParticleGenerator.h"
#include <numbers>
#include <utils/rand.h>

namespace pf::physarum {

PointParticleGenerator::PointParticleGenerator(glm::uvec2 point) : point(point) {}
std::vector<Particle> PointParticleGenerator::generateParticles(std::size_t count) {
  auto result = std::vector<Particle>{};
  for (std::size_t i = 0; i < count; ++i) {
    result.emplace_back(glm::vec2{point}, fastRandom(0.f, 1.f) * std::numbers::pi_v<float> * 2.f);
  }
  return result;
}
}// namespace pf::physarum