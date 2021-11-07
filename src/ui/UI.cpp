//
// Created by xflajs00 on 22.10.2021.
//

#include "UI.h"
#include <pf_imgui/backends/ImGuiGlfwOpenGLInterface.h>
#include <pf_imgui/enums.h>
#include <pf_imgui/styles/dark.h>

using namespace pf::enum_operators;

pf::ogl::UI::UI(const toml::table &config, GLFWwindow *windowHandle) {
  using namespace ui::ig;
  imguiInterface = std::make_unique<ImGuiGlfwOpenGLInterface>(ImGuiGlfwOpenGLConfig{
      .windowHandle = windowHandle,
      .flags = {},
      .enableMultiViewport = false,
      .config = config,
      .pathToIconFolder = *config["path_icons"].value<std::string>(),
      .enabledIconPacks = IconPack::FontAwesome5Regular,
      .defaultFontSize = 13.f});
  setDarkStyle(*imguiInterface);


  dockWindow = &imguiInterface->createWindow("dock_window", "Test");
  dockWindow->setUserResizable(false);
  dockWindow->setUserMovable(false);
  dockWindow->setTitleBarVisible(false);
  mainDockspace = &dockWindow->createChild<StretchLayout>("dock_stretch_layout", Size::Fill(), Stretch::All).createChild<DockSpace>("main_dockspace", Size::Auto());

  appMenuBar = &imguiInterface->getMenuBar();
  viewSubmenu = &appMenuBar->addSubmenu("app_view_submenu", "View");
  viewShowAll = &viewSubmenu->addButtonItem("show_all_view", "Show all");
  viewHideAll = &viewSubmenu->addButtonItem("hide_all_view", "Hide all");
  viewShowAll->addClickListener([&] {
    setAllWinVisibility(true);
  });
  viewHideAll->addClickListener([&] {
    setAllWinVisibility(false);
  });
  viewSubmenu->addSeparator("view_submenu_separator");
  viewSimWin = &viewSubmenu->addCheckboxItem("show_sim_menu", "Simulation", true, Persistent::Yes);
  viewImagesWin = &viewSubmenu->addCheckboxItem("show_images_menu", "Images", true, Persistent::Yes);
  viewSpeciesWin = &viewSubmenu->addCheckboxItem("show_species_menu", "Species", true, Persistent::Yes);
  viewInfoWin = &viewSubmenu->addCheckboxItem("show_info_menu", "Info", true, Persistent::Yes);
  viewInteractWin = &viewSubmenu->addCheckboxItem("show_interact_menu", "Interaction", true, Persistent::Yes);

  interactionWindow = &imguiInterface->createWindow("interaction_window", "Interaction");
  interactionWindow->addCloseListener([&]() {
    viewInteractWin->setValue(false);
  });
  viewInteractWin->addValueListener([&](bool value) {
    interactionWindow->setVisibility(value ? Visibility::Visible : Visibility::Invisible);
  },
                                    true);
  interactionWindow->setCloseable(true);
  interactionWindow->setIsDockable(true);
  mouseInteractionPanel = &interactionWindow->createChild<MouseInteractionPanel>("interaction_panel", Persistent::Yes);

  infoWindow = &imguiInterface->createWindow("info_window", "Info");
  fpsCurrentPlot = &infoWindow->createChild<SimplePlot>("fps_plot", "Fps current", PlotType::Lines,
                                                        std::vector<float>{}, std::nullopt, 200, 0, FLT_MAX,
                                                        Size{Width::Auto(), 30});
  fpsAveragePlot = &infoWindow->createChild<SimplePlot>("fps_avg_plot", "Fps average", PlotType::Lines,
                                                        std::vector<float>{}, std::nullopt, 200, 0, FLT_MAX,
                                                        Size{Width::Auto(), 30});
  fpsLabel = &infoWindow->createChild<Text>("fps_label", "Average FPS: {}");
  infoWindow->addCloseListener([&]() {
    viewInfoWin->setValue(false);
  });
  viewInfoWin->addValueListener([&](bool value) {
    infoWindow->setVisibility(value ? Visibility::Visible : Visibility::Invisible);
  },
                                true);
  infoWindow->setCloseable(true);
  infoWindow->setIsDockable(true);

  simWindow = &imguiInterface->createWindow("sim_window", "Simulation");
  viewSimWin->addValueListener([&](bool value) {
    simWindow->setVisibility(value ? Visibility::Visible : Visibility::Invisible);
  },
                               true);
  simWindow->addCloseListener([&]() {
    viewSimWin->setValue(false);
  });
  simWindow->setCloseable(true);
  simWindow->setIsDockable(true);

  playPauseButton = &simWindow->createChild<Button>("btn_play_pause", "Start");
  simControlGroup = &simWindow->createChild<Group>("group_sim_control", "Simulation controls", Persistent::Yes, AllowCollapse::Yes);
  simSpeedDrag = &simControlGroup->createChild<DragInput<int>>("sim_speed_drag", "Simulation speed", 1, 1, 10, 1, Persistent::Yes);
  restartSimButton = &simControlGroup->createChild<Button>("restart_sim", "Restart");

  imagesWindow = &imguiInterface->createWindow("image_window", "Images");
  viewImagesWin->addValueListener([&](bool value) {
    imagesWindow->setVisibility(value ? Visibility::Visible : Visibility::Invisible);
  },
                                  true);
  imagesWindow->addCloseListener([&]() {
    viewImagesWin->setValue(false);
  });
  imagesWindow->setCloseable(true);
  imagesWindow->setIsDockable(true);
  outImageStretch = &imagesWindow->createChild<StretchLayout>("out_img_stretch", Size::Auto(), Stretch::All);

  speciesWindow = &imguiInterface->createWindow("species_window", "Species");
  viewSpeciesWin->addValueListener([&](bool value) {
    speciesWindow->setVisibility(value ? Visibility::Visible : Visibility::Invisible);
  },
                                   true);
  speciesWindow->addCloseListener([&]() {
    viewSpeciesWin->setValue(false);
  });
  speciesWindow->setCloseable(true);
  speciesWindow->setIsDockable(true);
  speciesWindow->setCollapsible(true);

  speciesMenuBar = &speciesWindow->getMenuBar();
  fileSpeciesSubmenu = &speciesMenuBar->addSubmenu("species_file_submenu", "File");
  saveSpeciesButton = &fileSpeciesSubmenu->addButtonItem("species_save_button", "Save");
  loadSpeciesButton = &fileSpeciesSubmenu->addButtonItem("species_load_button", "Load");

  blendTypeCombobox = &speciesWindow->createChild<Combobox<BlendType>>("blend_type_combobox", "Blend type", "Select", magic_enum::enum_values<BlendType>(), ComboBoxCount::Items8, Persistent::Yes);
  blendTypeCombobox->setSelectedItem(BlendType::AlphaMix);
  backgroundColorEdit = &speciesWindow->createChild<ColorEdit<glm::vec3>>("background_color_edit", "Background", glm::vec3{.0f}, Persistent::Yes);
  speciesButtonLayout = &speciesWindow->createChild<BoxLayout>("species_buttons_layout", LayoutDirection::LeftToRight, Size{Width::Fill(), 30}, AllowCollapse::No, ShowBorder::No, Persistent::Yes);

  speciesTabBar = &speciesWindow->createChild<TabBar>("species_tabbar", true);
  addSpeciesButton = &speciesTabBar->addTabButton("add_species_button", "+", TabMod::ForceRight);
  addSpeciesButton->addClickListener([&] {
    imguiInterface->openInputDialog(
        "Species name", "Input species name", [&](const auto input) {
          auto names = getSpeciesNames();
          if (std::ranges::find(names, input) != names.end()) {
            imguiInterface->createMsgDlg("Duplicate name", fmt::format("The name '{}' is already present.", input), Flags{MessageButtons::Ok}, [](auto) {return true;});
            return;
          }
          createSpeciesTab(input);
          reloadSpeciesInteractions();
        }, [] {});
  });

  restartSimButton->addClickListener([&] {
    auto closedTabs = speciesTabBar->getTabs() | std::views::filter([](const auto &tab) {
                        return !tab.isOpen();
                      });
    for (auto &tab : closedTabs) {
      speciesPanels.erase(std::ranges::find_if(speciesPanels, [&](const auto &panel) { return panel->getName() == tab.getLabel() + "_species_panel"; }));
      tab.removeChild(tab.getLabel() + "_species_panel");
    }

    auto closedTabNames = closedTabs | std::views::transform(&Tab::getName) | ranges::to_vector;
    std::ranges::for_each(closedTabNames, [&](const auto &tabName) {
      speciesTabBar->removeTab(tabName);
    });
    setMouseInteractionSpecies();
  });

  saveSpeciesButton->addClickListener([&] {
    imguiInterface->openFileDialog(
        "Select save location", {ui::ig::FileExtensionSettings{{"toml"}, "toml", ImVec4{1, 0, 0, 1}}},
        [&](const auto &selected) {
          const auto &dst = selected[0];
          const auto data = speciesToToml();
          std::ofstream ostream{dst};
          ostream << data;
        },
        [] {}, ui::ig::Size{500, 400});
  });

  loadSpeciesButton->addClickListener([&] {
    imguiInterface->openFileDialog(
        "Select file to load", {ui::ig::FileExtensionSettings{{"toml"}, "toml", ImVec4{1, 0, 0, 1}}},
        [&](const auto &selected) {
          const auto &dst = selected[0];
          const auto config = toml::parse_file(std::filesystem::absolute(dst).string());
          loadFromToml(config);
        },
        [] {}, ui::ig::Size{500, 400});
  });

  updateSpeciesTabBarFromConfig(config);

  if (speciesPanels.empty()) {
    addDefaultSpecies();
    reloadSpeciesInteractions();
  }
  setMouseInteractionSpecies();

  addSpeciesButton->setTooltip("Add new species");

  imguiInterface->setStateFromConfig();
}

