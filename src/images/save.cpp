//
// Created by xflajs00 on 08.11.2021.
//

#include "save.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

void pf::saveImage(const std::filesystem::path &path, pf::ImageFormat format, PixelFormat pixelFormat, std::size_t width, std::size_t height, std::span<const std::byte> data) {

  const auto componentCount = getComponentCount(pixelFormat);

  switch (format) {
      case ImageFormat::PNG:
        stbi_write_png(path.string().c_str(), static_cast<int>(width), static_cast<int>(height),
                       componentCount, data.data(), static_cast<int>(width * componentCount));
        break;
      case ImageFormat::JPEG:
        stbi_write_jpg(path.string().c_str(), static_cast<int>(width), static_cast<int>(height),
                       componentCount, data.data(), 100);
        break;
      case ImageFormat::BMP:
        stbi_write_bmp(path.string().c_str(), static_cast<int>(width), static_cast<int>(height),
                       componentCount, data.data());
        break;
  }
}
