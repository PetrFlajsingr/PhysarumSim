//
// Created by xflajs00 on 04.11.2021.
//

#include "SpeciesColorPanel.h"

namespace pf {

using namespace ui::ig;
using namespace physarum;

SpeciesColorPanel::SpeciesColorPanel(const std::string &name, pf::ui::ig::Persistent persistent)
    : Element(name), ValueObservable(physarum::PopulationColor{ColorType::Simple}), Savable(persistent),
      layout({.name = name + "layout", .size = Size{Width::Auto(), 140}, .showBorder = true}) {
  createChildren();
  registerListeners();
  addTooltips();
}

void SpeciesColorPanel::renderImpl() { layout.render(); }

void SpeciesColorPanel::setFromToml(const toml::table &src) {
  auto config = PopulationColor::FromToml(src);
  setColor(config);
}

toml::table SpeciesColorPanel::toToml() const { return getValue().toToml(); }

PopulationColor SpeciesColorPanel::getColor() const {
  const auto colorToVec3 = [](Color color) { return glm::vec3{color.red(), color.green(), color.blue()}; };
  PopulationColor result{};
  result.setType(colorTypeCombobox->getValue());
  result.setSimpleColor(colorToVec3(simpleColorEdit->getValue()));
  result.setGradientStart(colorToVec3(gradientStartColorEdit->getValue()));
  result.setGradientEnd(colorToVec3(gradientEndColorEdit->getValue()));
  result.setStartHue(hueSlider->getValue());
  result.setTrailPow(trailPowDrag->getValue());
  return result;
}

void SpeciesColorPanel::setColor(const physarum::PopulationColor &color) {
  const auto vec3ToColor = [](glm::vec3 vec3) { return Color::RGB(vec3.r, vec3.g, vec3.b); };
  colorTypeCombobox->setSelectedItem(color.getType());
  simpleColorEdit->setValue(vec3ToColor(color.getSimpleColor()));
  gradientStartColorEdit->setValue(vec3ToColor(color.getGradientStart()));
  gradientEndColorEdit->setValue(vec3ToColor(color.getGradientEnd()));
  hueSlider->setValue(color.getStartHue());
  trailPowDrag->setValue(color.getTrailPow());
  setValue(color);
}

void SpeciesColorPanel::createChildren() {
  colorTypeCombobox = &layout.createChild<Combobox<ColorType>>(getName() + "combobox_color_type", "Color type",
                                                               "Select", magic_enum::enum_values<ColorType>());
  colorTypeCombobox->setSelectedItem(ColorType::Simple);
  trailPowDrag =
      &layout.createChild<DragInput<float>>(getName() + "trail_pow_drag", "Trail render mod", .01f, 0.1f, 10.f, 1.f);

  stack = &layout.createChild<StackedLayout>(getName() + "stack", Size::Auto());

  auto &simpleStack = stack->pushStack();
  simpleColorEdit = &simpleStack.createChild<ColorEdit<ui::ig::ColorChooserFormat::RGB>>(
      getName() + "simple_color_edit", "Color", Color::White);

  auto &gradientStack = stack->pushStack();
  gradientStartColorEdit = &gradientStack.createChild<ColorEdit<ui::ig::ColorChooserFormat::RGB>>(
      getName() + "grad_start_color_edit", "Gradient start", Color::White);
  gradientEndColorEdit = &gradientStack.createChild<ColorEdit<ui::ig::ColorChooserFormat::RGB>>(
      getName() + "grad_end_color_edit", "Gradient end", Color::White);
  flipGradientButton = &gradientStack.createChild<Button>(getName() + "flip_gradient_button", "Flip");

  auto &randomStack = stack->pushStack();
  randomizeButton = &randomStack.createChild<Button>(getName() + "randomize_button", "Randomize");

  auto &rainbowStack = stack->pushStack();
  hueSlider = &rainbowStack.createChild<Slider<float>>(getName() + "hue_slider", "Starting hue", 0.f, 360.f, 0.f);
}

void SpeciesColorPanel::registerListeners() {
  flipGradientButton->addClickListener([&] {
    const auto col1 = gradientStartColorEdit->getValue();
    gradientStartColorEdit->setValue(gradientEndColorEdit->getValue());
    gradientEndColorEdit->setValue(col1);
  });
  const auto onChange = [this] { setValue(getColor()); };
  colorTypeCombobox->addValueListener(
      [onChange, this](const auto type) {
        switch (type) {
          case ColorType::Simple: stack->setIndex(SIMPLE_INDEX); break;
          case ColorType::TwoColorGradient: stack->setIndex(GRADIENT_INDEX); break;
          case ColorType::Random: stack->setIndex(RANDOM_INDEX); break;
          case ColorType::Rainbow: stack->setIndex(RAINBOW_INDEX); break;
        }
        onChange();
      },
      true);
  simpleColorEdit->addValueListener([onChange](auto) { onChange(); });
  gradientStartColorEdit->addValueListener([onChange](auto) { onChange(); });
  gradientEndColorEdit->addValueListener([onChange](auto) { onChange(); });
  hueSlider->addValueListener([onChange](auto) { onChange(); });
  trailPowDrag->addValueListener([onChange](auto) { onChange(); });
  randomizeButton->addClickListener([onChange] { onChange(); });
}

void SpeciesColorPanel::addTooltips() {
  colorTypeCombobox->setTooltip("Type of trail color mapping");
  trailPowDrag->setTooltip("Trail color mapping correction coefficient - linear to log");
  simpleColorEdit->setTooltip("Color of the trail");
  gradientStartColorEdit->setTooltip("Starting gradient color of the trail - lower trail values");
  gradientEndColorEdit->setTooltip("Ending gradient color of the trail - higher trail values");
  flipGradientButton->setTooltip("Switch gradient start and end");
  randomizeButton->setTooltip("Re-generate colors");
  hueSlider->setTooltip("Starting hue value");
}

}// namespace pf
