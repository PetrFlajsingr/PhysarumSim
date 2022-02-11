//
// Created by xflajs00 on 03.11.2021.
//

#include "SpeciesPanel.h"
#include <pf_imgui/elements/Separator.h>

namespace pf {

using namespace ui::ig;
using namespace physarum;

SpeciesPanel::SpeciesPanel(const std::string &name, ui::ig::Persistent persistent)
    : Element(name), ValueObservable(), Savable(persistent), root({.name = name + "_root",
                                                                   .layoutDirection = LayoutDirection::TopToBottom,
                                                                   .size = Size::Auto(),
                                                                   .showBorder = ShowBorder::Yes,
                                                                   .persistent = persistent}) {
  createElements();
  registerListeners();
  createTooltips();
}

void SpeciesPanel::renderImpl() { root.render(); }

void SpeciesPanel::unserialize_impl(const toml::table &src) {
  auto config = PopulationConfig::FromToml(src);
  setConfig(config);
}

toml::table SpeciesPanel::serialize_impl() const {
  auto result = getConfig().toToml();
  return result;
}

PopulationConfig SpeciesPanel::getConfig() const {
  auto result = PopulationConfig{.senseAngle = senseAngleDrag->getValue(),
                                 .senseDistance = senseDistanceDrag->getValue(),
                                 .turnSpeed = turnSpeedDrag->getValue(),
                                 .movementSpeed = movementSpeedDrag->getValue(),
                                 .trailWeight = trailWeightDrag->getValue(),
                                 .blurKernelSize = kernelSizeCombobox->getValue(),
                                 .diffuseRate = diffuseRateDrag->getValue(),
                                 .decayRate = decayRateDrag->getValue(),
                                 .maxTrailValue = maxTrailValueDrag->getValue(),
                                 .particleStart = particleInitCombobox->getValue(),
                                 .particleCount = particleCountInput->getValue(),
                                 .sensorSize = sensorSizeCombobox->getValue(),
                                 .color = colorPanel->getValue(),
                                 .filterType = filterTypeCombobox->getValue(),
                                 .maxSteerRandomness = maxSteerRandomnessDrag->getValue(),
                                 .speciesInteractions = interactionsListbox->getItems() | ranges::to_vector};
  return result;
}

void SpeciesPanel::setConfig(const PopulationConfig &config) {
  senseAngleDrag->setValue(config.senseAngle);
  senseDistanceDrag->setValue(config.senseDistance);
  turnSpeedDrag->setValue(config.turnSpeed);
  movementSpeedDrag->setValue(config.movementSpeed);
  maxSteerRandomnessDrag->setValue(config.maxSteerRandomness);
  trailWeightDrag->setValue(config.trailWeight);
  kernelSizeCombobox->setSelectedItem(config.blurKernelSize);
  diffuseRateDrag->setValue(config.diffuseRate);
  decayRateDrag->setValue(config.decayRate);
  maxTrailValueDrag->setValue(config.maxTrailValue);
  particleInitCombobox->setSelectedItem(config.particleStart);
  particleCountInput->setValue(config.particleCount);
  sensorSizeCombobox->setValue(config.sensorSize);
  colorPanel->setColor(config.color);
  filterTypeCombobox->setSelectedItem(config.filterType);
  interactionsListbox->clearItems();
  std::ranges::for_each(config.speciesInteractions, [&](const auto &interaction) { addInteraction(interaction); });
}

void SpeciesPanel::createElements() {
  particleInitCombobox = &root.createChild<Combobox<ParticleStart>>(getName() + "combobox_particle_start", "Init type",
                                                                    "Select", magic_enum::enum_values<ParticleStart>());
  particleInitCombobox->setSelectedItem(ParticleStart::Random);
  particleCountInput =
      &root.createChild<Input<int>>(getName() + "input_particle_count", "Particle count", 100, 1'000, 100'000);
  particleCountInput->addValueListener([&](const auto value) {
    static auto previousValue = value;
    if (value <= 0) { particleCountInput->setValue(std::max(1, previousValue)); }
  });

  root.createChild<Separator>(getName() + "sep1");

  senseAngleDrag =
      &root.createChild<DragInput<float>>(getName() + "sense_angle_drag", "Sense angle", 0.1f, -180.f, 180.f, 30.f);
  senseDistanceDrag = &root.createChild<DragInput<float>>(getName() + "drag_sense_distance", "Sense distance", 1.0f,
                                                          0.1f, 1000.f, 35.f);
  sensorSizeCombobox = &root.createChild<Combobox<int>>(getName() + "combobox_sensor_size", "Sensor size", "select",
                                                        std::vector<int>{1, 3, 5, 7, 9});
  sensorSizeCombobox->setSelectedItem(1);
  turnSpeedDrag =
      &root.createChild<DragInput<float>>(getName() + "drag_turn_speed", "Turn speed", 0.1f, 0.f, 100.f, 2.f);
  movementSpeedDrag =
      &root.createChild<DragInput<float>>(getName() + "drag_move_speed", "Movement speed", 0.1f, -100.f, 100.f, 20.f);
  maxSteerRandomnessDrag = &root.createChild<DragInput<float>>(getName() + "drag_max_rand_steer",
                                                               "Max steer randomness", 0.1f, .0f, 1.f, 1.f);
  trailWeightDrag =
      &root.createChild<DragInput<float>>(getName() + "drag_trail_weight", "Trail weight", 0.1f, 0.f, 1000.f, 5.f);

  root.createChild<Separator>(getName() + "sep2");

  filterTypeCombobox = &root.createChild<Combobox<FilterType>>(getName() + "combobox_filter_size", "Filter", "select",
                                                               magic_enum::enum_values<FilterType>());
  filterTypeCombobox->setSelectedItem(FilterType::Blur);
  kernelSizeCombobox = &root.createChild<Combobox<int>>(getName() + "combobox_kernel_size", "Kernel size", "select",
                                                        std::vector<int>{1, 3, 5, 7, 9});
  kernelSizeCombobox->setSelectedItem(3);
  diffuseRateDrag =
      &root.createChild<DragInput<float>>(getName() + "drag_diffuse_rate", "Diffuse rate", 0.1f, 0.0f, 100.f, 3.f);
  decayRateDrag =
      &root.createChild<DragInput<float>>(getName() + "drag_decay_rate", "Decay rate", 0.01f, 0.0f, 1.f, .2f);
  maxTrailValueDrag =
      &root.createChild<DragInput<float>>(getName() + "drag_max_trail", "Max trail value", 0.01f, 0.01f, 10.f, 1.f);

  colorPanel = &root.createChild<SpeciesColorPanel>(getName() + "color_panel");

  interactionsListbox = &root.createChild<SpeciesInteractionListbox>(getName() + "inter_listbox", "Interactions",
                                                                     Size(Width::Auto(), Height::Fill()));
}

void SpeciesPanel::registerListeners() {
  const auto setChange = [this](auto) { setValue(getConfig()); };
  senseAngleDrag->addValueListener(setChange);
  particleCountInput->addValueListener(setChange);
  senseDistanceDrag->addValueListener(setChange);
  turnSpeedDrag->addValueListener(setChange);
  movementSpeedDrag->addValueListener(setChange);
  maxSteerRandomnessDrag->addValueListener(setChange);
  trailWeightDrag->addValueListener(setChange);
  kernelSizeCombobox->addValueListener(setChange);
  sensorSizeCombobox->addValueListener(setChange);
  diffuseRateDrag->addValueListener(setChange);
  decayRateDrag->addValueListener(setChange);
  maxTrailValueDrag->addValueListener(setChange);
  colorPanel->addValueListener(setChange);
  filterTypeCombobox->addValueListener(setChange);

  filterTypeCombobox->addValueListener([&](const auto value) {
    kernelSizeCombobox->setVisibility(value == FilterType::Blur ? Visibility::Visible : Visibility::Invisible);
  });
}

void SpeciesPanel::createTooltips() {
  particleInitCombobox->setTooltip("Initial particle distribution");
  particleCountInput->setTooltip("Count of particles in the population");
  senseAngleDrag->setTooltip("Angle offset of trail sensor");
  senseDistanceDrag->setTooltip("Distance offset of trail sensor");
  sensorSizeCombobox->setTooltip("Size of trail sensor (kernel)");
  turnSpeedDrag->setTooltip("Turning speed of the particle");
  movementSpeedDrag->setTooltip("Movement speed of the particle");
  maxSteerRandomnessDrag->setTooltip("Maximum random change during turning");
  trailWeightDrag->setTooltip("Amount of trail stored in each trail");
  filterTypeCombobox->setTooltip("Type of filter applied for diffusion");
  kernelSizeCombobox->setTooltip("Size of filter's kernel");
  diffuseRateDrag->setTooltip("Speed of trail diffusion");
  decayRateDrag->setTooltip("Speed of trail decay");
  maxTrailValueDrag->setTooltip("Highest possible trail value - may cause color artifacts");
}

void SpeciesPanel::addInteraction(const SpeciesInteractionConfig &interConfig) {
  const auto index = interactionsListbox->getItems().size();
  interactionsListbox->addItem(interConfig)
      .addValueListener([&, index](const physarum::SpeciesInteractionConfig &iConfig) {
        auto config = getConfig();
        config.speciesInteractions[index] = iConfig;
        setValue(config);
      });
}
void SpeciesPanel::clearInteractions() { interactionsListbox->clearItems(); }

}// namespace pf