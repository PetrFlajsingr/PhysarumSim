//
// Created by xflajs00 on 01.11.2021.
//

#ifndef PHYSARUMSIM_SRC_SIMULATION_RANDOMPARTICLEGENERATOR_H
#define PHYSARUMSIM_SRC_SIMULATION_RANDOMPARTICLEGENERATOR_H

#include "ParticleGenerator.h"
#include <glm/ext/vector_uint2.hpp>

namespace pf::physarum {

class RandomParticleGenerator : public ParticleGenerator {
 public:
  explicit RandomParticleGenerator(glm::uvec2 imageSize);

  std::vector<Particle> generateParticles(std::size_t count) override;

 private:
  glm::uvec2 size;
};

}// namespace pf::physarum

#endif//PHYSARUMSIM_SRC_SIMULATION_RANDOMPARTICLEGENERATOR_H
