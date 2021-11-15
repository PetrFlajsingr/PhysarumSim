#pragma once

#include <geGL/OpenGLContext.h>
#include <glad/glad.h>

class OpenGLObject {
 public:
  OpenGLObject(GLuint id = 0u);
  virtual ~OpenGLObject();
  GLuint getId() const;
  GLuint &getId();
  OpenGLObject(OpenGLObject const &) = delete;

 private:
  OpenGLObjectImpl *impl = nullptr;
};
