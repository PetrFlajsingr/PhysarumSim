//
// Created by xflajs00 on 01.11.2021.
//

#ifndef PHYSARUMSIM_SRC_SIMULATION_POINTPARTICLEGENERATOR_H
#define PHYSARUMSIM_SRC_SIMULATION_POINTPARTICLEGENERATOR_H

#include "ParticleGenerator.h"
#include <glm/ext/vector_uint2.hpp>

namespace pf::physarum {

class PointParticleGenerator : public ParticleGenerator {
 public:
  explicit PointParticleGenerator(glm::uvec2 point);

  std::vector<Particle> generateParticles(std::size_t count, std::uint32_t speciesID) override;

 private:
  glm::uvec2 point;
};

}// namespace pf::physarum

#endif//PHYSARUMSIM_SRC_SIMULATION_POINTPARTICLEGENERATOR_H
