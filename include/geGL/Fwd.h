#pragma once

#include <memory>

class FunctionLoaderInterface;
using FunctionLoaderInterfacePointer = std::shared_ptr<FunctionLoaderInterface>;
class OpenGLObject;
class OpenGLObjectImpl;
class Buffer;
class BufferImpl;
class Program;
class ProgramImpl;
class Shader;
class ShaderImpl;
class Texture;
class VertexArray;
class VertexArrayImpl;
class AsynchronousQuery;
class Framebuffer;
class ProgramPipeline;
class Sampler;
class Renderbuffer;
