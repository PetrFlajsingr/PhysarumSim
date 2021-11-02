//
// Created by xflajs00 on 01.11.2021.
//

#ifndef PHYSARUMSIM_SRC_SIMULATION_PARTICLEGENERATOR_H
#define PHYSARUMSIM_SRC_SIMULATION_PARTICLEGENERATOR_H

#include "../Particle.h"
#include <vector>

namespace pf::physarum {

class ParticleGenerator {
 public:
  virtual std::vector<Particle> generateParticles(std::size_t count) = 0;

  virtual ~ParticleGenerator() = default;
};

}// namespace pf::physarum

#endif//PHYSARUMSIM_SRC_SIMULATION_PARTICLEGENERATOR_H
