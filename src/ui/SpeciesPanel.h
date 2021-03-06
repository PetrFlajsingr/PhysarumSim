//
// Created by xflajs00 on 03.11.2021.
//

#ifndef PHYSARUMSIM_SRC_UI_SPECIESPANEL_H
#define PHYSARUMSIM_SRC_UI_SPECIESPANEL_H

#include "SpeciesColorPanel.h"
#include "SpeciesInteractionListbox.h"
#include <pf_imgui/elements/ColorChooser.h>
#include <pf_imgui/elements/Combobox.h>
#include <pf_imgui/elements/DragInput.h>
#include <pf_imgui/elements/Input.h>
#include <pf_imgui/elements/InputText.h>
#include <pf_imgui/interface/Element.h>
#include <pf_imgui/interface/Savable.h>
#include <pf_imgui/interface/ValueObservable.h>
#include <pf_imgui/layouts/VerticalLayout.h>
#include <simulation/SimConfig.h>

namespace pf {

class SpeciesPanel : public ui::ig::Element,
                     public ui::ig::ValueObservable<physarum::PopulationConfig>,
                     public ui::ig::Savable {
 public:
  SpeciesPanel(const std::string &name, ui::ig::Persistent persistent = ui::ig::Persistent::No);

  [[nodiscard]] physarum::PopulationConfig getConfig() const;
  void setConfig(const physarum::PopulationConfig &config);

  void addInteraction(const physarum::SpeciesInteractionConfig &interConfig);
  void clearInteractions();

  void setFromToml(const toml::table &src) override;
  [[nodiscard]] toml::table toToml() const override;

 protected:

  void renderImpl() override;

 private:
  void createElements();
  void registerListeners();
  void createTooltips();

  // clang-format off
  ui::ig::VerticalLayout root;
    ui::ig::DragInput<float> *senseAngleDrag;
    ui::ig::DragInput<float> *senseDistanceDrag;
    ui::ig::DragInput<float> *turnSpeedDrag;
    ui::ig::DragInput<float> *movementSpeedDrag;
    ui::ig::DragInput<float> *maxSteerRandomnessDrag;
    ui::ig::DragInput<float> *trailWeightDrag;
    ui::ig::Combobox<physarum::FilterType> *filterTypeCombobox;
    ui::ig::Combobox<int> *kernelSizeCombobox;
    ui::ig::DragInput<float> *diffuseRateDrag;
    ui::ig::DragInput<float> *decayRateDrag;
    ui::ig::DragInput<float> *maxTrailValueDrag;
    ui::ig::Combobox<physarum::ParticleStart> *particleInitCombobox;
    ui::ig::Input<int> *particleCountInput;
    ui::ig::Combobox<int> *sensorSizeCombobox;
    SpeciesColorPanel *colorPanel;
    SpeciesInteractionListbox *interactionsListbox;
  // clang-format on
};

}// namespace pf

#endif//PHYSARUMSIM_SRC_UI_SPECIESPANEL_H
