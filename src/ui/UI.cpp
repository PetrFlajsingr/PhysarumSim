//
// Created by xflajs00 on 22.10.2021.
//

#include "UI.h"
#include <pf_imgui/backends/ImGuiGlfwOpenGLInterface.h>
#include <pf_imgui/enums.h>
#include <pf_imgui/styles/dark.h>

using namespace pf::enum_operators;

pf::ogl::UI::UI(const toml::table &config, GLFWwindow *windowHandle, std::unique_ptr<HelpLoader> helpLoader,
                std::unique_ptr<AboutDataLoader> aboutLoader) {
  using namespace ui::ig;
  imguiInterface = std::make_unique<ImGuiGlfwOpenGLInterface>(
      ImGuiGlfwOpenGLConfig{.windowHandle = windowHandle,
                            .flags = {},
                            .enableMultiViewport = false,
                            .config = config,
                            .pathToIconFolder = *config["path_icons"].value<std::string>(),
                            .enabledIconPacks = IconPack::ForkAwesome,
                            .defaultFontSize = 13.f});
  setDarkStyle(*imguiInterface);

  dockWindow = &imguiInterface->createWindow("dock_window", "Test");
  dockWindow->setUserResizable(false);
  dockWindow->setUserMovable(false);
  dockWindow->setTitleBarVisible(false);
  dockWindow->setStayInBackground(true);
  mainDockspace = &dockWindow->createChild<StretchLayout>("dock_stretch_layout", Size::Fill(), Stretch::All)
                       .createChild<DockSpace>("main_dockspace", Size::Auto());

  appMenuBar = &imguiInterface->getMenuBar();
  viewSubmenu = &appMenuBar->addSubmenu("app_view_submenu", "View");
  viewShowAll = &viewSubmenu->addButtonItem("show_all_view", "Show all");
  viewHideAll = &viewSubmenu->addButtonItem("hide_all_view", "Hide all");
  viewShowAll->addClickListener([&] { setAllWinVisibility(true); });
  viewHideAll->addClickListener([&] { setAllWinVisibility(false); });
  viewSubmenu->addSeparator("view_submenu_separator");
  viewSimWin = &viewSubmenu->addCheckboxItem("show_sim_menu", "Simulation", true, Persistent::Yes);
  viewImagesWin = &viewSubmenu->addCheckboxItem("show_images_menu", "Images", true, Persistent::Yes);
  viewSpeciesWin = &viewSubmenu->addCheckboxItem("show_species_menu", "Species", true, Persistent::Yes);
  viewInfoWin = &viewSubmenu->addCheckboxItem("show_info_menu", "Info", true, Persistent::Yes);
  viewInteractWin = &viewSubmenu->addCheckboxItem("show_interact_menu", "Interaction", true, Persistent::Yes);

  helpSubmenu = &appMenuBar->addSubmenu("help_submenu", ICON_FK_QUESTION " Help");
  helpButton = &helpSubmenu->addButtonItem("help_button", ICON_FK_QUESTION_CIRCLE " Help");
  aboutButton = &helpSubmenu->addButtonItem("about_button", ICON_FK_INFO " About");

  statusBar = &imguiInterface->createStatusBar("status_bar");
  particleCountText = &statusBar->createChild<Text>("part_count_text", "Particle count: ");

  interactionWindow = &imguiInterface->createWindow("interaction_window", ICON_FK_MOUSE_POINTER " Interaction");
  interactionWindow->addCloseListener([&]() { viewInteractWin->setValue(false); });
  viewInteractWin->addValueListener(
      [&](bool value) { interactionWindow->setVisibility(value ? Visibility::Visible : Visibility::Invisible); }, true);
  interactionWindow->setCloseable(true);
  interactionWindow->setIsDockable(true);
  interactionWindow->getMenuBar().addButtonItem("inter_help_btn", "Help").addClickListener([this] {
    openHelp({"Controls", "UI", "Interaction"});
  });
  mouseInteractionPanel = &interactionWindow->createChild<MouseInteractionPanel>("interaction_panel", Persistent::Yes);

  infoWindow = &imguiInterface->createWindow("info_window", ICON_FK_INFO " Info");
  fpsCurrentPlot =
      &infoWindow->createChild<SimplePlot>("fps_plot", "Fps current", PlotType::Lines, std::vector<float>{},
                                           std::nullopt, 200, 0, FLT_MAX, Size{Width::Auto(), 30});
  fpsAveragePlot =
      &infoWindow->createChild<SimplePlot>("fps_avg_plot", "Fps average", PlotType::Lines, std::vector<float>{},
                                           std::nullopt, 200, 0, FLT_MAX, Size{Width::Auto(), 30});
  fpsLabel = &infoWindow->createChild<Text>("fps_label", "Average FPS: {}");
  infoWindow->addCloseListener([&]() { viewInfoWin->setValue(false); });
  viewInfoWin->addValueListener(
      [&](bool value) { infoWindow->setVisibility(value ? Visibility::Visible : Visibility::Invisible); }, true);
  infoWindow->setCloseable(true);
  infoWindow->setIsDockable(true);
  infoWindow->getMenuBar().addButtonItem("info_help_btn", "Help").addClickListener([this] {
    openHelp({"Controls", "UI", "Info"});
  });

  simWindow = &imguiInterface->createWindow("sim_window", ICON_FK_WRENCH " Simulation");
  viewSimWin->addValueListener(
      [&](bool value) { simWindow->setVisibility(value ? Visibility::Visible : Visibility::Invisible); }, true);
  simWindow->addCloseListener([&]() { viewSimWin->setValue(false); });
  simWindow->setCloseable(true);
  simWindow->setIsDockable(true);
  simWindow->getMenuBar().addButtonItem("sim_help_btn", "Help").addClickListener([this] {
    openHelp({"Controls", "UI", "Simulation"});
  });

  simControlsPanel = &simWindow->createChild<SimulationControlsPanel>("sim_controls_panel");

  imagesWindow = &imguiInterface->createWindow("image_window", ICON_FK_PICTURE_O " Images");
  viewImagesWin->addValueListener(
      [&](bool value) { imagesWindow->setVisibility(value ? Visibility::Visible : Visibility::Invisible); }, true);
  imagesWindow->addCloseListener([&]() { viewImagesWin->setValue(false); });
  imagesWindow->setCloseable(true);
  imagesWindow->setIsDockable(true);

  imagesMenuBar = &imagesWindow->getMenuBar();
  fileImagesSubmenu = &imagesMenuBar->addSubmenu("images_file_submenu", "File");
  saveImageButton = &fileImagesSubmenu->addButtonItem("save_image_btn", ICON_FK_FLOPPY_O " Save screenshot");
  startRecordingButton = &fileImagesSubmenu->addButtonItem("start_rec_btn", ICON_FK_VIDEO_CAMERA " Recording");
  imagesMenuBar->addButtonItem("img_help_btn", "Help").addClickListener([this] {
    openHelp({"Controls", "UI", "Image"});
  });

  outImageStretch = &imagesWindow->createChild<StretchLayout>("out_img_stretch", Size::Auto(), Stretch::All);

  speciesWindow = &imguiInterface->createWindow("species_window", ICON_FK_TASKS " Species");
  viewSpeciesWin->addValueListener(
      [&](bool value) { speciesWindow->setVisibility(value ? Visibility::Visible : Visibility::Invisible); }, true);
  speciesWindow->addCloseListener([&]() { viewSpeciesWin->setValue(false); });
  speciesWindow->setCloseable(true);
  speciesWindow->setIsDockable(true);
  speciesWindow->setCollapsible(true);

  speciesMenuBar = &speciesWindow->getMenuBar();
  fileSpeciesSubmenu = &speciesMenuBar->addSubmenu("species_file_submenu", "File");
  saveSpeciesButton = &fileSpeciesSubmenu->addButtonItem("species_save_button", ICON_FK_FLOPPY_O " Save");
  loadSpeciesButton = &fileSpeciesSubmenu->addButtonItem("species_load_button", ICON_FK_FILE_O " Load");
  speciesMenuBar->addButtonItem("species_help_submenu", "Help").addClickListener([this] {
    openHelp({"Controls", "UI", "Species"});
  });

  blendTypeCombobox = &speciesWindow->createChild<Combobox<BlendType>>("blend_type_combobox", "Blend type", "Select",
                                                                       magic_enum::enum_values<BlendType>(),
                                                                       ComboBoxCount::Items8, Persistent::Yes);
  blendTypeCombobox->setSelectedItem(BlendType::AlphaMix);
  backgroundColorEdit = &speciesWindow->createChild<ColorEdit<glm::vec3>>("background_color_edit", "Background",
                                                                          glm::vec3{.0f}, Persistent::Yes);
  speciesButtonLayout = &speciesWindow->createChild<BoxLayout>("species_buttons_layout", LayoutDirection::LeftToRight,
                                                               Size{Width::Fill(), 30}, AllowCollapse::No,
                                                               ShowBorder::No, Persistent::Yes);

  speciesTabBar = &speciesWindow->createChild<TabBar>("species_tabbar", true);
  addSpeciesButton = &speciesTabBar->addTabButton("add_species_button", ICON_FK_PLUS_SQUARE, TabMod::ForceRight);
  addSpeciesButton->addClickListener([&] {
    imguiInterface->openInputDialog(
        "Species name", "Input species name",
        [&](const auto input) {
          auto names = getSpeciesNames();
          if (std::ranges::find(names, input) != names.end()) {
            imguiInterface->createMsgDlg("Duplicate name", fmt::format("The name '{}' is already present.", input),
                                         Flags{MessageButtons::Ok}, [](auto) { return true; });
            return;
          }
          createSpeciesTab(input);
        },
        [] {});
  });

  simControlsPanel->addRestartClickListener([&] {
    auto closedTabs = speciesTabBar->getTabs() | std::views::filter([](const auto &tab) { return !tab.isOpen(); });
    for (auto &tab : closedTabs) {
      speciesPanels.erase(std::ranges::find_if(
          speciesPanels, [&](const auto &panel) { return panel->getName() == tab.getLabel() + "_species_panel"; }));
      tab.removeChild(tab.getLabel() + "_species_panel");
    }

    auto closedTabNames = closedTabs | std::views::transform(&Tab::getName) | ranges::to_vector;
    std::ranges::for_each(closedTabNames, [&](const auto &tabName) { speciesTabBar->removeTab(tabName); });
    setMouseInteractionSpecies();
    reloadSpeciesInteractions();
  });

  saveSpeciesButton->addClickListener([&] {
    imguiInterface->openFileDialog(
        "Select save location", {FileExtensionSettings{{"toml"}, "toml", ImVec4{1, 0, 0, 1}}},
        [&](const auto &selected) {
          const auto &dst = selected[0];
          toml::table data;
          data.insert("species", speciesToToml());
          std::ofstream ostream{dst};
          ostream << data;
        },
        [] {}, Size{500, 400});
  });

  loadSpeciesButton->addClickListener([&] {
    imguiInterface->openFileDialog(
        "Select file to load", {FileExtensionSettings{{"toml"}, "toml", ImVec4{1, 0, 0, 1}}},
        [&](const auto &selected) {
          const auto &dst = selected[0];
          const auto config = toml::parse_file(std::filesystem::absolute(dst).string());
          loadFromToml(config);
        },
        [] {}, Size{500, 400});
  });

  saveImageButton->addClickListener([&]() {
    auto extensions =
        std::vector<FileExtensionSettings>{FileExtensionSettings{{"png"}, "png", ImVec4{1, 0, 0, 1}},
                                           FileExtensionSettings{{"jpg", "jpeg"}, "jpg", ImVec4{1, 0, 0, 1}},
                                           FileExtensionSettings{{"bmp"}, "bmp", ImVec4{1, 0, 0, 1}}};
    imguiInterface->openFileDialog(
        "Select save location", extensions,
        [&](const auto &selected) {
          const auto &dst = selected[0];
          onScreenshotSave(dst);
        },
        [] {}, Size{500, 400});
  });

  recordingWindow = &imguiInterface->createWindow("rec_window", "REC");
  recordingWindow->setUserResizable(false);
  recordingWindow->setCloseable(true);
  recordingWindow->setSize(Size{140, 60});
  recordingWindow->setVisibility(Visibility::Invisible);
  recorderPanel = &recordingWindow->createChild<RecorderPanel>("rec_panel");
  recordingWindow->setColor<style::ColorOf::TitleBackgroundActive>(ImVec4(65, 0, 0, 1));
  recordingWindow->setColor<style::ColorOf::TitleBackground>(ImVec4(65, 0, 0, 1));

  startRecordingButton->addClickListener([&] { recordingWindow->setVisibility(Visibility::Visible); });
  recorderPanel->addValueListener([&](const auto recState) {
    if (recState == RecordingState::Stopped) {
      recordingWindow->setCloseable(true);
    } else {
      recordingWindow->setCloseable(false);
    }
  });

  helpWindow = &imguiInterface->createWindow("help_window", ICON_FK_QUESTION_CIRCLE " Help");
  helpWindow->setCloseable(true);
  helpWindow->setSize(Size{700, 600});
  helpPanel = &helpWindow->createChild<HelpPanel>("help_panel", Size{Width::Auto(), -30}, std::move(helpLoader),
                                                  *imguiInterface);
  helpPanel->selectItem({"Introduction"});
  showHelpOnStartupCheckbox =
      &helpWindow->createChild<Checkbox>("show_startup_checkbox", "Show on startup", true, Persistent::Yes);

  helpButton->addClickListener([this] { helpWindow->setVisibility(Visibility::Visible); });

  aboutWindow = &imguiInterface->createWindow("about_window", "About");
  aboutPanel =
      &aboutWindow->createChild<AboutPanel>("about_panel", Size::Auto(), std::move(aboutLoader), *imguiInterface);
  aboutWindow->setCloseable(true);
  aboutWindow->setVisibility(Visibility::Invisible);
  aboutButton->addClickListener([&] {
    aboutWindow->setVisibility(Visibility::Visible);
  });

  updateSpeciesTabBarFromConfig(config);

  if (speciesPanels.empty()) {
    addDefaultSpecies();
    reloadSpeciesInteractions();
  }
  setMouseInteractionSpecies();

  addSpeciesButton->setTooltip("Add new species");

  imguiInterface->setStateFromConfig();
  helpWindow->setVisibility(showHelpOnStartupCheckbox->getValue() ? Visibility::Visible : Visibility::Invisible);
}

