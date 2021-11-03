//
// Created by xflajs00 on 22.10.2021.
//

#include "PhysarumRenderer.h"
#include <utils/files.h>

#include <utility>

pf::ogl::PhysarumRenderer::PhysarumRenderer(const std::filesystem::path &shaderDir,
                                            std::shared_ptr<Texture> trailTexture,
                                            glm::ivec2 renderResolution)
    : shaderDir(shaderDir),
      trailTexture(std::move(trailTexture)),
      renderResolution(renderResolution){
  const auto renderTextureSrc = readFile(shaderDir / "physarum_render.comp");
  if (!renderTextureSrc.has_value()) {
    throw std::runtime_error("Could not load 'physarum_render.comp'");
  }
  auto renderTextureShader = std::make_shared<Shader>(GL_COMPUTE_SHADER, renderTextureSrc.value());
  renderTextureProgram = std::make_shared<Program>(renderTextureShader);

  const auto renderVertSrc = readFile(shaderDir / "render.vert");
  if (!renderVertSrc.has_value()) {
    throw std::runtime_error("Could not load 'render.vert'");
  }
  auto renderVertShader = std::make_shared<Shader>(GL_VERTEX_SHADER, renderVertSrc.value());
  const auto renderFragSrc = readFile(shaderDir / "render.frag");
  if (!renderFragSrc.has_value()) {
    throw std::runtime_error("Could not load 'render.frag'");
  }
  auto renderFragShader = std::make_shared<Shader>(GL_FRAGMENT_SHADER, renderFragSrc.value());
  renderQuadProgram = std::make_shared<Program>(renderVertShader, renderFragShader);

  renderTexture = std::make_shared<Texture>(GL_TEXTURE_2D, GL_RGBA32F, 0, renderResolution.x, renderResolution.y);
  renderTexture->texParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  renderTexture->texParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  const std::vector<float> quadVertices{
      -1.0f,
      1.0f,
      0.0f,
      0.0f,
      1.0f,
      -1.0f,
      -1.0f,
      0.0f,
      0.0f,
      0.0f,
      1.0f,
      1.0f,
      0.0f,
      1.0f,
      1.0f,
      1.0f,
      -1.0f,
      0.0f,
      1.0f,
      0.0f,
  };
  quadVBO = std::make_shared<Buffer>(sizeof(float) * quadVertices.size(), quadVertices.data());
  quadVAO = std::make_shared<VertexArray>();
  quadVAO->addAttrib(quadVBO, 0, 3, GL_FLOAT, 5 * sizeof(float), 0);
  quadVAO->addAttrib(quadVBO, 1, 2, GL_FLOAT, 5 * sizeof(float), (3 * sizeof(float)));
}

void pf::ogl::PhysarumRenderer::render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  renderTextureProgram->use();
  renderTextureProgram->set3f("color", color.r, color.g, color.b);
  trailTexture->bindImage(0);
  renderTexture->bindImage(1);
  renderTextureProgram->dispatch(renderResolution.x / 8, renderResolution.y / 8);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  renderQuadProgram->use();
  quadVAO->bind();
  renderTexture->bind(0);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
void pf::ogl::PhysarumRenderer::setTrailTexture(const std::shared_ptr<Texture> &trailTexture) {
  PhysarumRenderer::trailTexture = trailTexture;
}
void pf::ogl::PhysarumRenderer::setColor(const glm::vec3 &color) {
  PhysarumRenderer::color = color;
}

const std::shared_ptr<Texture> &pf::ogl::PhysarumRenderer::getRenderTexture() const {
  return renderTexture;
}