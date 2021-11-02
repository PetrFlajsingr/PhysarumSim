//
// Created by xflajs00 on 30.10.2021.
//

#ifndef PHYSARUMSIM_SRC_SIMULATION_PARTICLE_H
#define PHYSARUMSIM_SRC_SIMULATION_PARTICLE_H

#include <glm/vec2.hpp>

namespace pf::physarum {

struct Particle {
  glm::vec2 position;
  float angleDirection;
  float PADDING;
};

}

#endif//PHYSARUMSIM_SRC_SIMULATION_PARTICLE_H
