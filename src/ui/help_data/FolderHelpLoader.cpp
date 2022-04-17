//
// Created by xflajs00 on 14.11.2021.
//

#include "FolderHelpLoader.h"
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include <stb/stb_image.h>

namespace pf {
using namespace ui::ig;
FolderHelpLoader::FolderHelpLoader(const std::filesystem::path &srcFolder) : folder(srcFolder) {}

HelpLoader::ImageLoader FolderHelpLoader::getImageLoader() {
  return [this](std::string_view path) { return loadImageImpl(path); };
}

std::vector<HelpData> FolderHelpLoader::getHelpData() {
  const auto mainFile = folder / "help.toml";
  if (!std::filesystem::exists(mainFile)) { return {}; }
  auto data = toml::parse_file(mainFile.string());
  if (!data.contains("pages")) { return {}; }
  const auto pages = data["pages"].as_array();
  if (pages == nullptr) { return {}; }

  std::vector<HelpData> result;

  std::vector<std::string> categories;
  for (auto &page : *pages) {
    categories.clear();
    const auto pageTable = page.as_table();
    if (pageTable == nullptr) { continue; }
    const auto menu = pageTable->get("menu");
    if (menu == nullptr) { continue; }
    const auto menuArr = menu->as_array();
    if (menuArr == nullptr) { continue; }
    for (auto &category : *menuArr) {
      if (const auto value = category.value<std::string>(); value.has_value()) {
        categories.emplace_back(value.value());
      } else {
        break;
      }
    }
    const auto mdFile = pageTable->get("markdown");
    if (mdFile == nullptr) { continue; }
    std::filesystem::path mdPath;
    if (const auto value = mdFile->value<std::string>(); value.has_value()) {
      mdPath = folder / value.value();
    } else {
      continue;
    }

    std::basic_ifstream<char8_t> is{mdPath};
    std::basic_stringstream<char8_t> buffer;
    buffer << is.rdbuf();

    result.emplace_back(categories, buffer.str());
  }
  return result;
}

std::optional<MarkdownText::ImageData> FolderHelpLoader::loadImageImpl(std::string_view path) {
  const auto imgID = std::string(path);
  if (const auto iter = textures.find(imgID); iter != textures.end()) {
    return MarkdownText::ImageData{
        reinterpret_cast<ImTextureID>(static_cast<std::uintptr_t>(iter->second.texture->getId())), iter->second.size};
  }
  int width;
  int height;
  int channels;
  auto stbImgDeleter = [](stbi_uc *ptr) { stbi_image_free(ptr); };
  const auto texPath = folder / path;
  std::unique_ptr<stbi_uc, decltype(stbImgDeleter)> stbImage(
      stbi_load(absolute(texPath).string().c_str(), &width, &height, &channels, 0), stbImgDeleter);
  if (stbImage == nullptr) { return std::nullopt; }
  auto texture = std::make_shared<Texture>(GL_TEXTURE_2D, GL_RGBA, 0, width, height);
  texture->texParameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
  texture->texParameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);
  texture->texParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  texture->texParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  std::vector<unsigned char> data;
  data.reserve(height * width * 4);
  auto s = std::span{stbImage.get(), static_cast<std::size_t>(width * height * channels)};
  switch (channels) {
    case 1:
      for (int i = 0; i < width * height; ++i) {
        data.emplace_back(s[i]);
        data.emplace_back(s[i]);
        data.emplace_back(s[i]);
        data.emplace_back(255);
      }
      break;
    case 3:
      for (int i = 0; i < width * height * 3; i += 3) {
        data.emplace_back(s[i]);
        data.emplace_back(s[i + 1]);
        data.emplace_back(s[i + 2]);
        data.emplace_back(255);
      }
      break;
    case 4: std::ranges::copy(s, std::back_inserter(data)); break;
  }
  texture->setData2D(data.data());
  auto cachedData = TextureCacheData{texture, Size{static_cast<float>(width), static_cast<float>(height)}};
  textures[imgID] = cachedData;
  return MarkdownText::ImageData{(ImTextureID) (cachedData.texture->getId()), cachedData.size};
}

FolderHelpLoader::TextureCacheData::TextureCacheData() : texture(nullptr), size(ui::ig::Size::Auto()) {}

FolderHelpLoader::TextureCacheData::TextureCacheData(std::shared_ptr<Texture> texture, ui::ig::Size size)
    : texture(std::move(texture)), size(size) {}
}// namespace pf