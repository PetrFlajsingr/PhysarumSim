//
// Created by xflajs00 on 22.10.2021.
//

#ifndef OPENGL_TEMPLATE_SRC_TRIANGLERENDERER_H
#define OPENGL_TEMPLATE_SRC_TRIANGLERENDERER_H

#include <array>
#include <filesystem>
#include <geGL/Buffer.h>
#include <geGL/Program.h>
#include <geGL/Shader.h>
#include <geGL/VertexArray.h>
#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <simulation/PhysarumSimulator.h>
#include <simulation/SimConfig.h>

namespace pf::ogl {

enum class BlendType {
  Additive = 0, AlphaMix = 1, Layered = 2
};

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

class PhysarumRenderer {
 public:
  PhysarumRenderer(const std::filesystem::path &shaderDir, std::shared_ptr<Texture> trailTexture, glm::ivec2 renderResolution);

  void init(const std::vector<physarum::PopulationColor> &populations);

  void render();

  void setTrailTexture(const std::shared_ptr<Texture> &trailTexture);

  void setConfig(const physarum::PopulationColor &config, std::size_t index);
  void setColorLUT(const std::array<glm::vec3, 256> &lut, std::size_t index);

  void setBackgroundColor(const glm::vec3 &backgroundColor);
  void setBlendType(BlendType blendType);

  [[nodiscard]] const std::shared_ptr<Texture> &getRenderTexture() const;

 private:
  std::filesystem::path shaderDir;

  std::shared_ptr<Program> renderTextureProgram;
  std::shared_ptr<Program> renderQuadProgram;

  std::shared_ptr<Buffer> quadVBO;
  std::shared_ptr<VertexArray> quadVAO;

  std::shared_ptr<Texture> renderTexture;
  std::shared_ptr<Texture> trailTexture;

  std::shared_ptr<Buffer> colorLUTBuffer;
  std::shared_ptr<Buffer> speciesSettingsBuffer;

  glm::ivec2 renderResolution;
  glm::vec3 backgroundColor;

  int speciesCount;
  BlendType blendType = BlendType::AlphaMix;
};

namespace details {
struct SpeciesShaderRenderSettings {
  float trailPow;
  SpeciesShaderRenderSettings() = default;
  SpeciesShaderRenderSettings(const physarum::PopulationColor &src);
};

}

}// namespace pf::ogl

#endif//OPENGL_TEMPLATE_SRC_TRIANGLERENDERER_H
