//
// Created by xflajs00 on 23.11.2021.
//

#include "ImgTransform.h"
#include <utils/files.h>

namespace pf {

ImgTransform::ImgTransform(const std::filesystem::path &shaderDir) {
    const auto grayscaleSrc = readFile(shaderDir / "greyscale.comp");
    if (!grayscaleSrc.has_value()) { throw std::runtime_error("Could not load 'greyscale.comp'"); }
    auto grayscaleShader = std::make_shared<Shader>(GL_COMPUTE_SHADER, grayscaleSrc.value());
    grayscaleProgram = std::make_shared<Program>(grayscaleShader);

    const auto sobelSrc = readFile(shaderDir / "sobel.comp");
    if (!sobelSrc.has_value()) { throw std::runtime_error("Could not load 'greyscale.comp'"); }
    auto sobelShader = std::make_shared<Shader>(GL_COMPUTE_SHADER, sobelSrc.value());
    sobelProgram = std::make_shared<Program>(sobelShader);
}

void ImgTransform::setInputData(std::span<const std::byte> imgData, std::size_t width, std::size_t height) {
  assert(imgData.size() == width * height * 4);
  inputTexture = std::make_shared<Texture>(GL_TEXTURE_2D, GL_RGBA32F, 0, width, height);
  inputTexture->texParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  inputTexture->texParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  inputTexture->setData2D(imgData.data());

  greyscaleTexture = std::make_shared<Texture>(GL_TEXTURE_2D, GL_RGBA32F, 0, width, height);
  greyscaleTexture->texParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  greyscaleTexture->texParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  outputTexture = std::make_shared<Texture>(GL_TEXTURE_2D, GL_RGBA32F, 0, width, height);
  outputTexture->texParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  outputTexture->texParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

const std::shared_ptr<Texture> &ImgTransform::getInputTexture() const { return inputTexture; }

const std::shared_ptr<Texture> &ImgTransform::getOutputTexture() const { return outputTexture; }

void ImgTransform::transform() {
  grayscaleProgram->use();
  inputTexture->bindImage(0);
  greyscaleTexture->bindImage(1);
  grayscaleProgram->dispatch(inputTexture->getWidth(0) / 8, inputTexture->getHeight(0) / 8);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  sobelProgram->use();
  greyscaleTexture->bindImage(0);
  outputTexture->bindImage(1);
  sobelProgram->dispatch(outputTexture->getWidth(0) / 8, outputTexture->getHeight(0) / 8);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}


}// namespace pf