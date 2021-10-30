//
// Created by xflajs00 on 22.10.2021.
//

#include "DemoRenderer.h"
#include <utils/files.h>

pf::ogl::DemoRenderer::DemoRenderer(const std::filesystem::path &shaderDir) : shaderDir(shaderDir) {
}

std::optional<std::string> pf::ogl::DemoRenderer::init() {
  const auto vertexShaderSrc = readFile(shaderDir / "simple_shader.vert");
  if (!vertexShaderSrc.has_value()) {
    return "Could not load 'simple_shader.vert'";
  }
  const auto fragmentShaderSrc = readFile(shaderDir / "simple_shader.frag");
  if (!fragmentShaderSrc.has_value()) {
    return "Could not load 'simple_shader.frag'";
  }
  vertexShader = std::make_shared<Shader>(GL_VERTEX_SHADER, vertexShaderSrc.value());
  fragmentShader = std::make_shared<Shader>(GL_FRAGMENT_SHADER, fragmentShaderSrc.value());

  program = std::make_shared<Program>(vertexShader, fragmentShader);

  vbo = std::make_shared<Buffer>(sizeof(details::vertices), details::vertices, GL_STATIC_DRAW);
  ebo = std::make_shared<Buffer>(sizeof(details::indices), details::indices, GL_STATIC_DRAW);

  vao = std::make_shared<VertexArray>();
  vao->addAttrib(vbo, 0, 3, GL_FLOAT, 3 * sizeof(float));
  vao->addElementBuffer(ebo);
  return std::nullopt;
}

void pf::ogl::DemoRenderer::render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  program->use();
  vao->bind();
  glDrawElements(GL_TRIANGLES, ebo->getSize() / sizeof(float), GL_UNSIGNED_INT, nullptr);
}