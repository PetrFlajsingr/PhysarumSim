//
// Created by xflajs00 on 23.11.2021.
//

#ifndef PHYSARUMSIM_IMGTRANSFORM_H
#define PHYSARUMSIM_IMGTRANSFORM_H

#include <geGL/Texture.h>
#include <geGL/Program.h>
#include <geGL/Shader.h>
#include <span>
#include <filesystem>

namespace pf {

class ImgTransform {
 public:
  ImgTransform(const std::filesystem::path &shaderDir);

  void setInputData(std::span<const std::byte> imgData, std::size_t width, std::size_t height);

  [[nodiscard]] const std::shared_ptr<Texture> &getInputTexture() const;
  [[nodiscard]] const std::shared_ptr<Texture> &getOutputTexture() const;

  void transform();

 private:
  std::shared_ptr<Texture> inputTexture;
  std::shared_ptr<Texture> greyscaleTexture;
  std::shared_ptr<Texture> outputTexture;
  std::shared_ptr<Program> grayscaleProgram;
  std::shared_ptr<Program> sobelProgram;
};

}
#endif//PHYSARUMSIM_IMGTRANSFORM_H
