//
// Created by xflajs00 on 22.10.2021.
//

#include "DemoImGui.h"
#include <pf_imgui/backends/ImGuiGlfwOpenGLInterface.h>
#include <pf_imgui/styles/dark.h>

pf::ogl::DemoImGui::DemoImGui(const toml::table &config, GLFWwindow *windowHandle) {
  using namespace ui::ig;
  imguiInterface = std::make_unique<ImGuiGlfwOpenGLInterface>(ImGuiGlfwOpenGLConfig{
      .windowHandle = windowHandle,
      .flags = {},
      .enableMultiViewport = true,
      .config = config,
      .pathToIconFolder = *config["path_icons"].value<std::string>(),
      .enabledIconPacks = IconPack::FontAwesome5Regular,
      .defaultFontSize = 13.f});
  setDarkStyle(*imguiInterface);

  window1 = &imguiInterface->createWindow("demo_window", "Demo window");
  layout1 = &window1->createChild<BoxLayout>("box_layout_1", LayoutDirection::TopToBottom, Size::Auto(), AllowCollapse::No, Persistent::Yes);
  listBox1 = &layout1->createChild<Listbox<std::string>>("list_box_1", "Listbox", Size::FillWidth(), std::nullopt, Persistent::Yes);
  listboxLabel = &layout1->createChild<Text>("label1", "No selection");
  listBox1->addItem("item1");
  listBox1->addItem("item2");
  listBox1->addItem("item3");
  listBox1->addValueListener([this](const auto &newVal) {
    listboxLabel->setText(newVal);
  });
  button1 = &layout1->createChild<Button>("button1", "Open file");
  button1->addClickListener([this] {
    imguiInterface->openFileDialog(
        "Select a file", {FileExtensionSettings{{"*.txt"}, "text file", ImVec4{1, 0, 0, 1}}}, [this](const auto &files) {
      std::string str{};
      for (const auto &file : files) {
        str += file.string() + '\n';
      }
      auto &dialog = imguiInterface->createDialog("dialog1", "Selected Files");
      dialog.createChild<Text>("dialog_text", "", ImVec4{0, 0, 1, 1}).setText("Selected files:\n{}", str);
      dialog.createChild<Button>("close_dialog_btn", "Close").addClickListener([&dialog] { dialog.close(); }); }, [] {}, Size{300, 200}, ".", "", Modal::Yes);
  });
  button2 = &layout1->createChild<Button>("button2", "Enable/disable listbox");
  button2->addClickListener([this] {
    listBox1->setEnabled(listBox1->getEnabled() == Enabled::Yes ? Enabled::No : Enabled::Yes);
  });
  checkbox1 = &layout1->createChild<Checkbox>("checkbox_1", "Checkbox", false, Persistent::Yes);
  checkboxLabel = &layout1->createChild<Text>("label1", "Not selected");
  checkbox1->addValueListener([this](auto selected) {
    std::string str{};
    if (selected) {
      str = "Selected";
    } else {
      str = "Not selected";
    }
    checkboxLabel->setText(str);
  });
  radioGroup = &layout1->createChild<RadioGroup>("radio_group1", "Radio group", std::vector<std::unique_ptr<RadioButton>>{}, std::nullopt, Persistent::Yes);
  radioGroup->addButton("r_btn1", "First");
  radioGroup->addButton("r_btn2", "Second");
  radioGroup->addButton("r_btn3", "Third");
  radioGroup->addButton("r_btn4", "Fourth");
  radioGroupLabel = &layout1->createChild<Text>("label3", "Not selected");
  radioGroup->addValueListener([this](const auto &value) {
    radioGroupLabel->setText(std::string{value});
  });

  imguiInterface->setStateFromConfig();
}
