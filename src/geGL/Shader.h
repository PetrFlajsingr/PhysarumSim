#pragma once

#include "OpenGLObject.h"
#include <cassert>
#include <iostream>
#include <memory>
#include <set>
#include <vector>

class Shader : public OpenGLObject {
  friend class Program;

 public:
  using Source = std::string;
  using Sources = std::vector<Source>;
  Shader();
  Shader(GLenum type, Sources const &sources = {});
  virtual ~Shader();
  void create(GLenum type);
  void setSource(Sources const &sources = {});
  void compile(Sources const &sources = {});
  GLboolean isShader() const;
  GLenum getType() const;
  GLboolean getDeleteStatus() const;
  GLboolean getCompileStatus() const;
  GLuint getInfoLogLength() const;
  GLuint getSourceLength() const;
  std::string getInfoLog() const;
  Source getSource() const;
  static std::string define(std::string const &name);
  static std::string define(std::string const &name, uint32_t value);
  static std::string define(std::string const &name, uint32_t value0, uint32_t value1);
  static std::string define(std::string const &name, uint32_t value0, uint32_t value1, uint32_t value2);
  static std::string define(std::string const &name, uint32_t value0, uint32_t value1, uint32_t value2, uint32_t value3);
  static std::string define(std::string const &name, uint32_t vectorSize, uint32_t const *values);
  static std::string define(std::string const &name, int32_t value);
  static std::string define(std::string const &name, int32_t value0, int32_t value1);
  static std::string define(std::string const &name, int32_t value0, int32_t value1, int32_t value2);
  static std::string define(std::string const &name, int32_t value0, int32_t value1, int32_t value2, int32_t value3);
  static std::string define(std::string const &name, uint32_t vectorSize, int32_t const *values);
  static std::string define(std::string const &name, float value);
  static std::string define(std::string const &name, float value0, float value1);
  static std::string define(std::string const &name, float value0, float value1, float value2);
  static std::string define(std::string const &name, float value0, float value1, float value2, float value3);
  static std::string define(std::string const &name, uint32_t vectorSize, float const *values);
  static std::string define(std::string const &name, std::string const &value);
  template<typename... ARGS>
  Shader(GLenum type, ARGS... args);
  template<typename... ARGS>
  void setSource(ARGS const &...sources);
  template<typename... ARGS>
  void compile(ARGS const &...sources);

 protected:
  GLint _getParam(GLenum pname) const;
  ShaderImpl *impl = nullptr;
};

template<typename... ARGS>
Shader::Shader(GLenum type, ARGS... args) : Shader(type, Sources({args...})) {
}

template<typename... ARGS>
void Shader::setSource(ARGS const &...sources) {
  assert(this != nullptr);
  this->setSource(Sources({sources...}));
}

template<typename... ARGS>
void Shader::compile(ARGS const &...sources) {
  assert(this != nullptr);
  this->compile(Sources({sources...}));
}
