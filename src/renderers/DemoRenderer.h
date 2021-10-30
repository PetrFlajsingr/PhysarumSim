//
// Created by xflajs00 on 22.10.2021.
//

#ifndef OPENGL_TEMPLATE_SRC_TRIANGLERENDERER_H
#define OPENGL_TEMPLATE_SRC_TRIANGLERENDERER_H

#include "Renderer.h"
#include <filesystem>
#include <geGL/Buffer.h>
#include <geGL/Program.h>
#include <geGL/Shader.h>
#include <geGL/VertexArray.h>
#include <glad/glad.h>

namespace pf::ogl {

namespace details {
inline const float vertices[] = {
    0.5f, 0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
    -0.5f, 0.5f, 0.0f};

inline const unsigned int indices[] = {
    0, 1, 3,
    1, 2, 3};
}// namespace details

class DemoRenderer : public Renderer {
 public:
  DemoRenderer(const std::filesystem::path &shaderDir);

  std::optional<std::string> init() override;
  void render() override;

 private:
  std::filesystem::path shaderDir;

  std::shared_ptr<Buffer> vbo;
  std::shared_ptr<Buffer> ebo;
  std::shared_ptr<VertexArray> vao;
  std::shared_ptr<Shader> vertexShader;
  std::shared_ptr<Shader> fragmentShader;
  std::shared_ptr<Program> program;
};

}// namespace pf::ogl

#endif//OPENGL_TEMPLATE_SRC_TRIANGLERENDERER_H
