//
// Created by xflajs00 on 22.10.2021.
//

#include "UI.h"
#include <pf_imgui/backends/ImGuiGlfwOpenGLInterface.h>
#include <pf_imgui/styles/dark.h>

pf::ogl::UI::UI(const toml::table &config, GLFWwindow *windowHandle) {
  using namespace ui::ig;
  imguiInterface = std::make_unique<ImGuiGlfwOpenGLInterface>(ImGuiGlfwOpenGLConfig{
      .windowHandle = windowHandle,
      .flags = {},
      .enableMultiViewport = true,
      .config = config,
      .pathToIconFolder = *config["path_icons"].value<std::string>(),
      .enabledIconPacks = IconPack::FontAwesome5Regular,
      .defaultFontSize = 13.f});
  setDarkStyle(*imguiInterface);

  windowSim = &imguiInterface->createWindow("sim_window", "Simulation");
  windowSim->setIsDockable(true);
  windowSim->setCollapsible(true);
/*  simMenuBar = &windowSim->getMenuBar();
  fileSimSubmenu = &simMenuBar->addSubmenu("file_sim_submenu", "File");
  saveSimConfigButton = &fileSimSubmenu->addButtonItem("save_sim_button", "Save");
  loadSimConfigButton = &fileSimSubmenu->addButtonItem("load_sim_button", "Load");*/
  playPauseButton = &windowSim->createChild<Button>("btn_play_pause", "Start");
  simControlGroup = &windowSim->createChild<Group>("group_sim_control", "Simulation controls", Persistent::Yes, AllowCollapse::Yes);
  simSpeedDrag = &simControlGroup->createChild<DragInput<int>>("sim_speed_drag", "Simulation speed", 1, 1, 10, 1, Persistent::Yes);
  restartSimButton = &simControlGroup->createChild<Button>("restart_sim", "Restart");

  imagesWindow = &imguiInterface->createWindow("image_window", "Images");
  imagesWindow->setIsDockable(true);
  outImageStretch = &imagesWindow->createChild<StretchLayout>("out_img_stretch", Size::Auto(), Stretch::All);

  speciesWindow = &imguiInterface->createWindow("species_window", "Species");
  speciesWindow->setIsDockable(true);
  speciesWindow->setCollapsible(true);
  backgroundColorEdit = &speciesWindow->createChild<ColorEdit<glm::vec3>>("background_color_edit", "Background", glm::vec3{.0f}, Persistent::Yes);
  speciesButtonLayout = &speciesWindow->createChild<BoxLayout>("species_buttons_layout", LayoutDirection::LeftToRight, Size{Width::Fill(), 30}, AllowCollapse::No, ShowBorder::No, Persistent::Yes);
  addSpeciesButton = &speciesButtonLayout->createChild<Button>("add_species_layout", "Add species");
  removeSpeciesButton = &speciesButtonLayout->createChild<Button>("remove_species_layout", "Remove species");
  speciesTabBar = &speciesWindow->createChild<TabBar>("species_tabbar", true);

  auto &species1Tab = speciesTabBar->addTab("tab_test", "Species 1");
  auto species1Panel = speciesPanels.emplace_back(&species1Tab.createChild<SpeciesPanel>("species1", Persistent::Yes));

  /*auto &species2Tab = speciesTabBar->addTab("tab_test2", "Species 2");
  auto species2Panel = speciesPanels.emplace_back(&species1Tab.createChild<SpeciesPanel>("species2", Persistent::Yes));*/




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
