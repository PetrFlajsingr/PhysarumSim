//
// Created by xflajs00 on 22.10.2021.
//

#ifndef OPENGL_TEMPLATE_SRC_UI_DEMOIMGUI_H
#define OPENGL_TEMPLATE_SRC_UI_DEMOIMGUI_H

#include <GLFW/glfw3.h>
#include <pf_imgui/ImGuiInterface.h>
#include <pf_imgui/elements.h>
#include <pf_imgui/layouts/layouts.h>
#include <toml++/toml.h>

namespace pf::ogl {

/**
 * @brief Simple demo UI.
 */
class DemoImGui {
 public:
  DemoImGui(const toml::table &config, GLFWwindow *windowHandle);

  // clang-format off
  ui::ig::Window *window1;
    ui::ig::BoxLayout *layout1;
      ui::ig::Listbox<std::string> *listBox1;
      ui::ig::Text *listboxLabel;
      ui::ig::Button *button1;
      ui::ig::Button *button2;
      ui::ig::Checkbox *checkbox1;
      ui::ig::Text *checkboxLabel;
      ui::ig::RadioGroup *radioGroup;
      ui::ig::Text *radioGroupLabel;

  // clang-format on

  std::unique_ptr<ui::ig::ImGuiInterface> imguiInterface;
};

}// namespace pf::ogl

#endif//OPENGL_TEMPLATE_SRC_UI_DEMOIMGUI_H
