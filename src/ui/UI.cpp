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
  simMenuBar = &windowSim->getMenuBar();
  fileSimSubmenu = &simMenuBar->addSubmenu("file_sim_submenu", "File");
  saveSimConfigButton = &fileSimSubmenu->addButtonItem("save_sim_button", "Save");
  loadSimConfigButton = &fileSimSubmenu->addButtonItem("load_sim_button", "Load");
  playPauseButton = &windowSim->createChild<Button>("btn_play_pause", "Start");
  applyOnChangeCheckbox = &windowSim->createChild<Checkbox>("apply_change_checkbox", "Apply on change");
  simControlGroup = &windowSim->createChild<Group>("group_sim_control", "Simulation controls", Persistent::Yes, AllowCollapse::Yes);
  simSpeedDrag = &simControlGroup->createChild<DragInput<int>>("sim_speed_drag", "Simulation speed", 1, 1, 10, 1, Persistent::Yes);
  senseAngleDrag = &simControlGroup->createChild<DragInput<float>>("sense_angle_drag", "Sense angle", 0.1f, -180.f, 180.f, 30.f, Persistent::Yes);
  senseDistanceDrag = &simControlGroup->createChild<DragInput<float>>("drag_sense_distance", "Sense distance", 1.0f, 0.1f, 1000.f, 35.f, Persistent::Yes);
  turnSpeedDrag = &simControlGroup->createChild<DragInput<float>>("drag_turn_speed", "Turn speed", 0.1f, 0.f, 100.f, 2.f, Persistent::Yes);
  movementSpeedDrag = &simControlGroup->createChild<DragInput<float>>("drag_move_speed", "Movement speed", 0.1f, -100.f, 100.f, 20.f, Persistent::Yes);
  trailWeightSpeedDrag = &simControlGroup->createChild<DragInput<float>>("drag_trail_weight", "Trail weight", 0.1f, 0.f, 1000.f, 5.f, Persistent::Yes);
  sep1 = &simControlGroup->createChild<Separator>("sep1");
  particleCountInput = &simControlGroup->createChild<Input<int>>("input_particle_count", "Particle count", 100, 1'000, 10'000, Persistent::Yes);
  particleCountInput->addValueListener([&](const auto value) {
    static auto previousValue = value;
    if (value <= 0) {
      particleCountInput->setValue(std::max(1, previousValue));
    }
  });
  particleInitCombobox = &simControlGroup->createChild<Combobox<physarum::ParticleStart>>("combobox_particle_start", "Init type", "Select", magic_enum::enum_values<physarum::ParticleStart>(), ComboBoxCount::Items8, Persistent::Yes);
  particleInitCombobox->setValue(physarum::ParticleStart::Random);
  restartSimButton = &simControlGroup->createChild<Button>("restart_sim", "Restart");
  trailControlGroup = &windowSim->createChild<Group>("group_trail_control", "Trail controls", Persistent::Yes, AllowCollapse::Yes);
  kernelSizeCombobox = &trailControlGroup->createChild<Combobox<int>>("combobox_kernel_size", "Kernel size", "select", std::vector<int>{1, 3, 5, 7, 9}, ComboBoxCount::Items8, Persistent::Yes);
  kernelSizeCombobox->setSelectedItem(3);
  diffuseRateDrag = &trailControlGroup->createChild<DragInput<float>>("drag_diffuse_rate", "Diffuse rate", 0.1f, 0.0f, 100.f, 3.f, Persistent::Yes);
  decayRateDrag = &trailControlGroup->createChild<DragInput<float>>("drag_decay_rate", "Decay rate", 0.01f, 0.0f, 1.f, .2f, Persistent::Yes);
  trailColorEdit = &trailControlGroup->createChild<ColorEdit<glm::vec4>>("trail_color_edit", "Trail color", glm::vec4{1.0}, Persistent::Yes);
  applyButton = &windowSim->createChild<Button>("button_apply", "Apply");

  senseAngleDrag->addValueListener([&](auto) {
    valueChange();
  });
  particleCountInput->addValueListener([&](auto) {
    valueChange();
  });
  senseDistanceDrag->addValueListener([&](auto) {
    valueChange();
  });
  turnSpeedDrag->addValueListener([&](auto) {
    valueChange();
  });
  movementSpeedDrag->addValueListener([&](auto) {
    valueChange();
  });
  trailWeightSpeedDrag->addValueListener([&](auto) {
    valueChange();
  });
  kernelSizeCombobox->addValueListener([&](auto) {
    valueChange();
  });
  diffuseRateDrag->addValueListener([&](auto) {
    valueChange();
  });
  decayRateDrag->addValueListener([&](auto) {
    valueChange();
  });
  trailColorEdit->addValueListener([&](auto) {
    valueChange();
  });

  applyButton->addClickListener([&] {
    onConfigChange(getConfig());
  });

  saveSimConfigButton->addClickListener([&] {
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
  });

  imguiInterface->setStateFromConfig();
}

pf::physarum::SimConfig pf::ogl::UI::getConfig() const {
  auto result = physarum::SimConfig{
      .senseAngle = senseAngleDrag->getValue(),
      .senseDistance = senseDistanceDrag->getValue(),
      .turnSpeed = turnSpeedDrag->getValue(),
      .movementSpeed = movementSpeedDrag->getValue(),
      .trailWeight = trailWeightSpeedDrag->getValue(),
      .blurKernelSize = kernelSizeCombobox->getValue(),
      .diffuseRate = diffuseRateDrag->getValue(),
      .decayRate = decayRateDrag->getValue(),
      .particleStart = particleInitCombobox->getValue(),
      .particleCount = particleCountInput->getValue()};
  return result;
}

void pf::ogl::UI::valueChange() {
  if (applyOnChangeCheckbox->getValue()) {
    onConfigChange(getConfig());
  }
}
void pf::ogl::UI::loadFromConfig(const pf::physarum::SimConfig &config) {
  senseAngleDrag->setValue(config.senseAngle);
  senseDistanceDrag->setValue(config.senseDistance);
  turnSpeedDrag->setValue(config.turnSpeed);
  movementSpeedDrag->setValue(config.movementSpeed);
  trailWeightSpeedDrag->setValue(config.trailWeight);
  kernelSizeCombobox->setValue(config.blurKernelSize);
  diffuseRateDrag->setValue(config.diffuseRate);
  decayRateDrag->setValue(config.decayRate);
  particleInitCombobox->setValue(config.particleStart);
  particleCountInput->setValue(config.particleCount);

  onConfigChange(getConfig());
}
