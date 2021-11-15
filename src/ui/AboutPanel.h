//
// Created by xflajs00 on 11.11.2021.
//

#ifndef PHYSARUMSIM_ABOUTPANEL_H
#define PHYSARUMSIM_ABOUTPANEL_H

#include "about_data/AboutDataLoader.h"
#include <filesystem>
#include <pf_imgui/elements/Listbox.h>
#include <pf_imgui/layouts/BoxLayout.h>
#include <pf_imgui/layouts/StackedLayout.h>
#include <utility>

namespace pf {

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
