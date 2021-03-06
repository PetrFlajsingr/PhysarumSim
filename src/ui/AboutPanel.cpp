//
// Created by xflajs00 on 11.11.2021.
//

#include "AboutPanel.h"
#include <pf_imgui/elements/MarkdownText.h>

namespace pf {
using namespace ui::ig;

AboutPanel::AboutPanel(const std::string &name, const ui::ig::Size &size, std::unique_ptr<AboutDataLoader> dataLoader,
                       ui::ig::ImGuiInterface &imguiInterface)
    : ui::ig::Element(name), layout({.name = name + "_layout", .size = size, .showBorder = true}),
      listLayout(layout.createChild<ui::ig::VerticalLayout>(name + "_list_layout", Size{200, Height::Auto()})),
      libList(listLayout.createChild<Listbox<std::string>>(name + "_list_tree", "", Size::Fill())),
      textStack(
          layout.createChild(StackedLayout::Config{.name = name + "_stack", .size = Size::Auto(), .showBorder = true})),
      loader(std::move(dataLoader)) {
  auto aboutData = loader->getAboutData();
  if (auto iter = std::ranges::find(aboutData, "PhysarumSim", &AboutData::label); iter != aboutData.end()) {
    auto &stack = textStack.pushStack();
    auto &itemLayout = stack.createChild<VerticalLayout>(uniqueId(), Size::Auto());
    itemLayout.setScrollable(true);
    auto &markdownUI = itemLayout.createChild<MarkdownText>(uniqueId(), imguiInterface, iter->markdown, 12.f);
    auto &selectable = libList.addItem(iter->label);
    selectable.setColor<style::ColorOf::Text>(Color::RGB(0, 255, 0));
    selectable.addValueListener([&](const auto selected) {
      if (selected) { stack.setActive(); }
    });
    selectable.setValue(true);
    aboutData.erase(iter);
  }

  std::ranges::for_each(loader->getAboutData(), [this, &imguiInterface](const auto &aboutData) {
    auto &stack = textStack.pushStack();
    auto &itemLayout = stack.createChild<VerticalLayout>(uniqueId(), Size::Auto());
    itemLayout.setScrollable(true);
    auto &markdownUI = itemLayout.createChild<MarkdownText>(uniqueId(), imguiInterface, aboutData.markdown, 12.f);
    libList.addItem(aboutData.label).addValueListener([&](const auto selected) {
      if (selected) { stack.setActive(); }
    });
  });
}

void AboutPanel::renderImpl() { layout.render(); }

}// namespace pf