void pf::ogl::UI::setOutImage(const std::shared_ptr<Texture> &texture) {
  using namespace ui::ig;
  outImage = &outImageStretch->createChild<Image>("out_image", (ImTextureID) texture->getId(), Size{1920, 1080}, IsButton::No, true, [] {
    return std::pair(ImVec2{0, 1}, ImVec2{1, 0});
  });
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

toml::table pf::ogl::UI::speciesToToml() const {
  toml::table result{};
  auto openTabs = speciesTabBar->getTabs() | std::views::filter([](const auto &tab) {
                    return tab.isOpen();
                  });

  for (auto &tab : openTabs) {
    auto panel = *std::ranges::find_if(speciesPanels, [&](const auto &panel) { return panel->getName() == tab.getLabel() + "_species_panel"; });
    result.insert(tab.getLabel(), panel->getConfig().toToml());
  }
  return result;
}

void pf::ogl::UI::loadFromToml(const toml::table &src) {
  using namespace ui::ig;
  speciesPanels.clear();
  auto tabNames = speciesTabBar->getTabs() | std::views::transform([](const auto &tab) {
                    return tab.getName();
                  })
      | ranges::to_vector;
  for (const auto &tabName : tabNames) {
    speciesTabBar->removeTab(tabName);
  }

  for (const auto &[name, data] : src) {
    createSpeciesTab(name, *data.as_table());
  }
}

void pf::ogl::UI::updateSpeciesTabBarFromConfig(const toml::table &config) {
  using namespace ui::ig;
  const std::string speciesPanelPostfix = "_species_panel";
  for (const auto &[name, data] : config) {
    if (!name.ends_with(speciesPanelPostfix)) {
      continue;
    }
    speciesInConfig.emplace_back(name);
    const auto speciesName = name.substr(0, name.length() - speciesPanelPostfix.length());
    createSpeciesTab(speciesName, *data.as_table());
  }
}

void pf::ogl::UI::addDefaultSpecies() {
  createSpeciesTab("default");
}

void pf::ogl::UI::addSpeciesTabCloseConfirmation(pf::ui::ig::Tab &tab, const std::string &speciesName) {
  using namespace ui::ig;
  tab.addOpenListener([&, speciesName](bool open) {
    if (open) { return; }
    imguiInterface->createMsgDlg("Remove species?", fmt::format("Do you want to remove species '{}'", speciesName), MessageButtons::Yes | MessageButtons::No,
                                 [&tab](auto btn) {
                                   if (btn == MessageButtons::No) {tab.setOpen(true);}
                                   return true; });
  });
}

void pf::ogl::UI::createSpeciesTab(const std::string &name) {
  using namespace ui::ig;
  using namespace physarum;
  auto &tab = speciesTabBar->addTab(name + "_species_tab", name, true);
  auto newPanel = speciesPanels.emplace_back(&tab.createChild<SpeciesPanel>(name + "_species_panel", Persistent::Yes));
  addSpeciesTabCloseConfirmation(tab, name);
}

void pf::ogl::UI::createSpeciesTab(const std::string &name, const toml::table &src) {
  using namespace ui::ig;
  auto &tab = speciesTabBar->addTab(name + "_species_tab", name, true);
  speciesPanels.emplace_back(&tab.createChild<SpeciesPanel>(name + "_species_panel", Persistent::Yes))->setConfig(physarum::PopulationConfig::FromToml(src));
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

void pf::ogl::UI::cleanupConfig(toml::table &config) {
  auto speciesToErase = speciesInConfig | std::views::filter([&](const auto &speciesName) {
                          return std::ranges::find(speciesPanels, speciesName, [](const auto &panel) { return panel->getName(); }) == speciesPanels.end();
                        });
  std::ranges::for_each(speciesToErase, [&](const auto &erase) {
    config.erase(config.find(erase));
  });
}

void pf::ogl::UI::reloadSpeciesInteractions() {
  using namespace physarum;
  int panelIndex = 0;
  std::ranges::for_each(speciesPanels, [&](const auto &panel) {
    int i = 0;
    auto previousInteractions = panel->interactionsListbox->getItems() | ranges::to_vector;
    panel->interactionsListbox->clearItems();

    std::ranges::for_each(getSpeciesNames(), [&](const auto &name) {
      auto interactionType = i == panelIndex ? SpeciesInteraction::Follow : SpeciesInteraction::None;
      if (const auto iter = std::ranges::find(previousInteractions, name, &SpeciesInteractionConfig::speciesName); iter != previousInteractions.end()) {
        interactionType = iter->interactionType;
      }
      panel->interactionsListbox->addItem(SpeciesInteractionConfig{interactionType, 1.f, name, i++});
    });
    ++panelIndex;
  });
}
