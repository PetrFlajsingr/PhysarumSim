/*!
 * geGL library contains 10 OpenGLObject.
 * All are derived from OpenGLObject.
 * OpenGLObject class is derived from FunctionProvider that provides OpenGL.
 *
 *                                ┌───────┐
 *                                │Context│
 *                                └────△──┘
 *                              ┌──────┴─────┐
 *                              │OpenGLObject│
 *                              └──────△─────┘
 *          ┌───────────────┬──────────┴──┬─────────────┬──────────┬─────┐
 * ┌────────┴────────┐   ┌──┴───┐   ┌─────┴─────┐   ┌───┴──┐   ┌───┴───┐ │
 * │AsynchronousQuery│   │Buffer│   │Framebuffer│   │Shader│   │Program│ │
 * └─────────────────┘   └──────┘   └───────────┘   └──────┘   └───────┘ │
 *         ┌────────────┬────────┬──────────┬─────────────┬──────────────┘
 * ┌───────┴───────┐┌───┴───┐┌───┴───┐┌─────┴──────┐┌─────┴─────┐
 * │ProgramPipeline││Texture││Sampler││Renderbuffer││VertexArray│
 * └───────────────┘└───────┘└───────┘└────────────┘└───────────┘
 *
 * geGL library also contains OpenGL debug message handling,
 * FunctionTable and FunctionProvider classes and FunctionTable decorators.
 */

#pragma once

#include <geGL/AsynchronousQuery.h>
#include <geGL/Buffer.h>
#include <geGL/DebugMessage.h>
#include <geGL/DefaultLoader.h>
#include <geGL/Framebuffer.h>
#include <geGL/FunctionLoaderInterface.h>
#include <geGL/Program.h>
#include <geGL/ProgramPipeline.h>
#include <geGL/Renderbuffer.h>
#include <geGL/Sampler.h>
#include <geGL/Shader.h>
#include <geGL/Texture.h>
#include <geGL/VertexArray.h>
