
#ifndef PHYSARUMSIM_HELPPANEL_H
#define PHYSARUMSIM_HELPPANEL_H

#include <filesystem>
#include <geGL/Texture.h>
#include <pf_imgui/elements/InputText.h>
#include <pf_imgui/elements/MarkdownText.h>
#include <pf_imgui/elements/Tree.h>
#include <pf_imgui/interface/Element.h>
#include <pf_imgui/layouts/StackedLayout.h>
#include <variant>
#include "help_data/HelpLoader.h"

namespace pf {

class HelpPanel : public ui::ig::Element {
  using TreeNode = ui::ig::TreeNode<ui::ig::TreeType::Simple>;
  struct EmptyTag {};
  struct SearchResult {
    SearchResult(EmptyTag t = EmptyTag{});
    SearchResult(TreeNode *node, std::size_t idx);
    SearchResult(ui::ig::TreeLeaf *leaf, std::size_t idx);

    std::variant<TreeNode *, ui::ig::TreeLeaf *, EmptyTag> result;
    std::size_t index;
  };

 public:
  explicit HelpPanel(const std::string &name, const ui::ig::Size &size, std::unique_ptr<HelpLoader> loader,
                     ui::ig::ImGuiInterface &imguiInterface);

  ui::ig::StackedLayout::Stack &addItem(const std::vector<std::string> &section);

  void selectItem(const std::vector<std::string> &section);

  void setMenuWidth(float width);

 protected:
  void renderImpl() override;

 private:
  inline static auto createOpenForStack(ui::ig::StackedLayout::Stack &stack) {
    return [&stack](const bool open) {
      if (open) { stack.setActive(); }
    };
  }

  ui::ig::StackedLayout::Stack &addItemStart(const SearchResult &res, std::span<const std::string> section);

  ui::ig::StackedLayout::Stack &addItemImpl(TreeNode &parent, std::span<const std::string> section, std::size_t index);

  SearchResult searchItem(std::span<const std::string> section);

  std::string getNextChildName();

  // clang-format off
  ui::ig::BoxLayout layout;
    ui::ig::BoxLayout &menuLayout;
      ui::ig::InputText &searchInput;
      ui::ig::Tree<ui::ig::TreeType::Simple> &menuTree;
    ui::ig::StackedLayout &helpStack;
  // clang-format on
  std::size_t idCounter{};
  std::unique_ptr<HelpLoader> loader;
};

}// namespace pf
#endif//PHYSARUMSIM_HELPPANEL_H
