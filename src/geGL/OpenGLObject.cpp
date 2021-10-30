#include <cassert>
#include <geGL/OpenGLObject.h>

class OpenGLObjectImpl {
 public:
  GLuint id = 0u;///<object id

  OpenGLObjectImpl(GLuint id = 0u) : id(id) {}
  GLuint getId() const {
    return id;
  }
  GLuint &getId() {
    return id;
  }
};

OpenGLObject::OpenGLObject(
    GLuint id) {
  impl = new OpenGLObjectImpl(id);
}

OpenGLObject::~OpenGLObject() {
  assert(this != nullptr);
  delete impl;
}

GLuint OpenGLObject::getId() const {
  assert(this != nullptr);
  return this->impl->getId();
}

GLuint &OpenGLObject::getId() {
  assert(this != nullptr);
  return this->impl->getId();
}
