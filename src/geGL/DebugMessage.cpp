#include <cassert>
#include <geGL/DebugMessage.h>
#include <geGL/OpenGLUtil.h>
#include <string>

void defaultDebugMessage(
    GLenum source,
    GLenum type,
    GLuint /*id*/,
    GLenum severity,
    GLsizei /*length*/,
    const GLchar *message,
    void * /*userParam*/) {
  std::cerr << "source: " << translateDebugSource(source) << " type: " << translateDebugType(type) << " severity: " << translateDebugSeverity(severity) << " : " << message << std::endl;
}
void lowDebugMessage(
    GLenum source,
    GLenum type,
    GLuint /*id*/,
    GLenum severity,
    GLsizei /*length*/,
    const GLchar *message,
    void * /*userParam*/) {
  if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;
  std::cerr << "source: " << translateDebugSource(source) << " type: " << translateDebugType(type) << " severity: " << translateDebugSeverity(severity) << " : " << message << std::endl;
}
void mediumDebugMessage(
    GLenum source,
    GLenum type,
    GLuint /*id*/,
    GLenum severity,
    GLsizei /*length*/,
    const GLchar *message,
    void * /*userParam*/) {
  if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;
  if (severity == GL_DEBUG_SEVERITY_LOW) return;
  std::cerr << "source: " << translateDebugSource(source) << " type: " << translateDebugType(type) << " severity: " << translateDebugSeverity(severity) << " : " << message << std::endl;
}
void highDebugMessage(
    GLenum source,
    GLenum type,
    GLuint /*id*/,
    GLenum severity,
    GLsizei /*length*/,
    const GLchar *message,
    void * /*userParam*/) {
  if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;
  if (severity == GL_DEBUG_SEVERITY_LOW) return;
  if (severity == GL_DEBUG_SEVERITY_MEDIUM) return;
  std::cerr << "source: " << translateDebugSource(source) << " type: " << translateDebugType(type) << " severity: " << translateDebugSeverity(severity) << " : " << message << std::endl;
}

/**
 * @brief sets debug function - it will report GL_DEBUG_SEVERITY_LOW/MEDIUM/HIGH
 */
void setLowAndGreaterDebugMessage() {
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback((GLDEBUGPROC) lowDebugMessage, NULL);
}

/**
 * @brief sets debug function - it will report GL_DEBUG_SEVERITY_MEDIUM/HIGH
 */
void setMediumAndGreaterDebugMessage() {
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback((GLDEBUGPROC) mediumDebugMessage, NULL);
}

/**
 * @brief sets debug function - it will report only GL_DEBUG_SEVERITY_HIGH errors
 */
void setHighDebugMessage() {
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback((GLDEBUGPROC) highDebugMessage, NULL);
}

/**
 * @brief sets default debug function - it will report everything
 */
void setDefaultDebugMessage() {
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback((GLDEBUGPROC) defaultDebugMessage, NULL);
}

/**
 * @brief sets callback function for OpenGL debugging
 *
 * @param fce callback function
 * @param data callback user data
 */
void setDebugMessage(
    GLDEBUGPROC fce,
    void *data) {
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(fce, data);
}
