#include <cassert>
#include <geGL/Buffer.h>
#include <geGL/OpenGLUtil.h>
#include <geGL/VertexArray.h>
#include <geGL/private/BufferImpl.h>
#include <geGL/private/VertexArrayImpl.h>
#include <sstream>

VertexArray::VertexArray()
    : OpenGLObject() {
  impl = new VertexArrayImpl(this);
}

/**
 * @brief Destroyes vertex array object
 */
VertexArray::~VertexArray() { delete impl; }

/**
 * @brief Adds vertex attrib into vertex array object
 *
 * @param buffer        a buffer where a attrib is stored
 * @param index         index of attrib layout(location=index)
 * @param nofComponents number of components of attrib vec3 = 3
 * @param type          type of attrib vec3 = float, ivec2 = int
 * @param stride        distance between attribs
 * @param offset        offset to the first attrib
 * @param normalized    should the attrib be normalized?
 * @param divisor       rate of incrementation of attrib per instance, 0 = per
 * VS invocation
 * @param apt           NONE - glVertexAttribPointer, I -
 * glVertexAttribIPointer, L - glVertexAttribLPointer
 */
void VertexArray::addAttrib(Buffer *buffer,
                            GLuint index,
                            GLint nofComponents,
                            GLenum type,
                            GLsizei stride,
                            GLintptr offset,
                            GLboolean normalized,
                            GLuint divisor,
                            AttribPointerType apt) {
  assert(this != nullptr);

  if (buffer == nullptr)
    throw std::invalid_argument(
        "geGL: VertexArray::addAttrib - buffer is nullptr");

  removeAttrib(index);

  if (stride == 0) stride = getTypeSize(type) * nofComponents;
  glVertexArrayAttribBinding(getId(), index, index);
  glEnableVertexArrayAttrib(getId(), index);

  if (apt == VertexArray::AttribPointerType::NONE)
    glVertexArrayAttribFormat(getId(), index, nofComponents, type,
                              normalized, 0);
  else if (apt == VertexArray::AttribPointerType::I)
    glVertexArrayAttribIFormat(getId(), index, nofComponents, type,
                               0);
  else if (apt == VertexArray::AttribPointerType::L)
    glVertexArrayAttribLFormat(getId(), index, nofComponents, type,
                               0);

  glVertexArrayVertexBuffer(getId(), index, buffer->getId(),
                            offset, stride);
  glVertexArrayBindingDivisor(getId(), index, divisor);
  impl->resizeBuffersForIndex(index);
  impl->buffers[index] = buffer;
  buffer->impl->vertexArrays.insert(this);
}

/**
 * @brief Adds vertex attrib into vertex array object
 *
 * @param buffer        a buffer where a attrib is stored
 * @param index         index of attrib layout(location=index)
 * @param nofComponents number of components of attrib vec3 = 3
 * @param type          type of attrib vec3 = float, ivec2 = int
 * @param stride        distance between attribs
 * @param offset        offset to the first attrib
 * @param normalized    should the attrib be normalized?
 * @param divisor       rate of incrementation of attrib per instance, 0 = per
 * VS invocation
 * @param apt           NONE - glVertexAttribPointer, I -
 * glVertexAttribIPointer, L - glVertexAttribLPointer
 */
void VertexArray::addAttrib(std::shared_ptr<Buffer> const &buffer,
                            GLuint index,
                            GLint nofComponents,
                            GLenum type,
                            GLsizei stride,
                            GLintptr offset,
                            GLboolean normalized,
                            GLuint divisor,
                            AttribPointerType apt) {
  addAttrib(&*buffer, index, nofComponents, type, stride, offset, normalized,
            divisor, apt);
}

void VertexArray::addElementBuffer(Buffer *buffer) {
  assert(this != nullptr);
  assert(impl != nullptr);
  impl->addElementBuffer(buffer);
}

void VertexArray::addElementBuffer(std::shared_ptr<Buffer> const &buffer) {
  assert(this != nullptr);
  addElementBuffer(&*buffer);
}