void pf::ogl::UI::setOutImage(const std::shared_ptr<Texture> &texture) {
  using namespace ui::ig;
  outImage = &outImageStretch->createChild<Image>("out_image", (ImTextureID) texture->getId(), Size{1920, 1080});
}

void pf::ogl::UI::setAllWinVisibility(bool visible) {
  using namespace ui::ig;
  const auto vis = visible ? Visibility::Visible : Visibility::Invisible;
  simWindow->setVisibility(vis);
  speciesWindow->setVisibility(vis);
  imagesWindow->setVisibility(vis);
  infoWindow->setVisibility(vis);
  interactionWindow->setVisibility(vis);
  viewSimWin->setValue(visible);
  viewImagesWin->setValue(visible);
  viewSpeciesWin->setValue(visible);
  viewInfoWin->setValue(visible);
  viewInteractWin->setValue(visible);
}

toml::array pf::ogl::UI::speciesToToml() const {
  const std::string speciesPanelPostfix = "_species_panel";
  toml::array speciesArr;
  std::ranges::for_each(speciesPanels, [&](const auto &panel) {
    panel->setPersistent(true);
    auto panelData = panel->serialize().value();
    const auto name = panel->getName();
    panelData.insert("speciesName", name.substr(0, name.length() - speciesPanelPostfix.length()));
    speciesArr.emplace_back<toml::table>(panelData);
    panel->setPersistent(false);
  });
  return speciesArr;
}

