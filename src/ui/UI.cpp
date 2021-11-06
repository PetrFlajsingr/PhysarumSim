//
// Created by xflajs00 on 22.10.2021.
//

#include "UI.h"
#include <pf_imgui/backends/ImGuiGlfwOpenGLInterface.h>
#include <pf_imgui/styles/dark.h>

pf::ogl::UI::UI(const toml::table &config, GLFWwindow *windowHandle) {
  using namespace ui::ig;
  using namespace enum_operators;
  imguiInterface = std::make_unique<ImGuiGlfwOpenGLInterface>(ImGuiGlfwOpenGLConfig{
      .windowHandle = windowHandle,
      .flags = {},
      .enableMultiViewport = true,
      .config = config,
      .pathToIconFolder = *config["path_icons"].value<std::string>(),
      .enabledIconPacks = IconPack::FontAwesome5Regular,
      .defaultFontSize = 13.f});
  setDarkStyle(*imguiInterface);

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

  windowSim = &imguiInterface->createWindow("sim_window", "Simulation");
  viewSimWin->addValueListener([&](bool value) {
    windowSim->setVisibility(value ? Visibility::Visible : Visibility::Invisible);
  },
                               true);
  windowSim->addCloseListener([&]() {
    viewSimWin->setValue(false);
  });
  windowSim->setCloseable(true);
  windowSim->setIsDockable(true);
  /*  simMenuBar = &windowSim->getMenuBar();
  fileSimSubmenu = &simMenuBar->addSubmenu("file_sim_submenu", "File");
  saveSimConfigButton = &fileSimSubmenu->addButtonItem("save_sim_button", "Save");
  loadSimConfigButton = &fileSimSubmenu->addButtonItem("load_sim_button", "Load");*/
  playPauseButton = &windowSim->createChild<Button>("btn_play_pause", "Start");
  simControlGroup = &windowSim->createChild<Group>("group_sim_control", "Simulation controls", Persistent::Yes, AllowCollapse::Yes);
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
  blendTypeCombobox = &speciesWindow->createChild<Combobox<BlendType>>("blend_type_combobox", "Blend type", "Select", magic_enum::enum_values<BlendType>(), ComboBoxCount::Items8, Persistent::Yes);
  blendTypeCombobox->setSelectedItem(BlendType::AlphaMix);
  backgroundColorEdit = &speciesWindow->createChild<ColorEdit<glm::vec3>>("background_color_edit", "Background", glm::vec3{.0f}, Persistent::Yes);
  speciesButtonLayout = &speciesWindow->createChild<BoxLayout>("species_buttons_layout", LayoutDirection::LeftToRight, Size{Width::Fill(), 30}, AllowCollapse::No, ShowBorder::No, Persistent::Yes);

  speciesTabBar = &speciesWindow->createChild<TabBar>("species_tabbar", true);
  addSpeciesButton = &speciesTabBar->addTabButton("add_species_button", "+", TabMod::ForceRight);
  addSpeciesButton->addClickListener([&] {
    imguiInterface->openInputDialog(
        "Species name", "Input species name", [&](const auto input) {
          auto &tab = speciesTabBar->addTab(input + "_species_tab", input, true);
          speciesPanels.emplace_back(&tab.createChild<SpeciesPanel>(input + "_panel"));

          tab.addOpenListener([&, input](bool open) {
            if (open) { return;}
            imguiInterface->createMsgDlg("Remove species?", fmt::format("Do you want to remove species '{}'", input), MessageButtons::Yes | MessageButtons::No,
                [&tab] (auto btn) {
                                   if (btn == MessageButtons::No) {tab.setOpen();}
                                   return true;});
          }); }, [] {});
  });

  restartSimButton->addClickListener([&] {
    auto closedTabs = speciesTabBar->getTabs() | std::views::filter([](const auto &tab) {
                        return !tab.isOpen();
                      });
    for (auto &tab : closedTabs) {
      speciesPanels.erase(std::ranges::find_if(speciesPanels, [&](const auto &panel) { return panel->getName() == tab.getLabel() + "_panel"; }));
      tab.removeChild(tab.getLabel() + "_panel");
    }

    auto closedTabNames = closedTabs | std::views::transform(&Tab::getName) | ranges::to_vector;
    std::ranges::for_each(closedTabNames, [&](const auto &tabName) {
      speciesTabBar->removeTab(tabName);
    });
  });

  /* saveSimConfigButton->addClickListener([&] {
    imguiInterface->openFileDialog(
        "Select save location", {ui::ig::FileExtensionSettings{{"toml"}, "toml", ImVec4{1, 0, 0, 1}}},
        [&](const auto &selected) {
          const auto &dst = selected[0];
          const auto data = getConfig().toToml();
          std::ofstream ostream{dst};
          ostream << data;
        },
        [] {}, ui::ig::Size{500, 400});
  });

  loadSimConfigButton->addClickListener([&] {
    imguiInterface->openFileDialog(
        "Select save location", {ui::ig::FileExtensionSettings{{"toml"}, "toml", ImVec4{1, 0, 0, 1}}},
        [&](const auto &selected) {
          const auto &dst = selected[0];
          const auto config = physarum::SimConfig::FromToml(toml::parse_file(std::filesystem::absolute(dst).string()));
          loadFromConfig(config);
        },
        [] {}, ui::ig::Size{500, 400});
  });*/

  imguiInterface->setStateFromConfig();
}

void pf::ogl::UI::setOutImage(std::shared_ptr<Texture> texture) {
  using namespace ui::ig;
  outImage = &outImageStretch->createChild<Image>("out_image", (ImTextureID) texture->getId(), Size{1920, 1080}, IsButton::No, [] {
    return std::pair(ImVec2{0, 1}, ImVec2{1, 0});
  });
}

void pf::ogl::UI::setAllWinVisibility(bool visible) {
  using namespace ui::ig;
  const auto vis = visible ? Visibility::Visible : Visibility::Invisible;
  windowSim->setVisibility(vis);
  speciesWindow->setVisibility(vis);
  imagesWindow->setVisibility(vis);
  viewSimWin->setValue(visible);
  viewImagesWin->setValue(visible);
  viewSpeciesWin->setValue(visible);
}