void VertexArray::removeAttrib(GLuint index) {
  assert(this != nullptr);
  assert(impl != nullptr);
  if (index >= impl->buffers.size()) return;
  if (!getBuffer(index)) return;

  glVertexArrayVertexBuffer(getId(), index, 0, 0, 0);

  auto const nofBufferUsages = impl->getNofBufferUsages(getBuffer(index));
  if (nofBufferUsages > 1) return;

  getBuffer(index)->impl->vertexArrays.erase(this);
  impl->buffers.at(index) = nullptr;
}

void VertexArray::removeElementBuffer() {
  assert(this != nullptr);
  assert(impl != nullptr);
  impl->removeReferenceFromElementBufferIfItIsOnlyReference();
  impl->elementBuffer = nullptr;
}

void VertexArray::bind() const {
  assert(this != nullptr);
  glBindVertexArray(getId());
}

void VertexArray::unbind() const {
  assert(this != nullptr);
  glBindVertexArray(0);
}

GLuint VertexArray::getAttribBufferBinding(GLuint index) const {
  assert(this != nullptr);
  assert(impl != nullptr);
  return impl->getAttrib(index, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING);
}

GLboolean VertexArray::isAttribEnabled(GLuint index) const {
  assert(this != nullptr);
  assert(impl != nullptr);
  return (GLboolean) impl->getAttrib(index, GL_VERTEX_ATTRIB_ARRAY_ENABLED);
}

GLint VertexArray::getAttribSize(GLuint index) const {
  assert(this != nullptr);
  assert(impl != nullptr);
  return impl->getAttrib(index, GL_VERTEX_ATTRIB_ARRAY_SIZE);
}

GLsizei VertexArray::getAttribStride(GLuint index) const {
  assert(this != nullptr);
  assert(impl != nullptr);
  return impl->getAttrib(index, GL_VERTEX_ATTRIB_ARRAY_STRIDE);
}

GLenum VertexArray::getAttribType(GLuint index) const {
  assert(this != nullptr);
  // return _getAttrib(index,GL_VERTEX_ATTRIB_ARRAY_TYPE);
  // AMD bug in 15.9 on Linux glGetVertexArrayIndexediv return
  // type's number instead of type
  GLuint oldId;
  glGetIntegerv(GL_VERTEX_ARRAY_BINDING, (GLint *) &oldId);
  glBindVertexArray(getId());
  GLint data;
  glGetVertexAttribiv(index, GL_VERTEX_ATTRIB_ARRAY_TYPE, &data);
  glBindVertexArray(oldId);
  return data;
}

GLboolean VertexArray::isAttribNormalized(GLuint index) const {
  assert(this != nullptr);
  assert(impl != nullptr);
  return (GLboolean) impl->getAttrib(index, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED);
}

GLboolean VertexArray::isAttribInteger(GLuint index) const {
  assert(this != nullptr);
  assert(impl != nullptr);
  return (GLboolean) impl->getAttrib(index, GL_VERTEX_ATTRIB_ARRAY_INTEGER);
}

GLboolean VertexArray::isAttribLong(GLuint index) const {
  assert(this != nullptr);
  assert(impl != nullptr);
  return (GLboolean) impl->getAttrib(index, GL_VERTEX_ATTRIB_ARRAY_LONG);
}

GLuint VertexArray::getAttribDivisor(GLuint index) const {
  assert(this != nullptr);
  assert(impl != nullptr);
  return impl->getAttrib(index, GL_VERTEX_ATTRIB_ARRAY_DIVISOR);
}

GLuint VertexArray::getAttribBinding(GLuint index) const {
  assert(this != nullptr);
  assert(impl != nullptr);
  return impl->getAttrib(index, GL_VERTEX_ATTRIB_BINDING);
}

GLuint VertexArray::getAttribRelativeOffset(GLuint index) const {
  assert(this != nullptr);
  assert(impl != nullptr);
  return impl->getAttrib(index, GL_VERTEX_ATTRIB_RELATIVE_OFFSET);
}

