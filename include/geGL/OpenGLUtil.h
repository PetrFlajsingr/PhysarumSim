#pragma once

#include <array>
#include <geGL/OpenGL.h>
#include <iostream>
#include <string>

std::string translateDebugSource(GLenum source);
std::string translateDebugType(GLenum type);
std::string translateDebugSeverity(GLenum severity);

std::string translateBufferTarget(GLenum target);
std::string translateBufferBinding(GLenum binding);
GLenum bufferTarget2Binding(GLenum target);
GLenum bufferBinding2Target(GLenum binding);
bool areBufferFlagsMutable(GLbitfield flags);

std::string translateFramebufferComponentType(GLenum type);
std::string translateFramebufferType(GLenum type);
std::string translateFramebufferAttachment(GLenum attachment);
std::string translateFramebufferColorEncoding(GLenum type);
std::string translateCubeMapFace(GLenum face);

GLenum textureTarget2Binding(GLenum target);
GLenum textureBinding2Target(GLenum binding);
std::string translateTextureTarget(GLenum target);
std::string translateTextureBinding(GLenum binding);
std::string translateTextureCompareMode(GLint mode);
std::string translateTextureCompareFunc(GLint func);
std::string translateTextureFilter(GLint filter);
std::string translateTextureWrap(GLint wrap);
std::string translateTextureSwizzle(GLint swizzle);
std::string translateTextureChannelType(GLenum type);
unsigned internalFormatSize(GLenum internalFormat);

struct DepthInternalFormatElement {
  GLenum format;
  std::string name;
  std::string namePadding;
  GLenum baseFormat;
  size_t depthSize;
  bool depthFloating;
  size_t stencilSize;
  bool reqFormat;
};

bool isInternalFormatDepth(GLenum internalFormat);
DepthInternalFormatElement getDepthInternalFormatInformation(GLenum internalFormat);

struct CompressedInternalFormatElement {
  GLenum format;
  std::string name;
  std::string namePadding;
  GLenum baseFormat;
  bool generic;
  enum BorderType {
    UNORM,
    SNORM,
    FLOAT,
  } borderType;
};

bool isInternalFormatCompressed(GLenum internalFormat);
CompressedInternalFormatElement getCompressedInternalFormatInformation(GLenum internalFormat);

struct BasicInternalFormatElement {
  GLenum format;
  std::string name;
  std::string namePadding;
  GLenum baseFormat;
  size_t nofChannels;
  size_t channelSize[4];
  bool colorRenderable;
  bool reqRend;
  bool reqTex;
  enum Type {
    FLOAT,
    FIXED_POINT,
    UNSIGNED_FIXED_POINT,
    INT,
    UNSIGNED_INT,
  } type;
};

bool isInternalFormatBasic(GLenum internalFormat);
BasicInternalFormatElement getBasicInternalFormatInformation(GLenum internalFormat);

std::string translateInternalFormat(GLenum internalFormat);
std::string translateImageFormatCompatibilityType(GLenum type);

std::string translateAttribType(GLenum type);
std::string translateBoolean(GLboolean boolean);
unsigned getTypeSize(GLenum type);

std::string translateUniformType(GLenum type);
std::string translateBufferProperty(GLenum property);

bool isSamplerType(GLenum type);
