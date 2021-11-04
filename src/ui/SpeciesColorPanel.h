//
// Created by xflajs00 on 04.11.2021.
//

#ifndef PHYSARUMSIM_SRC_UI_SPECIESCOLORPANEL_H
#define PHYSARUMSIM_SRC_UI_SPECIESCOLORPANEL_H

#include <pf_imgui/elements/Button.h>
#include <pf_imgui/elements/Checkbox.h>
#include <pf_imgui/elements/ColorChooser.h>
#include <pf_imgui/elements/Combobox.h>
#include <pf_imgui/elements/DragInput.h>
#include <pf_imgui/elements/Slider.h>
#include <pf_imgui/interface/Element.h>
#include <pf_imgui/interface/Savable.h>
#include <pf_imgui/interface/ValueObservable.h>
#include <pf_imgui/layouts/BoxLayout.h>
#include <pf_imgui/layouts/StackedLayout.h>
#include <simulation/SimConfig.h>

namespace pf {

class SpeciesColorPanel : public ui::ig::Element,
                          public ui::ig::ValueObservable<physarum::PopulationColor>,
                          public ui::ig::Savable {
 public:
  SpeciesColorPanel(const std::string &name, ui::ig::Persistent persistent = ui::ig::Persistent::No);

  [[nodiscard]] physarum::PopulationColor getColor() const;
  void setColor(const physarum::PopulationColor &color);

 protected:
  void renderImpl() override;
  void unserialize_impl(const toml::table &src) override;
  toml::table serialize_impl() override;

 private:
  constexpr static auto SIMPLE_INDEX = 0;
  constexpr static auto GRADIENT_INDEX = 1;
  constexpr static auto RANDOM_INDEX = 2;
  constexpr static auto RAINBOW_INDEX = 3;

  ui::ig::BoxLayout layout;

  ui::ig::Checkbox *enableTrailMultiplyCheckbox;
  ui::ig::Combobox<physarum::ColorType> *colorTypeCombobox;
  ui::ig::DragInput<float> *trailPowDrag;

  ui::ig::StackedLayout *stack;

  ui::ig::ColorEdit<glm::vec3> *simpleColorEdit;

  // TODO: slider for gradient
  ui::ig::ColorEdit<glm::vec3> *gradientStartColorEdit;
  ui::ig::ColorEdit<glm::vec3> *gradientEndColorEdit;

  ui::ig::Button *randomizeButton;

  ui::ig::Slider<float> *hueSlider;
};

}
#endif//PHYSARUMSIM_SRC_UI_SPECIESCOLORPANEL_H