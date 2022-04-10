//
// Created by xflajs00 on 12.11.2021.
//

#include "HelpPanel.h"
#include <range/v3/view/join.hpp>

namespace pf {
using namespace ui::ig;

HelpPanel::SearchResult::SearchResult(HelpPanel::EmptyTag) : result(EmptyTag{}), index(0) {}

HelpPanel::SearchResult::SearchResult(HelpPanel::TreeNode *node, std::size_t idx) : result(node), index(idx) {}

HelpPanel::SearchResult::SearchResult(ui::ig::TreeLeaf *leaf, std::size_t idx) : result(leaf), index(idx) {}

HelpPanel::HelpPanel(const std::string &name, const ui::ig::Size &size, std::unique_ptr<HelpLoader> loader,
                     ImGuiInterface &imguiInterface)
    : ui::ig::Element(name), layout({.name = name + "_layout",
                                     .layoutDirection = LayoutDirection::LeftToRight,
                                     .size = Size::Auto(),
                                     .showBorder = ShowBorder::Yes}),
      menuLayout(layout.createChild<BoxLayout>(name + "_menu_layout", LayoutDirection::TopToBottom,
                                               Size{200, Height::Auto()})),
      searchInput(menuLayout.createChild<InputText>(name + "_filter_input", "Filter")),
      menuTree(menuLayout.createChild<Tree<TreeType::Simple>>(name + "_menu_tree", ShowBorder::Yes)),
      helpStack(layout.createChild(
          StackedLayout::Config{.name = name + "_stack", .size = Size::Auto(), .showBorder = ShowBorder::Yes})),
      loader(std::move(loader)) {

  std::ranges::for_each(this->loader->getHelpData(), [this, &imguiInterface](const HelpData &helpData) {
    auto &stack = addItem(helpData.section);
    auto &itemLayout = stack.createChild<BoxLayout>(uniqueId(), LayoutDirection::TopToBottom, Size::Auto());
    itemLayout.setScrollable(true);
    auto &markdownUI = itemLayout.createChild<MarkdownText>(uniqueId(), imguiInterface, helpData.markdown, 12.f,
                                                            this->loader->getImageLoader());
    markdownUI.setOnLinkClicked(
        [](std::string_view link, bool) { ImGui::SetClipboardText(std::string{link}.c_str()); });
  });
  menuTree.setLimitSelectionToOne(true);

  searchInput.addValueListener([this](auto filterStr) {
    std::string lowerCaseFilter;
    std::ranges::transform(filterStr, std::back_inserter(lowerCaseFilter), ::tolower);
    const auto strContains = [](std::string_view haystack, std::string_view needle) {
      std::string lowerCaseHaystack;
      std::ranges::transform(haystack, std::back_inserter(lowerCaseHaystack), ::tolower);
      return lowerCaseHaystack.find(needle) != std::string::npos;
    };
    auto visitor =
        Visitor{[&](TreeLeaf *leaf, std::size_t depth) {
                  leaf->clearColors();
                  if (filterStr.empty()) { return; }
                  if (strContains(leaf->getLabel(), lowerCaseFilter)) {
                    leaf->setColor<style::ColorOf::Text>(Color::RGB(0, 255, 0));
                  }
                },
                [&](TreeNode *node, std::size_t depth) {
                  node->clearColors();
                  if (filterStr.empty()) { return; }
                  if (strContains(node->getLabel(), lowerCaseFilter)) {
                    node->setColor<style::ColorOf::Text>(Color::RGB(0, 255, 0));
                  }
                  const auto shouldOpen = std::ranges::any_of(node->getTreeNodes(), [](const auto &node) {
                    if (auto nodePtr = dynamic_cast<const TreeNode *>(&node); nodePtr != nullptr) {
                      return nodePtr->getColor<style::ColorOf::Text>().has_value() || !nodePtr->isCollapsed();
                    } else if (auto leafPtr = dynamic_cast<const TreeLeaf *>(&node); leafPtr != nullptr) {
                      return leafPtr->getColor<style::ColorOf::Text>().has_value();
                    }
                  });
                  node->setCollapsed(!shouldOpen);
                }};
    menuTree.traversePostOrder(visitor);
  });
}

ui::ig::StackedLayout::Stack &HelpPanel::addItem(const std::vector<std::string> &section) {
  using namespace ui::ig;
  if (section.empty()) { throw std::runtime_error("Section empty in TreeMenuPanel"); }
  const auto res = searchItem(section);
  if (std::holds_alternative<TreeLeaf *>(res.result)) {
    throw std::runtime_error{"Item already present in TreeMenuPanel"};
  }
  return addItemStart(res, section);
}

void HelpPanel::selectItem(const std::vector<std::string> &section) {
  using namespace ui::ig;
  auto visitor = Visitor{[&](TreeLeaf *leaf, std::size_t depth) {
                           if (leaf->getLabel() == section[depth]) { leaf->setValue(true); }
                           return true;
                         },
                         [&](TreeNode *node, std::size_t depth) {
                           const auto isPartOfPath = node->getLabel() == section[depth];
                           node->setCollapsed(!isPartOfPath);
                           return isPartOfPath;
                         }};
  menuTree.traversePreOrder(visitor);
}

void HelpPanel::setMenuWidth(float width) { menuLayout.setSize(Size{width, menuLayout.getSize().height}); }

void HelpPanel::renderImpl() { layout.render(); }

std::string HelpPanel::getNextChildName() { return fmt::format("{}_child_{}", getName(), idCounter++); }

ui::ig::StackedLayout::Stack &HelpPanel::addItemStart(const HelpPanel::SearchResult &res,
                                                      std::span<const std::string> section) {
  if (std::holds_alternative<EmptyTag>(res.result)) {
    if (section.size() == 1) {
      auto &leaf = menuTree.addLeaf(getNextChildName(), section[0]);
      auto &stack = helpStack.pushStack();
      leaf.addValueListener(createOpenForStack(stack));
      return stack;
    } else {
      auto &node = menuTree.addHeaderNode(getNextChildName(), section[0]);
      return addItemImpl(node, section, 1);
    }
  } else if (res.index == section.size() - 1) {
    throw std::runtime_error{"Item already present as a node, while leaf is requested"};
  }
  return addItemImpl(*std::get<TreeNode *>(res.result), section, res.index + 1);
}

ui::ig::StackedLayout::Stack &HelpPanel::addItemImpl(TreeNode &parent, std::span<const std::string> section,
                                                     std::size_t index) {
  if (index == section.size() - 1) {
    auto &leaf = parent.addLeaf(getNextChildName(), section[index]);
    auto &stack = helpStack.pushStack();
    leaf.addValueListener(createOpenForStack(stack));
    return stack;
  } else {
    auto &node = parent.addNode(getNextChildName(), section[index]);
    return addItemImpl(node, section, index + 1);
  }
}

HelpPanel::SearchResult HelpPanel::searchItem(std::span<const std::string> section) {
  using namespace ui::ig;
  SearchResult result;
  auto visitor = Visitor{[&](TreeLeaf *leaf, std::size_t depth) {
                           if (leaf->getLabel() == section[depth]) {
                             result.index = depth;
                             result.result = leaf;
                           }
                           return true;
                         },
                         [&](TreeNode *node, std::size_t depth) {
                           if (node->getLabel() != section[depth]) { return false; }
                           result.index = depth;
                           result.result = node;
                           return true;
                         }};
  menuTree.traversePreOrder(visitor);
  return result;
}

}// namespace pf