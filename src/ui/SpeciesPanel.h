//
// Created by xflajs00 on 03.11.2021.
//

#ifndef PHYSARUMSIM_SRC_UI_SPECIESPANEL_H
#define PHYSARUMSIM_SRC_UI_SPECIESPANEL_H

#include <pf_imgui/elements/ColorChooser.h>
#include <pf_imgui/elements/Combobox.h>
#include <pf_imgui/elements/DragInput.h>
#include <pf_imgui/elements/Input.h>
#include <pf_imgui/elements/InputText.h>
#include <pf_imgui/interface/Element.h>
#include <pf_imgui/interface/Savable.h>
#include <pf_imgui/interface/ValueObservable.h>
#include <pf_imgui/layouts/BoxLayout.h>
#include <simulation/SimConfig.h>
#include "SpeciesColorPanel.h"

namespace pf {

class SpeciesPanel : public ui::ig::Element,
                     public ui::ig::ValueObservable<physarum::PopulationConfig>,
                     public ui::ig::Savable {
 public:
  SpeciesPanel(const std::string &name, ui::ig::Persistent persistent);

  [[nodiscard]] physarum::PopulationConfig getConfig() const;
  void setConfig(const physarum::PopulationConfig &config);

  void addNameListener(std::invocable<std::string_view> auto &&listener) {
    nameInputText->addValueListener(std::forward<decltype(listener)>(listener));
  }

 protected:
  void unserialize_impl(const toml::table &src) override;
  toml::table serialize_impl() override;

 protected:
  void renderImpl() override;

 private:
  void createElements();

  void registerListeners();

  ui::ig::BoxLayout root;
  ui::ig::InputText *nameInputText;
  ui::ig::DragInput<float> *senseAngleDrag;
  ui::ig::DragInput<float> *senseDistanceDrag;
  ui::ig::DragInput<float> *turnSpeedDrag;
  ui::ig::DragInput<float> *movementSpeedDrag;
  ui::ig::DragInput<float> *trailWeightDrag;
  ui::ig::Combobox<int> *kernelSizeCombobox;
  ui::ig::DragInput<float> *diffuseRateDrag;
  ui::ig::DragInput<float> *decayRateDrag;
  ui::ig::DragInput<float> *maxTrailValueDrag;
  ui::ig::Combobox<physarum::ParticleStart> *particleInitCombobox;
  ui::ig::Input<int> *particleCountInput;
  ui::ig::Combobox<int> *sensorSizeCombobox;
  SpeciesColorPanel *colorPanel;

};

}// namespace pf

#endif//PHYSARUMSIM_SRC_UI_SPECIESPANEL_H
