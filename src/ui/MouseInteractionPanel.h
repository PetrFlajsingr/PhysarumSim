//
// Created by xflajs00 on 06.11.2021.
//

#ifndef PHYSARUMSIM_SRC_UI_MOUSEINTERACTIONPANEL_H
#define PHYSARUMSIM_SRC_UI_MOUSEINTERACTIONPANEL_H

#include <ostream>
#include <pf_imgui/elements/Checkbox.h>
#include <pf_imgui/elements/Combobox.h>
#include <pf_imgui/elements/DragInput.h>
#include <pf_imgui/interface/Element.h>
#include <pf_imgui/interface/Savable.h>
#include <pf_imgui/interface/ValueObservable.h>
#include <pf_imgui/layouts/StackedLayout.h>
#include <pf_imgui/layouts/VerticalLayout.h>
#include <simulation/SimConfig.h>

namespace pf {

struct MouseInteractionSpecies {
  MouseInteractionSpecies() = default;
  MouseInteractionSpecies(int speciesId, std::string speciesName);
  explicit MouseInteractionSpecies(std::string speciesName);
  std::optional<int> speciesId;
  std::string speciesName;
  friend std::ostream &operator<<(std::ostream &os, const MouseInteractionSpecies &species);
  bool operator==(const MouseInteractionSpecies &rhs) const;
  bool operator!=(const MouseInteractionSpecies &rhs) const;
};

class MouseInteractionPanel : public ui::ig::Element,
                              public ui::ig::ValueObservable<physarum::InteractionConfig>,
                              public ui::ig::Savable {
 public:
  MouseInteractionPanel(const std::string &name, ui::ig::Persistent persistent);

  [[nodiscard]] physarum::InteractionConfig getConfig() const;
  void setConfig(const physarum::InteractionConfig &config);

  void setInteractableSpecies(const std::vector<MouseInteractionSpecies> &species);

  void setFromToml(const toml::table &src) override;
  [[nodiscard]] toml::table toToml() const override;

 protected:
  void renderImpl() override;

 private:
  // clang-format off
  ui::ig::VerticalLayout layout;
    ui::ig::Combobox<physarum::MouseInteraction> *mouseInteractionCombobox;
    ui::ig::DragInput<float> *distanceDrag;
    ui::ig::DragInput<float> *powerDrag;
    ui::ig::DragInput<int> *particleCountDrag;
    ui::ig::Checkbox *drawFalloffCheckbox;
    ui::ig::Combobox<MouseInteractionSpecies> *mouseInteractionSpeciesCombobox;
  // clang-format on
};

}// namespace pf
#endif//PHYSARUMSIM_SRC_UI_MOUSEINTERACTIONPANEL_H
