//
// Created by xflajs00 on 04.11.2021.
//

#include "SpeciesColorPanel.h"

namespace pf {

using namespace ui::ig;
using namespace physarum;

SpeciesColorPanel::SpeciesColorPanel(const std::string &name,
                                     pf::ui::ig::Persistent persistent)
    : Element(name),
      ValueObservable(physarum::PopulationColor{ColorType::Simple}),
      Savable(persistent),
      layout(name + "layout", LayoutDirection::TopToBottom, Size{Width::Auto(), 140}, ShowBorder::Yes) {

  enableTrailMultiplyCheckbox = &layout.createChild<Checkbox>(getName() + "enable_trail_mult_checkbox", "Enable trail multiplication", true);
  colorTypeCombobox = &layout.createChild<Combobox<ColorType>>(getName() + "combobox_color_type", "Color type", "Select", magic_enum::enum_values<ColorType>());
  colorTypeCombobox->setSelectedItem(ColorType::Simple);
  trailPowDrag = &layout.createChild<DragInput<float>>(getName() + "trail_pow_drag", "Trail render mod", .01f, 0.1f, 10.f, 1.f);

  stack = &layout.createChild<StackedLayout>(name + "stack", Size::Auto());

  auto &simpleStack = stack->pushStack();
  simpleColorEdit = &simpleStack.createChild<ColorEdit<glm::vec3>>(name + "simple_color_edit", "Color", glm::vec3{1.f});

  auto &gradientStack = stack->pushStack();
  gradientStartColorEdit = &gradientStack.createChild<ColorEdit<glm::vec3>>(name + "grad_start_color_edit", "Gradient start", glm::vec3{1.f});
  gradientEndColorEdit = &gradientStack.createChild<ColorEdit<glm::vec3>>(name + "grad_end_color_edit", "Gradient end", glm::vec3{1.f});

  auto &randomStack = stack->pushStack();
  randomizeButton = &randomStack.createChild<Button>(name + "randomize_button", "Randomize");

  auto &rainbowStack = stack->pushStack();
  hueSlider = &rainbowStack.createChild<Slider<float>>(name +"hue_slider", "Starting hue", 0.f, 360.f, 0.f);

  const auto onChange = [this] {
    setValue(getColor());
  };
  colorTypeCombobox->addValueListener([onChange, this](const auto type) {
    switch (type) {
      case ColorType::Simple: stack->setIndex(SIMPLE_INDEX); break;
      case ColorType::TwoColorGradient: stack->setIndex(GRADIENT_INDEX); break;
      case ColorType::Random: stack->setIndex(RANDOM_INDEX); break;
      case ColorType::Rainbow: stack->setIndex(RAINBOW_INDEX); break;
    }
    onChange();
  }, true);
  enableTrailMultiplyCheckbox->addValueListener([onChange](auto) { onChange(); });
  simpleColorEdit->addValueListener([onChange](auto) { onChange(); });
  gradientStartColorEdit->addValueListener([onChange](auto) { onChange(); });
  gradientEndColorEdit->addValueListener([onChange](auto) { onChange(); });
  hueSlider->addValueListener([onChange](auto) { onChange(); });
  trailPowDrag->addValueListener([onChange](auto) { onChange(); });
  randomizeButton->addClickListener([onChange] { onChange(); });
}

void SpeciesColorPanel::renderImpl() {
  layout.render();
}

void SpeciesColorPanel::unserialize_impl(const toml::table &src) {
  auto config = PopulationColor::FromToml(src);
  setColor(config);
}

toml::table SpeciesColorPanel::serialize_impl() {
  return getValue().toToml();
}

PopulationColor SpeciesColorPanel::getColor() const {
  PopulationColor result{};
  result.setType(colorTypeCombobox->getValue());
  result.setSimpleColor(simpleColorEdit->getValue());
  result.setGradientStart(gradientStartColorEdit->getValue());
  result.setGradientEnd(gradientEndColorEdit->getValue());
  result.setEnableTrailMult(enableTrailMultiplyCheckbox->getValue());
  result.setStartHue(hueSlider->getValue());
  result.setTrailPow(trailPowDrag->getValue());
  return result;
}

void SpeciesColorPanel::setColor(const physarum::PopulationColor &color) {
  colorTypeCombobox->setSelectedItem(color.getType());
  simpleColorEdit->setValue(color.getSimpleColor());
  gradientStartColorEdit->setValue(color.getGradientStart());
  gradientEndColorEdit->setValue(color.getGradientEnd());
  enableTrailMultiplyCheckbox->setValue(color.isEnableTrailMult());
  hueSlider->setValue(color.getStartHue());
  trailPowDrag->setValue(color.getTrailPow());
  setValue(color);
}

}// namespace pf
