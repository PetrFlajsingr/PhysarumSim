//
// Created by xflajs00 on 30.10.2021.
//

#ifndef PHYSARUMSIM_SRC_UTILS_RAND_H
#define PHYSARUMSIM_SRC_UTILS_RAND_H

#include <random>

inline float random(float min, float max) {
  std::random_device rd;
  std::default_random_engine eng(rd());
  std::uniform_real_distribution<> distr(min, max);
  return distr(eng);
}

#endif//PHYSARUMSIM_SRC_UTILS_RAND_H
