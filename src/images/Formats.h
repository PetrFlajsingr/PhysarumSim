//
// Created by xflajs00 on 08.11.2021.
//

#ifndef PHYSARUMSIM_SRC_IMAGES_FORMATS_H
#define PHYSARUMSIM_SRC_IMAGES_FORMATS_H

#include <filesystem>
#include <optional>

namespace pf {

enum class PixelFormat { RGB, RGBA, BGRA, BGR };

inline int getComponentCount(PixelFormat pixelFormat) {
  switch (pixelFormat) {
    case PixelFormat::RGB:
    case PixelFormat::BGR: return 3;
    case PixelFormat::RGBA:
    case PixelFormat::BGRA: return 4;
  }
  return 0;
}

enum class ImageFormat { PNG, JPEG, BMP };

inline std::optional<ImageFormat> getImageFormat(const std::filesystem::path &path) {
  auto ext = path.extension().string();
  if (ext == ".png") { return ImageFormat::PNG; }
  if (ext == ".jpg" || ext == "jpeg") { return ImageFormat::JPEG; }
  if (ext == ".bmp") { return ImageFormat::BMP; }
  return std::nullopt;
}

}// namespace pf

#endif//PHYSARUMSIM_SRC_IMAGES_FORMATS_H
