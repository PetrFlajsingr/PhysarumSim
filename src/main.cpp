#include "renderers/DemoRenderer.h"
#include "ui/DemoImGui.h"
#include "utils/files.h"
#include <filesystem>
#include <fmt/format.h>
#include <magic_enum.hpp>
#include <pf_glfw/GLFW.h>
#include <pf_mainloop/MainLoop.h>
#include <toml++/toml.h>
#include <ui/DemoImGui.h>

/**
 * Load toml config located next to the exe - config.toml
 * @return
 */
toml::table loadConfig() {
  const auto configPath = pf::getExeFolder() / "config.toml";
  const auto configPathStr = configPath.string();
  fmt::print("Loading config from: '{}'\n", configPathStr);
  return toml::parse_file(configPathStr);
}

/**
 * Serialize UI, save it to the config and save the config next to the exe into config.toml
 */
void saveConfig(toml::table config, pf::ui::ig::ImGuiInterface &imguiInterface) {
  const auto configPath = pf::getExeFolder() / "config.toml";
  const auto configPathStr = configPath.string();
  fmt::print("Saving config file to: '{}'\n", configPathStr);
  imguiInterface.updateConfig();
  config.insert_or_assign("imgui", imguiInterface.getConfig());
  auto ofstream = std::ofstream(configPathStr);
  ofstream << config;
}

int main(int argc, char *argv[]) {
  const auto config = loadConfig();
  const auto resourcesFolder = std::filesystem::path{config["files"]["resources_path"].value<std::string>().value()};

  fmt::print("Initializing window and OpenGL\n");
  pf::glfw::GLFW glfw{};
  auto window = glfw.createWindow({.width = 1200,
                                   .height = 900,
                                   .title = "OpenGL",
                                   .majorOpenGLVersion = 4,
                                   .minorOpenGLVersion = 6});
  window->setCurrent();
  if (!gladLoadGLLoader((GLADloadproc) glfw.getLoaderFnc())) {
    fmt::print(stderr, "Error while initializing GLAD");
    return -1;
  }

  auto demoUI = pf::ogl::DemoImGui{*config["imgui"].as_table(), window->getHandle()};

  window->setInputIgnorePredicate([&] { return demoUI.imguiInterface->isWindowHovered() || demoUI.imguiInterface->isKeyboardCaptured(); });

  pf::ogl::DemoRenderer renderer{resourcesFolder / "shaders"};
  if (const auto initResult = renderer.init(); initResult.has_value()) {
    fmt::print(stderr, "Error during initialization: {}\n", initResult.value());
    return -1;
  }

  window->setMouseClickCallback([&](pf::glfw::MouseButton btn, pf::Flags<pf::glfw::ModifierKey> mods) {
    std::string txt = fmt::format("Clicked {} button", magic_enum::enum_name(btn));
    if (mods.is(pf::glfw::ModifierKey::Shift)) {
      txt += " with shift";
    }
    if (mods.is(pf::glfw::ModifierKey::Control)) {
      txt += " with Control";
    }
    demoUI.imguiInterface->showNotification(pf::ui::ig::NotificationType::Info, txt);
  });

  pf::MainLoop::Get()->setOnMainLoop([&](auto) {
    if (window->shouldClose()) {
      pf::MainLoop::Get()->stop();
    }
    renderer.render();
    demoUI.imguiInterface->render();
    window->swapBuffers();
    glfw.pollEvents();
  });

  fmt::print("Starting main loop\n");
  pf::MainLoop::Get()->run();
  fmt::print("Main loop ended\n");

  saveConfig(config, *demoUI.imguiInterface);
  return 0;
}
