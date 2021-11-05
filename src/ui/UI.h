//
// Created by xflajs00 on 22.10.2021.
//

#ifndef OPENGL_TEMPLATE_SRC_UI_DEMOIMGUI_H
#define OPENGL_TEMPLATE_SRC_UI_DEMOIMGUI_H

#include "SpeciesPanel.h"
#include "simulation/SimConfig.h"
#include <GLFW/glfw3.h>
#include <geGL/Texture.h>
#include <pf_common/enums.h>
#include <pf_imgui/ImGuiInterface.h>
#include <pf_imgui/elements.h>
#include <pf_imgui/layouts/layouts.h>
#include <renderers/PhysarumRenderer.h>
#include <toml++/toml.h>

ENABLE_PF_ENUM_OUT_FOR_NAMESPACE(pf::ui::ig)
ENABLE_PF_ENUM_OUT_FOR_NAMESPACE(pf::ogl)

namespace pf::ogl {


class UI {
 public:
  UI(const toml::table &config, GLFWwindow *windowHandle);

  // clang-format off
  ui::ig::AppMenuBar *appMenuBar;
    ui::ig::SubMenu *viewSubmenu;

  ui::ig::Window *windowSim;
  /*  ui::ig::WindowMenuBar *simMenuBar;
      ui::ig::SubMenu *fileSimSubmenu;
        ui::ig::MenuButtonItem *saveSimConfigButton;
        ui::ig::MenuButtonItem *loadSimConfigButton;*/ // TODO: move this to species window
    ui::ig::Button *playPauseButton;
    ui::ig::Group *simControlGroup;
      ui::ig::DragInput<int> *simSpeedDrag;
      ui::ig::Button *restartSimButton;
  ui::ig::Window *imagesWindow;
    ui::ig::StretchLayout *outImageStretch;
    ui::ig::Image *outImage = nullptr;
  ui::ig::Window *speciesWindow;
    ui::ig::Combobox<BlendType> *blendTypeCombobox;
    ui::ig::ColorEdit<glm::vec3> *backgroundColorEdit;
    ui::ig::BoxLayout *speciesButtonLayout;
    ui::ig::TabBar *speciesTabBar;
      ui::ig::TabButton *addSpeciesButton;
      std::vector<SpeciesPanel*> speciesPanels;
  // clang-format on

  void setOutImage(std::shared_ptr<Texture> texture);

  Subscription addResetListener(std::invocable auto &&listener) {
      return resetObservable.addListener(std::forward<decltype(listener)>(listener));
  }

  std::unique_ptr<ui::ig::ImGuiInterface> imguiInterface;
 private:
  ui::ig::Observable_impl<SpeciesPanel*> resetObservable;

};

}// namespace pf::ogl

#endif//OPENGL_TEMPLATE_SRC_UI_DEMOIMGUI_H
