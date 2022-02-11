//
// Created by xflajs00 on 06.11.2021.
//

#include "SpeciesInteractionListbox.h"
#include <memory>
#include <pf_imgui/interface/decorators/WidthDecorator.h>

namespace pf {
using namespace ui::ig;
using namespace physarum;

SpeciesInteractionRow::SpeciesInteractionRow(const std::string &name, const SpeciesInteractionConfig &config,
                                             Persistent persistent)
    : Element(name), Savable(persistent), ValueObservable(config),
      layout({.name = name + "layout", .layoutDirection = LayoutDirection::LeftToRight, .size = {Width::Auto(), 19}}) {

  otherSpeciesText = &layout.createChild(WidthDecorator<InputText>::Config{
      .width = 70,
      .config{.name = name + "species_name_test", .label = "", .value = config.speciesName}});
  otherSpeciesText->setReadOnly(true);
  otherSpeciesText->setTooltip("Other species");

  interactionCombobox = &layout.createChild(WidthDecorator<Combobox<SpeciesInteraction>>::Config{
      .width = 70,
      .config{.name = name + "interaction_combobox", .label = "", .preview = "Select"}});
  interactionCombobox->setItems(magic_enum::enum_values<SpeciesInteraction>());
  interactionCombobox->setSelectedItem(config.interactionType);
  interactionCombobox->setTooltip("Type of interaction");

  factorDrag = &layout.createChild(WidthDecorator<DragInput<float>>::Config{
      .width = 70,
      .config{.name = name + "factor_drag", .label = "Factor", .speed = .01f, .min = .01f, .max = 10.f, .value = 1.f}});
  factorDrag->setValue(config.factor);
  factorDrag->setTooltip("Weight of the species's trail");

  const auto onChange = [&](auto) { setValue(getConfig()); };
  factorDrag->addValueListener(onChange);
  interactionCombobox->addValueListener(
      [this, onChange](const auto type) {
        factorDrag->setVisibility(type == SpeciesInteraction::None ? Visibility::Invisible : Visibility::Visible);
        onChange(type);
      },
      true);
}

SpeciesInteractionConfig SpeciesInteractionRow::getConfig() const {
  return {.interactionType = interactionCombobox->getValue(),
          .factor = factorDrag->getValue(),
          .speciesName = std::string{otherSpeciesText->getValue()}};
}

void SpeciesInteractionRow::setConfig(const SpeciesInteractionConfig &config) {
  interactionCombobox->setValue(config.interactionType);
  factorDrag->setValue(config.factor);
  otherSpeciesText->setValue(config.speciesName);
}

void SpeciesInteractionRow::renderImpl() {
  layout.render();
  ImGui::Separator();
}

void SpeciesInteractionRow::unserialize_impl(const toml::table &src) {
  setConfig(SpeciesInteractionConfig::FromToml(src));
}

toml::table SpeciesInteractionRow::serialize_impl() const { return getConfig().toToml(); }

std::unique_ptr<SpeciesInteractionRow> SpeciesInteractionRowFactory::operator()(const SpeciesInteractionConfig &item) {
  return std::make_unique<SpeciesInteractionRow>(idStart + std::to_string(IdCounter++), item);
}

SpeciesInteractionListbox::SpeciesInteractionListbox(const std::string &elementName, const std::string &label,
                                                     const Size &s)
    : CustomListbox<physarum::SpeciesInteractionConfig, SpeciesInteractionRow>(elementName, label,
                                                                               SpeciesInteractionRowFactory{}, s) {}

SpeciesInteractionRow &SpeciesInteractionListbox::addItem(const SpeciesInteractionConfig &item) {
  auto &result = ui::ig::CustomListbox<physarum::SpeciesInteractionConfig, SpeciesInteractionRow>::addItem(item);
  const auto itemIndex = items.size() - 1;
  result.addValueListener(
      [this, itemIndex](const physarum::SpeciesInteractionConfig &item) { items[itemIndex].first = item; });
  return result;
}
}// namespace pf