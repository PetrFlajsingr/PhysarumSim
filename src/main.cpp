#include "app_icon.h"
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
#include <utils/FPSCounter.h>
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
void saveConfig(toml::table config, pf::ogl::UI &ui) {
  const auto configPath = pf::getExeFolder() / "config.toml";
  const auto configPathStr = configPath.string();
  fmt::print("Saving config file to: '{}'\n", configPathStr);
  ui.imguiInterface->updateConfig();
  auto imguiConfig = ui.imguiInterface->getConfig();
  ui.cleanupConfig(imguiConfig);
  config.insert_or_assign("imgui", imguiConfig);
  auto ofstream = std::ofstream(configPathStr);
  ofstream << config;
}

int main(int argc, char *argv[]) {
  using namespace pf;
  using namespace pf::glfw;
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

  auto s = std::span{APP_ICON.pixel_data, static_cast<std::size_t>(APP_ICON.width * APP_ICON.height)};

  // FIXME
  GLFWimage imageHandle;
  imageHandle.width = APP_ICON.width;
  imageHandle.height = APP_ICON.height;
  imageHandle.pixels = const_cast<unsigned char *>(APP_ICON.pixel_data);

  glfwSetWindowIcon(window->getHandle(), 1, &imageHandle);

  if (!gladLoadGLLoader((GLADloadproc) glfw.getLoaderFnc())) {
    fmt::print(stderr, "Error while initializing GLAD");
    return -1;
  }
  glfw.setSwapInterval(0);

  const auto shaderFolder = resourcesFolder / "shaders";

  auto ui = ogl::UI{*config["imgui"].as_table(), window->getHandle()};

  window->setInputIgnorePredicate([&] { return ui.imguiInterface->isWindowHovered() || ui.imguiInterface->isKeyboardCaptured(); });

  auto sim = std::make_unique<physarum::PhysarumSimulator>(shaderFolder, windowSize);

  ogl::PhysarumRenderer renderer{shaderFolder, sim->getTrailTexture(), windowSize};

  bool anySpecies = false;
  std::vector<Subscription> speciesSubscriptions{};
  const auto initFromUI = [&] {
    std::ranges::for_each(speciesSubscriptions, &Subscription::unsubscribe);
    anySpecies = !ui.speciesPanels.empty();
    if (!anySpecies) {
      return;
    }
    std::vector<physarum::PopulationConfig> configs;
    std::ranges::transform(ui.speciesPanels, std::back_inserter(configs), &SpeciesPanel::getConfig);
    std::vector<physarum::PopulationColor> colors;
    std::ranges::transform(configs, std::back_inserter(colors), &physarum::PopulationConfig::color);
    int cnt = 0;
    std::ranges::for_each(ui.speciesPanels, [&](const auto panel) {
      int index = cnt++;
      speciesSubscriptions.emplace_back(panel->addValueListener([&, index](const physarum::PopulationConfig &config) {
        sim->updateConfig(config, index);
        renderer.setConfig(config.color, index);
      }));
    });
    sim->initialize(configs);
    renderer.init(colors);
    renderer.setTrailTexture(sim->getTrailTexture());
  };

  initFromUI();

  ui.setOutImage(renderer.getRenderTexture());

  bool isAttractorActive = false;
  window->setMouseButtonCallback([&](glfw::MouseButton btn, glfw::ButtonState state, const Flags<glfw::ModifierKey> &mods) {
    if (ui.imguiInterface->isWindowHovered() || ui.imguiInterface->isKeyboardCaptured()) { return; }
    if (btn == glfw::MouseButton::Left) {
      isAttractorActive = state == glfw::ButtonState::Down;
      sim->setMouseInteractionActive(isAttractorActive);
      const auto cursorPos = window->getCursorPosition();
      sim->setAttractorPosition({cursorPos.x, window->getSize().height - cursorPos.y});
    }
  });
  window->setCursorPositionCallback([&](const auto &cursorPos) {
    sim->setAttractorPosition({cursorPos.x, window->getSize().height - cursorPos.y});
  });
  window->setKeyCallback([&](Key key, KeyAction action, Flags<ModifierKey>) {
    if (key == Key::H && action == KeyAction::Down) {
      ui.imguiInterface->setVisibility(!ui.imguiInterface->getVisibility());
    }
  });
  ui.mouseInteractionPanel->addValueListener([&](const auto config) {
    sim->setInteractionConfig(config);
  },
                                             true);

  bool isSimPaused = true;
  ui.playPauseButton->addClickListener([&] {
    isSimPaused = !isSimPaused;
    if (isSimPaused) {
      ui.playPauseButton->setLabel("Start");
    } else {
      ui.playPauseButton->setLabel("Pause");
    }
  });

  ui.restartSimButton->addClickListener(initFromUI);

  ui.backgroundColorEdit->addValueListener([&](const auto &color) {
    renderer.setBackgroundColor(color);
  },
                                           true);
  ui.blendTypeCombobox->addValueListener([&](const auto blendType) {
    renderer.setBlendType(blendType);
  },
                                         true);

  FPSCounter fpsCounter{};
  MainLoop::Get()->setOnMainLoop([&](std::chrono::nanoseconds deltaT) {
    if (window->shouldClose()) {
      MainLoop::Get()->stop();
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const float currentTime = std::chrono::duration_cast<std::chrono::microseconds>(MainLoop::Get()->getRuntime()).count() / 1000000.f;
    const float timeDelta = std::chrono::duration_cast<std::chrono::microseconds>(deltaT).count() / 1000000.f;

    if (anySpecies) {
      if (!isSimPaused) {
        for (int i = 0; i < ui.simSpeedDrag->getValue(); ++i) {
          sim->simulate(currentTime, timeDelta);
        }
      }
      renderer.render();
    }
    ui.fpsAveragePlot->addValue(fpsCounter.averageFPS());
    ui.fpsCurrentPlot->addValue(fpsCounter.currentFPS());
    ui.fpsLabel->setText("Average FPS: {}", fpsCounter.averageFPS());
    ui.imguiInterface->render();
    window->swapBuffers();
    glfw.pollEvents();
    fpsCounter.onFrame();
  });

  fmt::print("Starting main loop\n");
  MainLoop::Get()->run();
  fmt::print("Main loop ended\n");

  saveConfig(config, ui);
  return 0;
}
