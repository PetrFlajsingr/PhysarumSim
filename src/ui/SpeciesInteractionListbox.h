//
// Created by xflajs00 on 06.11.2021.
//

#ifndef PHYSARUMSIM_SRC_UI_SPECIESINTERACTIONLISTBOX_H
#define PHYSARUMSIM_SRC_UI_SPECIESINTERACTIONLISTBOX_H

#include <pf_imgui/elements/Combobox.h>
#include <pf_imgui/elements/CustomListbox.h>
#include <pf_imgui/elements/DragInput.h>
#include <pf_imgui/elements/InputText.h>
#include <pf_imgui/elements/Text.h>
#include <pf_imgui/interface/Element.h>
#include <pf_imgui/interface/Savable.h>
#include <pf_imgui/interface/ValueObservable.h>
#include <pf_imgui/layouts/BoxLayout.h>
#include <simulation/SimConfig.h>

namespace pf {
class SpeciesInteractionRow : public ui::ig::Element,
                              public ui::ig::Savable,
                              public ui::ig::ValueObservable<physarum::SpeciesInteractionConfig> {
 public:
  SpeciesInteractionRow(const std::string &name, const physarum::SpeciesInteractionConfig &config,
                        ui::ig::Persistent persistent = ui::ig::Persistent::No);

  [[nodiscard]] physarum::SpeciesInteractionConfig getConfig() const;
  void setConfig(const physarum::SpeciesInteractionConfig &config);

 protected:
  void renderImpl() override;

  void unserialize_impl(const toml::table &src) override;
  toml::table serialize_impl() override;

 private:
  // clang-format off
  ui::ig::BoxLayout layout;
    ui::ig::InputText *otherSpeciesText;
    ui::ig::Combobox<physarum::SpeciesInteraction> *interactionCombobox;
    ui::ig::DragInput<float> *factorDrag;
  // clang-format on
};

struct SpeciesInteractionRowFactory {
  static inline cppcoro::generator<std::size_t> idGenerator = iota<std::size_t>();
  const std::string idStart = ui::ig::uniqueId();
  std::unique_ptr<SpeciesInteractionRow> operator()(const physarum::SpeciesInteractionConfig &item);
};
static_assert(ui::ig::CustomItemBoxFactory<SpeciesInteractionRowFactory, physarum::SpeciesInteractionConfig,
                                           SpeciesInteractionRow>);

class SpeciesInteractionListbox
    : public ui::ig::CustomListbox<physarum::SpeciesInteractionConfig, SpeciesInteractionRow> {
 public:
  SpeciesInteractionListbox(const std::string &elementName, const std::string &label, const ui::ig::Size &s);
};
}// namespace pf

#endif//PHYSARUMSIM_SRC_UI_SPECIESINTERACTIONLISTBOX_H
