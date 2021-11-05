#include "renderers/PhysarumRenderer.h"
#include "simulation/generators/PointParticleGenerator.h"
#include "simulation/generators/RandomParticleGenerator.h"
#include "ui/UI.h"
#include "utils/files.h"
#include <filesystem>
#include <fmt/format.h>
#include <magic_enum.hpp>
#include <numbers>
#include <pf_glfw/GLFW.h>
#include <pf_mainloop/MainLoop.h>
#include <toml++/toml.h>
#include <ui/UI.h>
#include <utils/rand.h>

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
  using namespace pf;
  const auto config = loadConfig();
  const auto resourcesFolder = std::filesystem::path{config["files"]["resources_path"].value<std::string>().value()};

  const glm::ivec2 windowSize{config["window"]["width"].value<int>().value(),
                              config["window"]["height"].value<int>().value()};

  fmt::print("Initializing window and OpenGL\n");
  glfw::GLFW glfw{};
  auto window = glfw.createWindow({.width = static_cast<size_t>(windowSize.x),
                                   .height = static_cast<size_t>(windowSize.y),
                                   .title = config["window"]["title"].value<std::string>().value(),
                                   .majorOpenGLVersion = 4,
                                   .minorOpenGLVersion = 6});
  window->setCurrent();
  if (!gladLoadGLLoader((GLADloadproc) glfw.getLoaderFnc())) {
    fmt::print(stderr, "Error while initializing GLAD");
    return -1;
  }

  const auto shaderFolder = resourcesFolder / "shaders";

  auto ui = ogl::UI{*config["imgui"].as_table(), window->getHandle()};

  window->setInputIgnorePredicate([&] { return ui.imguiInterface->isWindowHovered() || ui.imguiInterface->isKeyboardCaptured(); });

  auto sim = std::make_unique<physarum::PhysarumSimulator>(shaderFolder, windowSize);

  ogl::PhysarumRenderer renderer{shaderFolder, sim->getTrailTexture(), windowSize};

  renderer.init({ui.speciesPanels[0]->getConfig().color});
  sim->initialize({ui.speciesPanels[0]->getConfig()});
  renderer.setTrailTexture(sim->getTrailTexture());

  ui.setOutImage(renderer.getRenderTexture());

  ui.speciesPanels[0]->addValueListener([&](const physarum::PopulationConfig &config) {
    sim->updateConfig(config, 0);
    renderer.setConfig(config.color, 0);
  });

  bool isAttractorActive = false;
  window->setMouseButtonCallback([&](glfw::MouseButton btn, glfw::MouseButtonAction action, const Flags<glfw::ModifierKey> &mods) {
    if (ui.imguiInterface->isWindowHovered() || ui.imguiInterface->isKeyboardCaptured()) { return; }
    if (btn == glfw::MouseButton::Left) {
      isAttractorActive = action == glfw::MouseButtonAction::Press;
      sim->setAttractorEnabled(isAttractorActive);
      const auto cursorPos = window->getCursorPosition();
      sim->setAttractorPosition({cursorPos.x, window->getSize().height - cursorPos.y});
    }
  });
  window->setCursorPositionCallback([&](const auto &cursorPos) {
    sim->setAttractorPosition({cursorPos.x, window->getSize().height - cursorPos.y});
  });

  bool isSimPaused = true;
  ui.playPauseButton->addClickListener([&] {
    isSimPaused = !isSimPaused;
    if (isSimPaused) {
      ui.playPauseButton->setLabel("Start");
    } else {
      ui.playPauseButton->setLabel("Pause");
    }
  });

  ui.restartSimButton->addClickListener([&] {
    renderer.init({ui.speciesPanels[0]->getConfig().color});
    sim->initialize({ui.speciesPanels[0]->getConfig()});
    renderer.setTrailTexture(sim->getTrailTexture());
  });

  ui.backgroundColorEdit->addValueListener([&](const auto &color) {
    renderer.setBackgroundColor(color);
  },
                                           true);

  MainLoop::Get()->setOnMainLoop([&](std::chrono::nanoseconds deltaT) {
    if (window->shouldClose()) {
      MainLoop::Get()->stop();
    }

    const float currentTime = std::chrono::duration_cast<std::chrono::microseconds>(MainLoop::Get()->getRuntime()).count() / 1000000.f;
    const float timeDelta = std::chrono::duration_cast<std::chrono::microseconds>(deltaT).count() / 1000000.f;

    if (!isSimPaused) {
      for (int i = 0; i < ui.simSpeedDrag->getValue(); ++i) {
        sim->simulate(currentTime, timeDelta);
      }
    }

    renderer.render();
    ui.imguiInterface->render();
    window->swapBuffers();
    glfw.pollEvents();
  });

  fmt::print("Starting main loop\n");
  MainLoop::Get()->run();
  fmt::print("Main loop ended\n");

  saveConfig(config, *ui.imguiInterface);
  return 0;
}
