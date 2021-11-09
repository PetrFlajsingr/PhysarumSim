#include "app_icon.h"
#include "renderers/PhysarumRenderer.h"
#include "simulation/generators/PointParticleGenerator.h"
#include "simulation/generators/RandomParticleGenerator.h"
#include "ui/UI.h"
#include "utils/files.h"
#include <filesystem>
#include <fmt/format.h>
#include <images/save.h>
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

glm::vec2 mousePosToTexPos(pf::glfw::Position<double> mousePos, pf::glfw::Size<int> winSize, glm::ivec2 texSize) {
  const auto nX = mousePos.x / winSize.width;
  const auto nY = 1.f - mousePos.y / winSize.height;
  return glm::vec2{texSize} * glm::vec2{nX, nY};
}

glm::vec2 mousePosToTexPos(ImVec2 mousePos, pf::ui::ig::Size winSize, glm::ivec2 texSize) {
  const auto nX = mousePos.x / winSize.width;
  const auto nY = 1.f - mousePos.y / winSize.height;
  return glm::vec2{texSize} * glm::vec2{nX, nY};
}

// TODO: texture size change in UI
int main(int argc, char *argv[]) {
  using namespace pf;
  using namespace pf::glfw;
  using namespace pf::enum_operators;
  auto config = loadConfig();
  const auto resourcesFolder = std::filesystem::path{config["files"]["resources_path"].value<std::string>().value()};

  const glm::ivec2 trailTextureSize{config["physarum"]["texture_width"].value<int>().value(),
                                    config["physarum"]["texture_height"].value<int>().value()};

  fmt::print("Initializing window and OpenGL\n");
  glfw::GLFW glfw{};
  auto window = glfw.createWindow({.width = static_cast<size_t>(config["window"]["width"].value<int>().value()),
                                   .height = static_cast<size_t>(config["window"]["height"].value<int>().value()),
                                   .title = "Physarum",
                                   .majorOpenGLVersion = 4,
                                   .minorOpenGLVersion = 6});
  window->setCurrent();
  if (config["window"]["maximized"].value<bool>().value_or(false)) {
    window->maximize();
  }

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

  const auto shaderFolder = resourcesFolder / "shaders";

  auto ui = ogl::UI{*config["imgui"].as_table(), window->getHandle()};

  window->setInputIgnorePredicate([&] { return ui.imguiInterface->isWindowHovered() || ui.imguiInterface->isKeyboardCaptured(); });

  auto sim = std::make_unique<physarum::PhysarumSimulator>(shaderFolder, trailTextureSize);

  ogl::PhysarumRenderer renderer{shaderFolder, sim->getTrailTexture(), trailTextureSize};

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

  bool isSimPaused = true;
  ui.simControlsPanel->addSimStateListener([&](bool running) {
    isSimPaused = !running;
  });

  window->setCursorPositionCallback([&](const auto &cursorPos) {
    const auto attractorPosition = mousePosToTexPos(window->getCursorPosition(), window->getSize(), trailTextureSize);

    sim->setMouseInteractionActive(window->getLastMouseButtonState(MouseButton::Left) == ButtonState::Down);
    sim->setAttractorPosition(attractorPosition);
  });
  window->setKeyCallback([&](Key key, KeyAction action, Flags<ModifierKey> mods) {
    if (key == Key::H && action == KeyAction::Down) {
      ui.imguiInterface->setVisibility(!ui.imguiInterface->getVisibility());
    } else if (key == Key::Space && action == KeyAction::Down) {
      isSimPaused = !isSimPaused;
      ui.simControlsPanel->setSimRunning(!isSimPaused);
    } else if (key == Key::Enter && action == KeyAction::Down && mods.is(ModifierKey::Alt)) {
      // TODO: fullscreen switch
    }
  });

  ui.outImage->addMousePositionListener([&](const auto &mousePos) {
    const auto size = ui.outImage->getSize();
    const auto isBtnDown = window->getLastMouseButtonState(MouseButton::Left) == ButtonState::Down;
    sim->setMouseInteractionActive(isBtnDown);
    sim->setAttractorPosition(mousePosToTexPos(mousePos, size, trailTextureSize));
  });

  ui.mouseInteractionPanel->addValueListener([&](const auto config) {
    sim->setInteractionConfig(config);
  },
                                             true);

  ui.onScreenshotSave = [&](const auto &path) {
    const auto imgFormat = getImageFormat(path).value();
    auto texture = renderer.getRenderTexture();
    auto imageData = texture->getData(0, GL_RGBA, GL_UNSIGNED_BYTE);
    saveImage(path, imgFormat, PixelFormat::RGBA, trailTextureSize.x, trailTextureSize.y, std::span{imageData});
  };

  ui.simControlsPanel->addRestartClickListener(initFromUI);

  ui.backgroundColorEdit->addValueListener([&](const auto &color) {
    renderer.setBackgroundColor(color);
  },
                                           true);
  ui.blendTypeCombobox->addValueListener([&](const auto blendType) {
    renderer.setBlendType(blendType);
  },
                                         true);

  const auto updateUIPosition = [&] {
    const auto winSize = window->getSize();
    ui.dockWindow->setPosition(ImVec2{0.f, 19.f});
    ui.dockWindow->setSize(ui::ig::Size{winSize.width, winSize.height - 19});
  };

  window->setSizeListener([&](const auto &size) {
    updateUIPosition();
    glViewport(0, 0, size.width, size.height);
  });

  updateUIPosition();

  FPSCounter fpsCounter{};
  const auto fpsLabelUpdateFrequency = std::chrono::milliseconds{100};
  auto timeSinceLastFpsLabelUpdate = fpsLabelUpdateFrequency;
  MainLoop::Get()->setOnMainLoop([&](std::chrono::nanoseconds deltaT) {
    try {
      glfw.setSwapInterval(0);
      if (window->shouldClose()) {
        MainLoop::Get()->stop();
      }
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      const float currentTime = std::chrono::duration_cast<std::chrono::microseconds>(MainLoop::Get()->getRuntime()).count() / 1000000.f;
      const float timeDelta = std::chrono::duration_cast<std::chrono::microseconds>(deltaT).count() / 1000000.f;

      if (anySpecies) {
        if (!isSimPaused) {
          for (int i = 0; i < ui.simControlsPanel->getSimSpeed(); ++i) {
            sim->simulate(currentTime, timeDelta);
          }
        }
        renderer.render();
      }
      ui.fpsAveragePlot->addValue(fpsCounter.averageFPS());
      ui.fpsCurrentPlot->addValue(fpsCounter.currentFPS());
      timeSinceLastFpsLabelUpdate += std::chrono::duration_cast<std::chrono::milliseconds>(deltaT);
      if (timeSinceLastFpsLabelUpdate >= fpsLabelUpdateFrequency) {
        timeSinceLastFpsLabelUpdate = std::chrono::milliseconds{0};
        ui.fpsLabel->setText("Average FPS: {}", fpsCounter.averageFPS());
      }
      ui.imguiInterface->render();

      window->swapBuffers();
      glfw.pollEvents();
      fpsCounter.onFrame();
    } catch (const std::exception &e) {
      fmt::print(stderr, "Exception: {}\n", e.what());
      ui.imguiInterface->createMsgDlg("Exception", e.what(), Flags<ui::ig::MessageButtons>{ui::ig::MessageButtons::Ok}, [](auto) { return true; });
    }
  });

  fmt::print("Starting main loop\n");
  MainLoop::Get()->run();
  fmt::print("Main loop ended\n");

  const auto windowSize = window->getSize();
  config["window"].as_table()->insert_or_assign("width", windowSize.width);
  config["window"].as_table()->insert_or_assign("height", windowSize.height);
  config["window"].as_table()->insert_or_assign("maximized", window->isMaximized());
  saveConfig(config, ui);
  return 0;
}
