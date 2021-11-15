//
// Created by xflajs00 on 14.11.2021.
//

#ifndef PHYSARUMSIM_HELPLOADER_H
#define PHYSARUMSIM_HELPLOADER_H

#include "HelpData.h"
#include <pf_imgui/elements/MarkdownText.h>

namespace pf {

class HelpLoader {
 public:
  using ImageLoader = ui::ig::MarkdownText::ImageLoader;
  [[nodiscard]] virtual ImageLoader getImageLoader() = 0;
  [[nodiscard]] virtual std::vector<HelpData> getHelpData() = 0;
  virtual ~HelpLoader() = default;
};

// TODO: remove
class EmptyHelpLoader : public HelpLoader {
 public:
  ImageLoader getImageLoader() override { return [](auto) { return std::optional<ui::ig::MarkdownText::ImageData>{}; }; }
  std::vector<HelpData> getHelpData() override { return {}; }
};
}

#endif//PHYSARUMSIM_HELPLOADER_H
