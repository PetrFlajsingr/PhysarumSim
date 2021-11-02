//
// Created by xflajs00 on 02.11.2021.
//

#ifndef PHYSARUMSIM_SRC_SIMULATION_SIMCONFIG_H
#define PHYSARUMSIM_SRC_SIMULATION_SIMCONFIG_H

#include <glm/vec4.hpp>

namespace pf::physarum {

enum class ParticleStart {
  Random, Point
};

struct SimConfig {
  float senseAngle;
  float senseDistance;
  float turnSpeed;
  float movementSpeed;
  float trailWeight;
  int blurKernelSize;
  float diffuseRate;
  float decayRate;
  ParticleStart particleStart;
  int particleCount;
};

}// namespace pf::physarum

#endif//PHYSARUMSIM_SRC_SIMULATION_SIMCONFIG_H