void pf::ogl::UI::loadFromToml(const toml::table &src) {
  using namespace ui::ig;
  speciesPanels.clear();
  auto tabNames = speciesTabBar->getTabs() | std::views::transform([](const auto &tab) { return tab.getName(); })
      | ranges::to_vector;
  for (const auto &tabName : tabNames) { speciesTabBar->removeTab(tabName); }

  updateSpeciesTabBarFromConfig(src);
}

void pf::ogl::UI::updateSpeciesTabBarFromConfig(const toml::table &config) {
  auto speciesToml = config["species"].as_array();
  if (speciesToml == nullptr) { return; }
  for (auto &s : *speciesToml) {
    auto &data = *s.as_table();
    createSpeciesTab(data["speciesName"].value<std::string>().value(), data);
  }
}

void pf::ogl::UI::addDefaultSpecies() { createSpeciesTab("default"); }

void pf::ogl::UI::addSpeciesTabCloseConfirmation(pf::ui::ig::Tab &tab, const std::string &speciesName) {
  using namespace ui::ig;
  tab.addOpenListener([&, speciesName](bool open) {
    if (open) { return; }
    imguiInterface->createMsgDlg("Remove species?", fmt::format("Do you want to remove species '{}'", speciesName),
                                 MessageButtons::Yes | MessageButtons::No, [&tab](auto btn) {
                                   if (btn == MessageButtons::No) { tab.setOpen(true); }
                                   return true;
                                 });
  });
}

