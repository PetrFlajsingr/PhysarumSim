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
  kernelSizeRadioGroup = &trailControlGroup->createChild<RadioGroup>("radio_group_kernel_size", "Kernel size", std::vector<std::unique_ptr<RadioButton>>{}, std::nullopt, Persistent::Yes);
  kernelSizeRadioGroup->addButton("1", "1", false);
  kernelSizeRadioGroup->addButton("3", "3", true);
  kernelSizeRadioGroup->addButton("5", "5", false);
  kernelSizeRadioGroup->addButton("7", "7", false);
  kernelSizeRadioGroup->addButton("9", "9", false);
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
  kernelSizeRadioGroup->addValueListener([&](auto) {
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

  imguiInterface->setStateFromConfig();
}

pf::physarum::SimConfig pf::ogl::UI::getConfig() const {
  return {
      .senseAngle = senseAngleDrag->getValue(),
      .senseDistance = senseDistanceDrag->getValue(),
      .turnSpeed = turnSpeedDrag->getValue(),
      .movementSpeed = movementSpeedDrag->getValue(),
      .trailWeight = trailWeightSpeedDrag->getValue(),
      .blurKernelSize = std::atoi(std::string{kernelSizeRadioGroup->getValue()}.c_str()),
      .diffuseRate = diffuseRateDrag->getValue(),
      .decayRate = decayRateDrag->getValue(),
      .particleStart = particleInitCombobox->getValue(),
      .particleCount = particleCountInput->getValue()};
}

void pf::ogl::UI::valueChange() {
  if (applyOnChangeCheckbox->getValue()) {
    onConfigChange(getConfig());
  }
}
