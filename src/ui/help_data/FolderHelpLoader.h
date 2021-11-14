//
// Created by xflajs00 on 14.11.2021.
//

#ifndef PHYSARUMSIM_FOLDERHELPLOADER_H
#define PHYSARUMSIM_FOLDERHELPLOADER_H

#include "HelpLoader.h"
#include <filesystem>
#include <optional>
#include <string_view>
#include <geGL/Texture.h>
#include <pf_imgui/elements/MarkdownText.h>

namespace pf {

class FolderHelpLoader : public HelpLoader {
 public:
  FolderHelpLoader(const std::filesystem::path &srcFolder);

  [[nodiscard]] ImageLoader getImageLoader() override;

  [[nodiscard]] std::vector<HelpData> getHelpData() override;

 private:
  std::optional<ui::ig::MarkdownText::ImageData> loadImageImpl(std::string_view path);

  std::filesystem::path folder;
  struct TextureCacheData {
    TextureCacheData();
    TextureCacheData(std::shared_ptr<Texture> texture, ui::ig::Size size);
    std::shared_ptr<Texture> texture;
    ui::ig::Size size;
  };
  std::unordered_map<std::string, TextureCacheData> textures;
};

}

#endif//PHYSARUMSIM_FOLDERHELPLOADER_H