void pf::ogl::UI::createSpeciesTab(const std::string &name) {
  using namespace ui::ig;
  using namespace physarum;
  auto &tab = speciesTabBar->addTab(name + "_species_tab", name, true);
  speciesPanels.emplace_back(&tab.createChild<SpeciesPanel>(name + "_species_panel"));
  addSpeciesTabCloseConfirmation(tab, name);
}

void pf::ogl::UI::createSpeciesTab(const std::string &name, const toml::table &src) {
  using namespace ui::ig;
  auto &tab = speciesTabBar->addTab(name + "_species_tab", name, true);
  speciesPanels.emplace_back(&tab.createChild<SpeciesPanel>(name + "_species_panel"))
      ->setConfig(physarum::PopulationConfig::FromToml(src));
  addSpeciesTabCloseConfirmation(tab, name);
}

void pf::ogl::UI::setMouseInteractionSpecies() {
  using namespace ui::ig;
  std::size_t idx = 0;
  std::vector<MouseInteractionSpecies> interInfo;
  for (const auto &tab : speciesTabBar->getTabs()) {
    const auto name = tab.getLabel();
    interInfo.emplace_back(idx, name);
    ++idx;
  }
  mouseInteractionPanel->setInteractableSpecies(interInfo);
}

void pf::ogl::UI::cleanupConfig(toml::table &config) { config.insert_or_assign("species", speciesToToml()); }

void pf::ogl::UI::reloadSpeciesInteractions() {
  using namespace physarum;
  int panelIndex = 0;
  std::ranges::for_each(speciesPanels, [&](const auto &panel) {
    int i = 0;
    auto previousInteractions = panel->getConfig().speciesInteractions;
    panel->clearInteractions();

    std::ranges::for_each(getSpeciesNames(), [&](const auto &name) {
      auto interactionType = i == panelIndex ? SpeciesInteraction::Follow : SpeciesInteraction::None;
      auto factor = 1.f;
      if (const auto iter = std::ranges::find(previousInteractions, name, &SpeciesInteractionConfig::speciesName);
          iter != previousInteractions.end()) {
        interactionType = iter->interactionType;
        factor = iter->factor;
      }
      panel->addInteraction(SpeciesInteractionConfig{interactionType, factor, name, i++});
    });
    ++panelIndex;
  });
}

void pf::ogl::UI::openHelp(const std::vector<std::string> &section) {
  helpWindow->setVisibility(ui::ig::Visibility::Visible);
  helpPanel->selectItem(section);
}
