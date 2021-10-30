#pragma once

#include <geGL/OpenGLObject.h>
#include <iostream>
#include <memory>
#include <vector>

class VertexArray : public OpenGLObject {
 public:
  enum AttribPointerType { NONE,
                           I,
                           L };
  VertexArray();
  ~VertexArray();
  void addAttrib(Buffer *buffer,
                 GLuint index,
                 GLint nofComponentsa,
                 GLenum type,
                 GLsizei stride = 0,
                 GLintptr offset = 0,
                 GLboolean normalized = GL_FALSE,
                 GLuint divisor = 0,
                 AttribPointerType attribPointerType = NONE);
  void addElementBuffer(Buffer *buffer);
  void addAttrib(std::shared_ptr<Buffer> const &buffer,
                 GLuint index,
                 GLint nofComponentsa,
                 GLenum type,
                 GLsizei stride = 0,
                 GLintptr offset = 0,
                 GLboolean normalized = GL_FALSE,
                 GLuint divisor = 0,
                 AttribPointerType attribPointerType = NONE);
  void addElementBuffer(std::shared_ptr<Buffer> const &buffer);
  void removeAttrib(GLuint index);
  void removeElementBuffer();
  void bind() const;
  void unbind() const;
  GLuint getAttribBufferBinding(GLuint index) const;
  GLboolean isAttribEnabled(GLuint index) const;
  GLint getAttribSize(GLuint index) const;
  GLsizei getAttribStride(GLuint index) const;
  GLenum getAttribType(GLuint index) const;
  GLboolean isAttribNormalized(GLuint index) const;
  GLboolean isAttribInteger(GLuint index) const;
  GLboolean isAttribLong(GLuint index) const;
  GLuint getAttribDivisor(GLuint index) const;
  GLuint getAttribBinding(GLuint index) const;
  GLuint getAttribRelativeOffset(GLuint index) const;
  GLuint getElementBuffer() const;
  std::string getInfo() const;
  Buffer *getElement() const;
  Buffer *getBuffer(GLuint index) const;
  size_t getNofBuffers() const;

 protected:
  VertexArrayImpl *impl = nullptr;
  friend class VertexArrayImpl;
  friend class BufferImpl;
};
