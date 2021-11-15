//
// Created by xflajs00 on 30.10.2021.
//

#ifndef PHYSARUMSIM_SRC_SIMULATION_PARTICLE_H
#define PHYSARUMSIM_SRC_SIMULATION_PARTICLE_H

#include <glm/vec2.hpp>

namespace pf::physarum {

struct Particle {
  inline Particle(const glm::vec2 &position, float angleDirection, uint32_t speciesId)
      : position(position), angleDirection(angleDirection), speciesID(speciesId) {}
  glm::vec2 position;
  float angleDirection;
  std::uint32_t speciesID;
};

}// namespace pf::physarum

#endif//PHYSARUMSIM_SRC_SIMULATION_PARTICLE_H
