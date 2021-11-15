//
// Created by xflajs00 on 11.11.2021.
//

#ifndef PHYSARUMSIM_ABOUTPANEL_H
#define PHYSARUMSIM_ABOUTPANEL_H

#include <filesystem>
#include <utility>
#include <pf_imgui/elements/Listbox.h>
#include <pf_imgui/layouts/BoxLayout.h>
#include <pf_imgui/layouts/StackedLayout.h>

namespace pf {
struct AboutData {
  std::string label;
  std::u8string markdown;
};

class AboutDataLoader {
 public:
  [[nodiscard]] virtual std::vector<AboutData> getAboutData() = 0;
  virtual ~AboutDataLoader() = default;
};

class FolderAboutDataLoader : public AboutDataLoader {
 public:
  explicit FolderAboutDataLoader(std::filesystem::path aboutPath) : folder(std::move(aboutPath)) {}

  [[nodiscard]] std::vector<AboutData> getAboutData() override {
    auto dirIter = std::filesystem::directory_iterator(folder);
    return dirIter | ranges::views::transform([](const auto &entry) {
             const auto label = entry.path().filename().string();
             std::basic_ifstream<char8_t> is{entry.path()};
             std::basic_stringstream<char8_t> ss;
             ss << is.rdbuf();
             return AboutData{label, ss.str()};
           })
        | ranges::to_vector;
  }

 private:
  std::filesystem::path folder;
};

class AboutPanel : public ui::ig::Element {
 public:
  AboutPanel(const std::string &name, const ui::ig::Size &size, std::unique_ptr<AboutDataLoader> dataLoader,
             ui::ig::ImGuiInterface &imguiInterface);

 protected:
  void renderImpl() override;

 private:
  // clang-format off
  ui::ig::BoxLayout layout;
    ui::ig::BoxLayout &listLayout;
      ui::ig::Listbox<std::string> &libList;
    ui::ig::StackedLayout &textStack;
  // clang-format on
    std::unique_ptr<AboutDataLoader> loader;
};

}// namespace pf
#endif//PHYSARUMSIM_ABOUTPANEL_H
