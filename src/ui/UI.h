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

ENABLE_PF_ENUM_OUT_FOR_NAMESPACE(pf::physarum)

namespace pf::ogl {


class UI {
 public:
  UI(const toml::table &config, GLFWwindow *windowHandle);

  // clang-format off
  ui::ig::Window *windowSim;
    ui::ig::WindowMenuBar *simMenuBar;
      ui::ig::SubMenu *fileSimSubmenu;
        ui::ig::MenuButtonItem *saveSimConfigButton;
        ui::ig::MenuButtonItem *loadSimConfigButton;
    ui::ig::Button *playPauseButton;
    ui::ig::Checkbox *applyOnChangeCheckbox;
    ui::ig::Group *simControlGroup;
      ui::ig::DragInput<int> *simSpeedDrag;
      ui::ig::DragInput<float> *senseAngleDrag;
      ui::ig::DragInput<float> *senseDistanceDrag;
      ui::ig::Combobox<int> *sensorSizeCombobox;
      ui::ig::DragInput<float> *turnSpeedDrag;
      ui::ig::DragInput<float> *movementSpeedDrag;
      ui::ig::DragInput<float> *trailWeightDrag;
      ui::ig::Separator *sep1;
      ui::ig::Input<int> *particleCountInput;
      ui::ig::Combobox<physarum::ParticleStart> *particleInitCombobox;
      ui::ig::Button *restartSimButton;
      // reset btn
    ui::ig::Group *trailControlGroup;
      ui::ig::Combobox<int> *kernelSizeCombobox;
      ui::ig::DragInput<float> *diffuseRateDrag;
      ui::ig::DragInput<float> *decayRateDrag;
      ui::ig::DragInput<float> *maxTrailValueDrag;
    ui::ig::ColorEdit<glm::vec4> *trailColorEdit; // add
    ui::ig::Button *applyButton;
  ui::ig::Window *imagesWindow;
    ui::ig::StretchLayout *outImageStretch;
    ui::ig::Image *outImage = nullptr;
  // clang-format on

  void setOutImage(std::shared_ptr<Texture> texture);

  std::unique_ptr<ui::ig::ImGuiInterface> imguiInterface;

  [[nodiscard]] physarum::SimConfig getConfig() const;
  void loadFromConfig(const physarum::SimConfig &config);

  std::function<void(physarum::SimConfig)> onConfigChange = [](auto){};

 private:
  void valueChange();
};

}// namespace pf::ogl

#endif//OPENGL_TEMPLATE_SRC_UI_DEMOIMGUI_H
