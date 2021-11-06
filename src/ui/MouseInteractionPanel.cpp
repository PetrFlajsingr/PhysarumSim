//
// Created by xflajs00 on 06.11.2021.
//

#include "MouseInteractionPanel.h"

namespace pf {

using namespace ui::ig;
using namespace physarum;

MouseInteractionPanel::MouseInteractionPanel(const std::string &name, ui::ig::Persistent persistent)
    : Element(name),
      ValueObservable(physarum::InteractionConfig{}),
      Savable(persistent),
      layout(name + "layout", LayoutDirection::TopToBottom, Size::Auto(), ShowBorder::Yes) {
  mouseInteractionCombobox = &layout.createChild<Combobox<MouseInteraction>>(name + "mouse_int_type_combobox", "Mouse interaction", "Select", magic_enum::enum_values<MouseInteraction>());
  mouseInteractionCombobox->setSelectedItem(MouseInteraction::None);
  distanceDrag = &layout.createChild<DragInput<float>>(name + "distance_drag", "Effect distance", 1.f, 1.f, 10000.f, 100.f);
  powerDrag = &layout.createChild<DragInput<float>>(name + "power_drag", "Effect power", 0.01f, .1f, 10.f, 10.f);
  const auto onChange = [&](auto) {
    setValue(getConfig());
  };
  mouseInteractionCombobox->addValueListener([this, onChange](const auto type) {
    const auto optionsVisibility = type == MouseInteraction::None ? Visibility::Invisible : Visibility::Visible;
    distanceDrag->setVisibility(optionsVisibility);
    powerDrag->setVisibility(optionsVisibility);
    onChange(type);
  },
                                             true);
  distanceDrag->addValueListener(onChange);
  powerDrag->addValueListener(onChange);

  mouseInteractionCombobox->setTooltip("Type of mouse interaction with particles");
  distanceDrag->setTooltip("Distance of the selected effect");
  powerDrag->setTooltip("Power of the selected effect");
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
      .power = powerDrag->getValue()};
}

void MouseInteractionPanel::setConfig(const InteractionConfig &config) {
  mouseInteractionCombobox->setSelectedItem(config.interactionType);
  distanceDrag->setValue(config.distance);
  powerDrag->setValue(config.power);
}

}// namespace pf