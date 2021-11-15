//
// Created by xflajs00 on 02.11.2021.
//

#ifndef PHYSARUMSIM_SRC_SIMULATION_GENERATORS_RANDOMCIRCLEPARTICLEGENERATOR_H
#define PHYSARUMSIM_SRC_SIMULATION_GENERATORS_RANDOMCIRCLEPARTICLEGENERATOR_H

#include "ParticleGenerator.h"
#include <glm/ext/vector_uint2.hpp>

namespace pf::physarum {

class RandomCircleParticleGenerator : public ParticleGenerator {
 public:
  RandomCircleParticleGenerator(const glm::ivec2 &size);
  std::vector<Particle> generateParticles(std::size_t count, std::uint32_t speciesID) override;

 private:
  glm::ivec2 size;
};

}// namespace pf::physarum

#endif//PHYSARUMSIM_SRC_SIMULATION_GENERATORS_RANDOMCIRCLEPARTICLEGENERATOR_H
