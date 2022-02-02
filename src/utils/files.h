//
// Created by xflajs00 on 22.10.2021.
//

#ifndef OPENGL_TEMPLATE_SRC_UTILS_FILES_H
#define OPENGL_TEMPLATE_SRC_UTILS_FILES_H

#include <filesystem>
#include <optional>
#include <string>

namespace pf {

/**
 * @return absolute path to folder of current executable
 */
[[nodiscard]] std::filesystem::path getExeFolder();

[[nodiscard]] std::optional<std::string> readFile(const std::filesystem::path &path);

struct ImageLoadData {
  std::size_t width;
  std::size_t height;
  std::size_t channels;
  std::vector<std::byte> data;
};

[[nodiscard]] std::optional<ImageLoadData> loadImage(const std::filesystem::path &path, std::optional<int> preferredChannels = std::nullopt);

}// namespace pf

#endif//OPENGL_TEMPLATE_SRC_UTILS_FILES_H
