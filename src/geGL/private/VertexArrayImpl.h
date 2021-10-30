#pragma once

#include <geGL/OpenGL.h>
#include <vector>

class VertexArrayImpl {
 public:
  VertexArrayImpl(VertexArray *v);
  ~VertexArrayImpl();
  size_t getNofBufferUsages(Buffer const *buffer) const;
  GLint getAttrib(GLuint index, GLenum pname) const;
  void addElementBuffer(Buffer *buffer);
  void removeReferencesFromBuffers();
  void removeReferenceFromElementBuffer();
  void removeReferenceFromElementBufferIfItIsOnlyReference();
  void addReferenceToBuffer(Buffer *buffer) const;
  void resizeBuffersForIndex(GLuint index);
  std::vector<Buffer *> buffers;
  Buffer *elementBuffer = nullptr;
  VertexArray *vao = nullptr;
};
