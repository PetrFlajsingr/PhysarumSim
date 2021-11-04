//
// Created by xflajs00 on 30.10.2021.
//

#ifndef PHYSARUMSIM_SRC_UTILS_RAND_H
#define PHYSARUMSIM_SRC_UTILS_RAND_H

#include <glm/vec3.hpp>
#include <numbers>
#include <random>

inline float random(float min, float max) {
  std::random_device rd;
  std::default_random_engine eng(rd());
  std::uniform_real_distribution<> distr(min, max);
  return distr(eng);
}

inline glm::vec2 randomUnitCircle() {
  const auto theta = 2 * std::numbers::pi_v<float> * random(0.f, 1.f);
  const auto r = std::sqrt(random(0.f, 1.f));
  return {r * std::cos(theta), r * sin(theta)};
}

inline glm::vec3 randomVec3(float min, float max) {
  return {random(min, max), random(min, max), random(min, max)};
}

#endif//PHYSARUMSIM_SRC_UTILS_RAND_H
