//
// Created by xflajs00 on 22.10.2021.
//

#include "files.h"
#include <fstream>
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include <span>
#include <stb/stb_image.h>

std::filesystem::path pf::getExeFolder() {
  const auto currentPath = std::filesystem::current_path();
  return absolute(currentPath);
}

std::optional<std::string> pf::readFile(const std::filesystem::path &path) {
  std::ifstream ifstream{path};
  if (!ifstream.is_open()) { return std::nullopt; }
  return std::string{std::istreambuf_iterator<char>{ifstream}, std::istreambuf_iterator<char>{}};
}

std::optional<pf::ImageLoadData> pf::loadImage(const std::filesystem::path &path, std::optional<int> preferredChannels) {
  auto result = ImageLoadData{};
  int width;
  int height;
  int channels;
  auto stbImgDeleter = [](stbi_uc *ptr) { stbi_image_free(ptr); };
  std::unique_ptr<stbi_uc, decltype(stbImgDeleter)> stbImage(
      stbi_load(absolute(path).string().c_str(), &width, &height, &channels, preferredChannels.value_or(0)), stbImgDeleter);
  if (stbImage == nullptr) { return std::nullopt; }
  result.width = width;
  result.height = height;
  result.channels = channels;
  if (preferredChannels.has_value()) {
    if (*preferredChannels != 4) {
      throw std::runtime_error("unsupported preferred channels");
    }
    auto imgData = std::span<std::byte>{reinterpret_cast<std::byte*>(stbImage.get()), static_cast<std::size_t>(width * height * *preferredChannels)};
    result.data.reserve(imgData.size());
    result.channels = *preferredChannels;
    /*switch (channels) {
      case 1:
        for (int i = 0; i < width * height; ++i) {
          result.data.emplace_back(imgData[i]);
          result.data.emplace_back(imgData[i]);
          result.data.emplace_back(imgData[i]);
          result.data.emplace_back(static_cast<std::byte>(255));
        }
        break;
      case 3:
        for (int i = 0; i < width * height * 3; i += 3) {
          result.data.emplace_back(imgData[i]);
          result.data.emplace_back(imgData[i + 1]);
          result.data.emplace_back(imgData[i + 2]);
          result.data.emplace_back(static_cast<std::byte>(255));
        }
        break;
      case 4: std::ranges::copy(imgData, std::back_inserter(result.data)); break;
      default: throw std::runtime_error("unsupported image format");
    }*/
    std::ranges::copy(imgData, std::back_inserter(result.data));
  } else {
    auto imgData = std::span<std::byte>{reinterpret_cast<std::byte*>(stbImage.get()), static_cast<std::size_t>(width * height * channels)};
    result.data.reserve(imgData.size());
    std::ranges::copy(imgData, std::back_inserter(result.data));
  }
  return result;
}
