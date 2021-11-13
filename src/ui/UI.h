//
// Created by xflajs00 on 22.10.2021.
//

#ifndef OPENGL_TEMPLATE_SRC_UI_DEMOIMGUI_H
#define OPENGL_TEMPLATE_SRC_UI_DEMOIMGUI_H

#include "MouseInteractionPanel.h"
#include "SimulationControlsPanel.h"
#include "SpeciesPanel.h"
#include "RecorderPanel.h"
#include "simulation/SimConfig.h"
#include <GLFW/glfw3.h>
#include <geGL/Texture.h>
#include <pf_common/enums.h>
#include <pf_imgui/ImGuiInterface.h>
#include <pf_imgui/elements.h>
#include <pf_imgui/elements/Spinner.h>
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
      ui::ig::MenuButtonItem *viewShowAll;
      ui::ig::MenuButtonItem *viewHideAll;
      ui::ig::MenuCheckboxItem *viewSimWin;
      ui::ig::MenuCheckboxItem *viewImagesWin;
      ui::ig::MenuCheckboxItem *viewSpeciesWin;
      ui::ig::MenuCheckboxItem *viewInfoWin;
      ui::ig::MenuCheckboxItem *viewInteractWin;
    ui::ig::SubMenu *helpSubmenu;
      ui::ig::MenuButtonItem *helpButton;
      ui::ig::MenuButtonItem *aboutButton;
  ui::ig::Window *dockWindow;
    ui::ig::DockSpace *mainDockspace;
  ui::ig::Window *interactionWindow;
    MouseInteractionPanel *mouseInteractionPanel;
  ui::ig::Window *infoWindow;
    ui::ig::SimplePlot *fpsCurrentPlot;
    ui::ig::SimplePlot *fpsAveragePlot;
    ui::ig::Text *fpsLabel;
  ui::ig::Window *simWindow;
    SimulationControlsPanel *simControlsPanel;
  ui::ig::Window *imagesWindow;
    ui::ig::WindowMenuBar *imagesMenuBar;
      ui::ig::SubMenu *fileImagesSubmenu;
        ui::ig::MenuButtonItem *saveImageButton;
        ui::ig::MenuButtonItem *startRecordingButton;
    ui::ig::StretchLayout *outImageStretch;
    ui::ig::Image *outImage = nullptr;
  ui::ig::Window *speciesWindow;
    ui::ig::WindowMenuBar *speciesMenuBar;
      ui::ig::SubMenu *fileSpeciesSubmenu;
        ui::ig::MenuButtonItem *saveSpeciesButton;
        ui::ig::MenuButtonItem *loadSpeciesButton;
    ui::ig::Combobox<BlendType> *blendTypeCombobox;
    ui::ig::ColorEdit<glm::vec3> *backgroundColorEdit;
    ui::ig::BoxLayout *speciesButtonLayout;
    ui::ig::TabBar *speciesTabBar;
      ui::ig::TabButton *addSpeciesButton;
      std::vector<SpeciesPanel*> speciesPanels;
  ui::ig::Window *recordingWindow;
    RecorderPanel *recorderPanel;
  // clang-format on

  void setOutImage(const std::shared_ptr<Texture> &texture);

  Subscription addResetListener(std::invocable auto &&listener) {
    return resetObservable.addListener(std::forward<decltype(listener)>(listener));
  }

  void cleanupConfig(toml::table &config);

  std::function<void(std::filesystem::path)> onScreenshotSave = [](auto) {};

  std::unique_ptr<ui::ig::ImGuiInterface> imguiInterface;

 private:
  ui::ig::Observable_impl<SpeciesPanel *> resetObservable;

  void setAllWinVisibility(bool visible);

  [[nodiscard]] toml::array speciesToToml() const;

  void loadFromToml(const toml::table &src);

  void updateSpeciesTabBarFromConfig(const toml::table &config);

  void addDefaultSpecies();

  void addSpeciesTabCloseConfirmation(ui::ig::Tab &tab, const std::string &speciesName);

  void createSpeciesTab(const std::string &name);
  void createSpeciesTab(const std::string &name, const toml::table &src);

  void setMouseInteractionSpecies();

  inline auto getSpeciesNames() { return speciesTabBar->getTabs() | std::views::transform(&ui::ig::Tab::getLabel); }

  void reloadSpeciesInteractions();
};

}// namespace pf::ogl

#endif//OPENGL_TEMPLATE_SRC_UI_DEMOIMGUI_H
