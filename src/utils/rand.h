//
// Created by xflajs00 on 30.10.2021.
//

#ifndef PHYSARUMSIM_SRC_UTILS_RAND_H
#define PHYSARUMSIM_SRC_UTILS_RAND_H

#include <glm/vec3.hpp>
#include <numbers>
#include <random>

namespace details {
static inline std::random_device rd{};
static inline std::mt19937 slowEng(rd());

static inline std::minstd_rand fastEng(rd());
}// namespace details
/*
inline float random(float min, float max) {
  std::uniform_real_distribution<> distr(min, max);
  return distr(details::slowEng);
}*/

inline float fastRandom(float min, float max) {
  std::uniform_real_distribution<> distr(min, max);
  return distr(details::fastEng);
}
/*
inline glm::vec2 randomUnitCircle() {
  const auto theta = 2 * std::numbers::pi_v<float> * random(0.f, 1.f);
  const auto r = std::sqrt(random(0.f, 1.f));
  return {r * std::cos(theta), r * sin(theta)};
}
*/
inline glm::vec2 fastRandomUnitCircle() {
  const auto theta = 2 * std::numbers::pi_v<float> * fastRandom(0.f, 1.f);
  const auto r = std::sqrt(fastRandom(0.f, 1.f));
  return {r * std::cos(theta), r * sin(theta)};
}
/*
inline glm::vec3 randomVec3(float min, float max) {
  return {random(min, max), random(min, max), random(min, max)};
}
*/
inline glm::vec3 fastRandomVec3(float min, float max) {
  return {fastRandom(min, max), fastRandom(min, max), fastRandom(min, max)};
}

#endif//PHYSARUMSIM_SRC_UTILS_RAND_H
