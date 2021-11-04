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
#include "simulation/SimConfig.h"
#include <pf_common/enums.h>
#include <geGL/Texture.h>
#include "SpeciesPanel.h"


namespace pf::ogl {


class UI {
 public:
  UI(const toml::table &config, GLFWwindow *windowHandle);

  // clang-format off
  ui::ig::Window *windowSim;
  /*  ui::ig::WindowMenuBar *simMenuBar;
      ui::ig::SubMenu *fileSimSubmenu;
        ui::ig::MenuButtonItem *saveSimConfigButton;
        ui::ig::MenuButtonItem *loadSimConfigButton;*/ // TODO: move this to species window
    ui::ig::Button *playPauseButton;
    ui::ig::Checkbox *applyOnChangeCheckbox;
    ui::ig::Group *simControlGroup;
      ui::ig::DragInput<int> *simSpeedDrag;
      ui::ig::Button *restartSimButton;
    ui::ig::Button *applyButton;
  ui::ig::Window *imagesWindow;
    ui::ig::StretchLayout *outImageStretch;
    ui::ig::Image *outImage = nullptr;
  ui::ig::Window *speciesWindow;
    ui::ig::BoxLayout *speciesButtonLayout;
      ui::ig::Button *addSpeciesButton;
      ui::ig::Button *removeSpeciesButton;
    ui::ig::TabBar *speciesTabBar;
      std::vector<SpeciesPanel*> speciesPanels;
  // clang-format on

  void setOutImage(std::shared_ptr<Texture> texture);

  std::unique_ptr<ui::ig::ImGuiInterface> imguiInterface;

  [[nodiscard]] physarum::PopulationConfig getConfig() const;
  void loadFromConfig(const physarum::PopulationConfig &config);

  std::function<void(physarum::PopulationConfig)> onConfigChange = [](auto){};

 private:
  void valueChange(const pf::physarum::PopulationConfig &config);
};

}// namespace pf::ogl

#endif//OPENGL_TEMPLATE_SRC_UI_DEMOIMGUI_H
