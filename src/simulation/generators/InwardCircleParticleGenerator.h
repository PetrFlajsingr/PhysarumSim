//
// Created by xflajs00 on 02.11.2021.
//

#ifndef PHYSARUMSIM_SRC_SIMULATION_GENERATORS_INWARDCIRCLEPARTICLEGENERATOR_H
#define PHYSARUMSIM_SRC_SIMULATION_GENERATORS_INWARDCIRCLEPARTICLEGENERATOR_H

#include "ParticleGenerator.h"
#include <glm/ext/vector_uint2.hpp>

namespace pf::physarum {

class InwardCircleParticleGenerator : public ParticleGenerator {
 public:
  InwardCircleParticleGenerator(const glm::ivec2 &size);
  std::vector<Particle> generateParticles(std::size_t count) override;

 private:
  glm::ivec2 size;
};

}// namespace pf::physarum
#endif//PHYSARUMSIM_SRC_SIMULATION_GENERATORS_INWARDCIRCLEPARTICLEGENERATOR_H
