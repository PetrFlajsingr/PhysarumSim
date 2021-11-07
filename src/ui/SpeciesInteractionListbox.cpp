//
// Created by xflajs00 on 06.11.2021.
//

#include "SpeciesInteractionListbox.h"
#include <memory>
#include <pf_imgui/interface/decorators/WidthDecorator.h>

namespace pf {
using namespace ui::ig;
using namespace physarum;

SpeciesInteractionRow::SpeciesInteractionRow(const std::string &name, const SpeciesInteractionConfig &config, Persistent persistent) : Element(name), Savable(persistent), ValueObservable(config),
                                                                                                                                       layout(name + "layout", LayoutDirection::LeftToRight, Size{Width::Auto(), 19}, AllowCollapse::No, Persistent::No) {
  otherSpeciesText = &layout.createChild<WidthDecorator<InputText>>(name + "species_name_test", 70, "", config.speciesName);
  otherSpeciesText->setReadOnly(true);
  interactionCombobox = &layout.createChild<WidthDecorator<Combobox<SpeciesInteraction>>>(name + "interaction_combobox", 70, "", "Select", magic_enum::enum_values<SpeciesInteraction>());
  interactionCombobox->setSelectedItem(config.interactionType);
  factorDrag = &layout.createChild<WidthDecorator<DragInput<float>>>(name + "factor_drag", 70, "Factor", .01f, .01f, 10.f, 1.f);
  factorDrag->setValue(config.factor);

  const auto onChange = [&](auto) {
    setValue(getConfig());
  };
  factorDrag->addValueListener(onChange);
  interactionCombobox->addValueListener([this, onChange](const auto type) {
    factorDrag->setVisibility(type == SpeciesInteraction::None ? Visibility::Invisible : Visibility::Visible);
    onChange(type);
  },
                                        true);
}

SpeciesInteractionConfig SpeciesInteractionRow::getConfig() const {
  return {
      .interactionType = interactionCombobox->getValue(),
      .factor = factorDrag->getValue(),
      .speciesName = otherSpeciesText->getText()};
}

void SpeciesInteractionRow::setConfig(const SpeciesInteractionConfig &config) {
  interactionCombobox->setValue(config.interactionType);
  factorDrag->setValue(config.factor);
  otherSpeciesText->setText(config.speciesName);
}

void SpeciesInteractionRow::renderImpl() {
  layout.render();
  ImGui::Separator();
}

void SpeciesInteractionRow::unserialize_impl(const toml::table &src) {
  setConfig(SpeciesInteractionConfig::FromToml(src));
}

toml::table SpeciesInteractionRow::serialize_impl() {
  return getConfig().toToml();
}

std::unique_ptr<SpeciesInteractionRow> SpeciesInteractionRowFactory::operator()(const SpeciesInteractionConfig &item) {
  return std::make_unique<SpeciesInteractionRow>(idStart + std::to_string(getNext(idGenerator)), item);
}

SpeciesInteractionListbox::SpeciesInteractionListbox(const std::string &elementName, const std::string &label, const Size &s)
    : CustomListbox<physarum::SpeciesInteractionConfig, SpeciesInteractionRow>(elementName, label, SpeciesInteractionRowFactory{}, s) {}
}// namespace pf