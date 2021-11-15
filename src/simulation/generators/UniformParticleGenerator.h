//
// Created by xflajs00 on 07.11.2021.
//

#ifndef PHYSARUMSIM_SRC_SIMULATION_GENERATORS_UNIFORMPARTICLEGENERATOR_H
#define PHYSARUMSIM_SRC_SIMULATION_GENERATORS_UNIFORMPARTICLEGENERATOR_H

#include "ParticleGenerator.h"

namespace pf::physarum {

class UniformParticleGenerator : public ParticleGenerator {
 public:
  explicit UniformParticleGenerator(glm::uvec2 imageSize, float step);

  std::vector<Particle> generateParticles(std::size_t count, std::uint32_t speciesID) override;

 private:
  float step;
  glm::uvec2 size;
};

}// namespace pf::physarum

#endif//PHYSARUMSIM_SRC_SIMULATION_GENERATORS_UNIFORMPARTICLEGENERATOR_H
