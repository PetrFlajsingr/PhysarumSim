#pragma once

#include <geGL/ProgramInfo.h>
#include <map>
#include <memory>
#include <set>

class ProgramImpl {
 public:
  using ShaderPointer = std::shared_ptr<Shader>;
  std::set<ShaderPointer> shaders;
  std::map<std::string, GLint> name2Uniform;
  std::shared_ptr<ProgramInfo> info;
};