//
// Created by xflajs00 on 06.11.2021.
//

#include "MouseInteractionPanel.h"

#include <utility>

namespace pf {

using namespace ui::ig;
using namespace physarum;

MouseInteractionSpecies::MouseInteractionSpecies(int speciesId, std::string speciesName) : speciesId(speciesId), speciesName(std::move(speciesName)) {}

std::ostream &operator<<(std::ostream &os, const MouseInteractionSpecies &species) {
  os << species.speciesName;
  return os;
}
MouseInteractionSpecies::MouseInteractionSpecies(std::string speciesName) : speciesId(std::nullopt), speciesName(std::move(speciesName)) {
}
bool MouseInteractionSpecies::operator==(const MouseInteractionSpecies &rhs) const {
  return speciesId == rhs.speciesId && speciesName == rhs.speciesName;
}
bool MouseInteractionSpecies::operator!=(const MouseInteractionSpecies &rhs) const {
  return !(rhs == *this);
}

MouseInteractionPanel::MouseInteractionPanel(const std::string &name, ui::ig::Persistent persistent)
    : Element(name),
      ValueObservable(physarum::InteractionConfig{}),
      Savable(persistent),
      layout(name + "layout", LayoutDirection::TopToBottom, Size::Auto(), ShowBorder::Yes) {
  mouseInteractionCombobox = &layout.createChild<Combobox<MouseInteraction>>(name + "mouse_int_type_combobox", "Mouse interaction", "Select", magic_enum::enum_values<MouseInteraction>());
  mouseInteractionCombobox->setSelectedItem(MouseInteraction::None);
  distanceDrag = &layout.createChild<DragInput<float>>(name + "distance_drag", "Effect distance", 1.f, 1.f, 10000.f, 100.f);
  powerDrag = &layout.createChild<DragInput<float>>(name + "power_drag", "Effect power", 0.01f, .1f, 10.f, 10.f);
  drawFalloffCheckbox = &layout.createChild<Checkbox>(name + "draw_falloff_checkbox", "Draw falloff");
  drawFalloffCheckbox->setVisibility(Visibility::Invisible);
  mouseInteractionSpeciesCombobox = &layout.createChild<Combobox<MouseInteractionSpecies>>(name + "mouse_int_species_combobox", "Species", "Select", std::vector<MouseInteractionSpecies>{});

  setInteractableSpecies({});

  const auto onChange = [&](auto) {
    setValue(getConfig());
  };
  mouseInteractionCombobox->addValueListener([this, onChange](const auto type) {
    const auto optionsVisibility = type == MouseInteraction::None ? Visibility::Invisible : Visibility::Visible;
    distanceDrag->setVisibility(optionsVisibility);
    powerDrag->setVisibility(optionsVisibility);
    drawFalloffCheckbox->setVisibility(type == MouseInteraction::Draw ? Visibility::Visible : Visibility::Invisible);
    onChange(type);
  },
                                             true);
  distanceDrag->addValueListener(onChange);
  powerDrag->addValueListener(onChange);
  drawFalloffCheckbox->addValueListener(onChange);
  mouseInteractionSpeciesCombobox->addValueListener(onChange);

  mouseInteractionCombobox->setTooltip("Type of mouse interaction with particles");
  distanceDrag->setTooltip("Distance of the selected effect");
  powerDrag->setTooltip("Power of the selected effect");
  mouseInteractionSpeciesCombobox->setTooltip("Species selected for interaction");
}

void MouseInteractionPanel::renderImpl() {
  layout.render();
}

void MouseInteractionPanel::unserialize_impl(const toml::table &src) {
  setConfig(physarum::InteractionConfig::FromToml(src));
}

toml::table MouseInteractionPanel::serialize_impl() {
  return getValue().toToml();
}

physarum::InteractionConfig MouseInteractionPanel::getConfig() const {
  return {
      .interactionType = mouseInteractionCombobox->getValue(),
      .distance = distanceDrag->getValue(),
      .power = powerDrag->getValue(),
      .interactedSpecies = mouseInteractionSpeciesCombobox->getValue().speciesId.value_or(-1),
      .enableDrawFalloff = drawFalloffCheckbox->getValue(),
  };
}

void MouseInteractionPanel::setConfig(const InteractionConfig &config) {
  mouseInteractionCombobox->setSelectedItem(config.interactionType);
  distanceDrag->setValue(config.distance);
  powerDrag->setValue(config.power);
  drawFalloffCheckbox->setValue(config.enableDrawFalloff);
}
void MouseInteractionPanel::setInteractableSpecies(const std::vector<MouseInteractionSpecies> &species) {
  auto previousSelected = mouseInteractionSpeciesCombobox->getSelectedItem();
  mouseInteractionSpeciesCombobox->removeItemIf([](auto) { return true; });
  auto allSpecies = MouseInteractionSpecies{"All"};
  mouseInteractionSpeciesCombobox->addItem(MouseInteractionSpecies{"All"});
  bool containsPreviousSelected = false;
  for (const auto &s : species) {
    if (previousSelected.has_value() && *previousSelected == s) {
      containsPreviousSelected = true;
    }
    mouseInteractionSpeciesCombobox->addItem(s);
  }
  if (!containsPreviousSelected) {
    previousSelected = std::nullopt;
  }
  mouseInteractionSpeciesCombobox->setSelectedItem(previousSelected.value_or(allSpecies));
}

}// namespace pf