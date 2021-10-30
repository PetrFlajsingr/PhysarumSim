#pragma once

#include <geGL/OpenGL.h>
#include <geGL/OpenGLContext.h>

void setDefaultDebugMessage();
void setLowAndGreaterDebugMessage();
void setMediumAndGreaterDebugMessage();
void setHighDebugMessage();
void setDebugMessage(
    GLDEBUGPROC fce,
    void *data);