GLuint VertexArray::getElementBuffer() const {
  assert(this != nullptr);
  assert(impl != nullptr);
  GLint id;
  glGetVertexArrayiv(getId(), GL_ELEMENT_ARRAY_BUFFER_BINDING,
                     &id);
  return id;
}

std::string VertexArray::getInfo() const {
  assert(this != nullptr);
  std::stringstream ss;
  GLint maxVertexAttribs;
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs);
  ss << "vao: " << getId() << std::endl;
  ss << "GL_MAX_VERTEX_ATTRIBS: " << maxVertexAttribs << std::endl;
  for (GLint a = 0; a < maxVertexAttribs; ++a) {
    if (isAttribEnabled(a) == GL_FALSE) continue;
    ss << "attrib: " << a << std::endl;
    ss << "  GL_VERTEX_ATTRIB_ARRAY_ENABLED:        "
       << translateBoolean(
              /*                                         */ isAttribEnabled(a))
       << std::endl;
    ss << "  GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING: "
       << getAttribBufferBinding(a) << std::endl;
    ss << "  GL_VERTEX_ATTRIB_ARRAY_SIZE:           " << getAttribSize(a)
       << std::endl;
    ss << "  GL_VERTEX_ATTRIB_ARRAY_STRIDE:         " << getAttribStride(a)
       << std::endl;
    ss << "  GL_VERTEX_ATTRIB_ARRAY_TYPE:           "
       << translateAttribType(
              /*                                         */ getAttribType(a))
       << std::endl;
    ss << "  GL_VERTEX_ATTRIB_ARRAY_NORMALIZED:     "
       << translateBoolean(
              /*                                         */ isAttribNormalized(
                  a))
       << std::endl;
    ss << "  GL_VERTEX_ATTRIB_ARRAY_INTEGER:        "
       << translateBoolean(
              /*                                         */ isAttribInteger(a))
       << std::endl;
    ss << "  GL_VERTEX_ATTRIB_ARRAY_LONG:           "
       << translateBoolean(
              /*                                         */ isAttribLong(a))
       << std::endl;
    ss << "  GL_VERTEX_ATTRIB_ARRAY_DIVISOR:        " << getAttribDivisor(a)
       << std::endl;
    ss << "  GL_VERTEX_ATTRIB_BINDING:              " << getAttribBinding(a)
       << std::endl;
    ss << "  GL_VERTEX_ATTRIB_RELATIVE_OFFSET:      "
       << getAttribRelativeOffset(a) << std::endl;
  }
  return ss.str();
}

Buffer *VertexArray::getElement() const {
  assert(this != nullptr);
  assert(impl != nullptr);
  return impl->elementBuffer;
}

Buffer *VertexArray::getBuffer(GLuint index) const {
  assert(this != nullptr);
  assert(impl != nullptr);
  assert(index < impl->buffers.size());
  return impl->buffers.at(index);
}

size_t VertexArray::getNofBuffers() const {
  assert(this != nullptr);
  assert(impl != nullptr);
  return impl->buffers.size();
}

// OpenGL 3.3
// glBindBuffer
// glBindVertexArray
// glDeleteVertexArrays
// glEnableVertexAttribArray
// glGenVertexArrays
// glGetIntegerv
// glGetVertexAttribiv
// glVertexAttribPointer
// glVertexAttribIPointer

// OpenGL 4.2
// glVertexAttribDivisor
// glVertexAttribLPointer

//
// OpenGL 4.5
// glCreateVertexArrays
// glEnableVertexArrayAttrib
// glGetVertexArrayIndexediv
// glGetVertexArrayiv
// glVertexArrayAttribBinding
// glVertexArrayAttribFormat
// glVertexArrayAttribIFormat
// glVertexArrayAttribLFormat
// glVertexArrayBindingDivisor
// glVertexArrayElementBuffer
// glVertexArrayVertexBuffer
