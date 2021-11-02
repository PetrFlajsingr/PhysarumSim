//
// Created by xflajs00 on 02.11.2021.
//

#ifndef PHYSARUMSIM_SRC_SIMULATION_SIMCONFIG_H
#define PHYSARUMSIM_SRC_SIMULATION_SIMCONFIG_H

#include <glm/vec4.hpp>
#include <magic_enum.hpp>
#include <toml++/toml.h>

namespace pf::physarum {

enum class ParticleStart {
  Random,
  Point
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

  inline static SimConfig FromToml(const toml::table &src) {
    return {
        .senseAngle = src["senseAngle"].value<float>().value(),
        .senseDistance = src["senseDistance"].value<float>().value(),
        .turnSpeed = src["turnSpeed"].value<float>().value(),
        .movementSpeed = src["movementSpeed"].value<float>().value(),
        .trailWeight = src["trailWeight"].value<float>().value(),
        .blurKernelSize = src["blurKernelSize"].value<int>().value(),
        .diffuseRate = src["diffuseRate"].value<float>().value(),
        .decayRate = src["decayRate"].value<float>().value(),
        .particleStart = static_cast<ParticleStart>(src["particleStart"].value<int>().value()),
        .particleCount = src["particleCount"].value<int>().value()};
  }

  inline toml::table toToml() const {
    return toml::table{
        {{"senseAngle", senseAngle},
         {"senseDistance", senseDistance},
         {"turnSpeed", turnSpeed},
         {"movementSpeed", movementSpeed},
         {"trailWeight", trailWeight},
         {"blurKernelSize", blurKernelSize},
         {"diffuseRate", diffuseRate},
         {"decayRate", decayRate},
         {"particleStart", static_cast<int>(particleStart)},
         {"particleCount", particleCount}}};
  }
};

}// namespace pf::physarum

#endif//PHYSARUMSIM_SRC_SIMULATION_SIMCONFIG_H
