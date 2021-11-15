#pragma once

#include <geGL/Fwd.h>
#include <set>

class ShaderImpl {
 public:
  std::set<Program *> programs;
};
