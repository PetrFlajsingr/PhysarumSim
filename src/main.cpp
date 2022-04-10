#include "GlobalThreadpool.h"
#include "app_icon.h"
#include "renderers/PhysarumRenderer.h"
#include "simulation/generators/RandomParticleGenerator.h"
#include "utils/files.h"
#include <filesystem>
#include <fmt/format.h>
#include <geGL/DebugMessage.h>
#include <images/VideoRecorder.h>
#include <images/save.h>
#include <magic_enum.hpp>
#include <numbers>
#include <pf_glfw/GLFW.h>
#include <pf_imgui/dialogs/FileDialog.h>
#include <pf_imgui/enums.h>
#include <pf_mainloop/MainLoop.h>
#include <toml++/toml.h>
#include <ui/UI.h>
#include <ui/about_data/FolderAboutDataLoader.h>
#include <ui/help_data/FolderHelpLoader.h>
#include <utils/FPSCounter.h>
#include <utils/rand.h>

// TODO: clean this up, divide
/**
 * Load toml config located next to the exe - config.toml
 * @return
 */
toml::table loadConfig() {
  constexpr auto DEFAULT_CONFIG = R"toml(

)toml";
  const auto configPath = pf::getExeFolder() / "config.toml";
  if (!exists(configPath)) {}
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

typedef void(APIENTRY *GLDEBUGPROC)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                    const GLchar *message, const void *userParam);

void glDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message,
                    const void *userParam) {
  fmt::print("{}\n", std::string(message, length));
}

glm::vec2 mousePosToTexPos(pf::glfw::Position<double> mousePos, pf::glfw::Size<int> winSize, glm::ivec2 texSize) {
  const auto nX = mousePos.x / winSize.width;
  const auto nY = mousePos.y / winSize.height;
  return glm::vec2{texSize} * glm::vec2{nX, nY};
}

glm::vec2 mousePosToTexPos(ImVec2 mousePos, pf::ui::ig::Size winSize, glm::ivec2 texSize) {
  const auto nX = mousePos.x / winSize.width;
  const auto nY = mousePos.y / winSize.height;// TODO: check this on windows
  return glm::vec2{texSize} * glm::vec2{nX, nY};
}

// TODO: texture size change in UI
int main(int argc, char *argv[]) {
  pf::initGlobalThreadPool(4);
  using namespace pf;
  using namespace pf::glfw;
  using namespace pf::enum_operators;
  using namespace pf::ui;
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
  if (config["window"]["maximized"].value<bool>().value_or(false)) { window->maximize(); }

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
  const auto helpFolder = resourcesFolder / "help";
  const auto aboutFolder = resourcesFolder / "licenses";

  auto ui = ogl::UI{*config["imgui"].as_table(), window->getHandle(), std::make_unique<FolderHelpLoader>(helpFolder),
                    std::make_unique<FolderAboutDataLoader>(aboutFolder)};

  auto sim = std::make_unique<physarum::PhysarumSimulator>(shaderFolder, trailTextureSize);

  ogl::PhysarumRenderer renderer{shaderFolder, sim->getTrailTexture(), trailTextureSize};

  bool anySpecies = false;
  std::vector<Subscription> speciesSubscriptions{};
  const auto initFromUI = [&] {
    std::ranges::for_each(speciesSubscriptions, &Subscription::unsubscribe);
    anySpecies = !ui.speciesPanels.empty();
    if (!anySpecies) { return; }
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
  ui.simControlsPanel->addSimStateListener([&](bool running) { isSimPaused = !running; });

  window->setCursorPositionCallback([&](const auto &cursorPos) {
    if (ui.imguiInterface->isWindowHovered()) { return; }
    const auto attractorPosition = mousePosToTexPos(window->getCursorPosition(), window->getSize(), trailTextureSize);

    sim->setMouseInteractionActive(window->getLastMouseButtonState(MouseButton::Left) == ButtonState::Down);
    sim->setAttractorPosition(attractorPosition);
  }, true);
  window->setKeyCallback(
      [&](Key key, KeyAction action, const Flags<ModifierKey> &mods) {
        if (key == Key::H && action == KeyAction::Down && mods.is(ModifierKey::Alt)) {
          ui.imguiInterface->setVisibility(!ui.imguiInterface->getVisibility());
        } else if (key == Key::Enter && action == KeyAction::Down && mods.is(ModifierKey::Alt)) {
          // TODO: fullscreen switch
        }
        if (ui.imguiInterface->isKeyboardCaptured()) { return; }
        if (action == KeyAction::Up) { return; }
        switch (key) {
          case Key::Space: {
            isSimPaused = !isSimPaused;
            ui.simControlsPanel->setSimRunning(!isSimPaused);
            break;
          }
          case Key::Right: {
            const auto newSteps = std::clamp(ui.simControlsPanel->getStepsPerFrame() + 1, 1, 10);
            ui.simControlsPanel->setStepsPerFrame(newSteps);
            break;
          }
          case Key::Left: {
            const auto newSteps = std::clamp(ui.simControlsPanel->getStepsPerFrame() - 1, 1, 10);
            ui.simControlsPanel->setStepsPerFrame(newSteps);
            break;
          }
          default: break;
        }
      },
      true);

  ui.outImage->addMousePositionListener([&](const auto &mousePos) {
    const auto size = ui.outImage->getSize();
    const auto texPos = mousePosToTexPos(mousePos, size, trailTextureSize);
    const auto isBtnDown = window->getLastMouseButtonState(MouseButton::Left) == ButtonState::Down;
    sim->setMouseInteractionActive(isBtnDown);
    sim->setAttractorPosition(texPos);
  });

  ui.mouseInteractionPanel->addValueListener([&](const auto config) { sim->setInteractionConfig(config); }, true);

  ui.onScreenshotSave = [&](const auto &path) {
    const auto imgFormat = getImageFormat(path).value();
    const auto &texture = renderer.getRenderTexture();
    auto imageData = texture->getData(0, GL_RGBA, GL_UNSIGNED_BYTE);
    GlobalThreadPool().enqueue([path, imgFormat, trailTextureSize, data = std::move(imageData), &ui] {
      try {
        saveImage(path, imgFormat, PixelFormat::RGBA, trailTextureSize.x, trailTextureSize.y, std::span{data});
        MainLoop::Get()->enqueue([&ui, path] {
          ui.imguiInterface->getNotificationManager()
              .createNotification(ig::NotificationType::Success, ig::uniqueId(), "Success")
              .createChild<ig::Text>(ig::uniqueId(), fmt::format("Image saved to '{}'", path.string()));
        });
      } catch (...) {
        MainLoop::Get()->enqueue([&ui] {
          ui.imguiInterface->getNotificationManager()
              .createNotification(ig::NotificationType::Error, ig::uniqueId(), "Error", std::chrono::seconds{5})
              .createChild<ig::Text>(ig::uniqueId(), "Image failed to save");
        });
      }
    });
  };

  ui.simControlsPanel->addRestartClickListener(initFromUI);

  ui.backgroundColorEdit->addValueListener([&](const auto &color) { renderer.setBackgroundColor(color); }, true);
  ui.blendTypeCombobox->addValueListener([&](const auto blendType) { renderer.setBlendType(blendType); }, true);

  const auto updateUIPosition = [&] {
    const auto winSize = window->getSize();
    ui.dockWindow->setPosition({0.f, 19.f});
    const auto dockSize = ig::Size{winSize.width, winSize.height - 19 - ui.statusBar->getHeight()};
    ui.dockWindow->setSize(dockSize);
   // ui.dockWindow->setSize(ig::Size{winSize.width, winSize.height - 19});
  };

  window->setSizeListener([&](const auto &size) {
    updateUIPosition();
    glViewport(0, 0, size.width, size.height);
  }, true);

  updateUIPosition();

  FPSCounter fpsCounter{};
  const auto fpsLabelUpdateFrequency = std::chrono::milliseconds{100};
  auto timeSinceLastFpsLabelUpdate = fpsLabelUpdateFrequency;

  VideoRecorder recorder{[](auto f) { MainLoop::Get()->enqueue(f); },
                         [&](const auto &msg) {
                           const auto errMsg = fmt::format("Recording has failed: '{}'", msg);
                           ui.imguiInterface->getNotificationManager()
                               .createNotification(ig::NotificationType::Error, ig::uniqueId(), "Error")
                               .createChild<ig::Text>(ig::uniqueId(), errMsg);
                           fmt::print(stderr, errMsg);
                         },
                         [&](const auto &path) {
                           ui.imguiInterface->getNotificationManager()
                               .createNotification(ig::NotificationType::Success, ig::uniqueId(), "Success")
                               .createChild<ig::Text>(ig::uniqueId(),
                                                      fmt::format("Recording has been saved to '{}'", path.string()));
                         }};

  const auto startRecording = [&] {
    using namespace pf::ui::ig;
    ui.imguiInterface->buildFileDialog(FileDialogType::File)
        .label("Select save location")
        .extension({{"mp4"}, "mp4", Color::RGB(255, 0, 0)})
        .onSelect( [&](const auto &selected) {
          const auto &dst = selected[0];
          const auto res =
              recorder.start(trailTextureSize.x, trailTextureSize.y, 60, AVPixelFormat::AV_PIX_FMT_RGBA, dst);
          if (res.has_value()) {
            ui.imguiInterface->getNotificationManager()
                .createNotification(ig::NotificationType::Error, ig::uniqueId(), "Error")
                .createChild<ig::Text>(ig::uniqueId(), fmt::format("Error while starting recording: '{}'", *res));
            ui.recorderPanel->setValue(RecordingState::Stopped);
          }
          ui.recorderPanel->startCounter();
        })
        .onCancel([&] { ui.recorderPanel->setValue(RecordingState::Stopped); })
        .size(pf::ui::ig::Size{500, 400})
        .build();
  };

  auto isRecordingPaused = false;
  ui.recorderPanel->addValueListener([&](RecordingState recState) {
    switch (recState) {
      case RecordingState::Stopped:
        isRecordingPaused = false;
        recorder.stop();
        break;
      case RecordingState::Running:
        if (isRecordingPaused) {
          isRecordingPaused = false;
          return;
        }
        isRecordingPaused = false;
        startRecording();
        break;
      case RecordingState::Paused: isRecordingPaused = true; break;
    }
  });

  std::optional<float> fixedStep = std::nullopt;
  ui.simControlsPanel->addFixedStepListener([&](const auto val) { fixedStep = val; });

  MainLoop::Get()->setOnMainLoop([&](std::chrono::nanoseconds deltaT) {
    try {
      glfw.setSwapInterval(0);
      if (window->shouldClose()) { MainLoop::Get()->stop(); }
      ui.particleCountText->setText("Particle count: {}", sim->getParticleCount());
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      const float currentTime =
          std::chrono::duration_cast<std::chrono::microseconds>(MainLoop::Get()->getRuntime()).count() / 1000000.f;
      float timeDelta = std::chrono::duration_cast<std::chrono::microseconds>(deltaT).count() / 1000000.f
          * ui.simControlsPanel->getTimeMultiplier();
      if (fixedStep.has_value()) {
        timeDelta = *fixedStep;
      }

      if (anySpecies) { renderer.render(); }

      if (anySpecies && !isSimPaused) {
        for (int i = 0; i < ui.simControlsPanel->getStepsPerFrame(); ++i) { sim->simulate(currentTime, timeDelta); }
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
      if (recorder.isRecording() && !isRecordingPaused) {
        const auto &texture = renderer.getRenderTexture();
        auto imageData = texture->getData(0, GL_RGBA, GL_UNSIGNED_BYTE);// TODO: speed this up somehow
        recorder.write(std::move(imageData));
      }
    } catch (const std::exception &e) {
      fmt::print(stderr, "Exception: {}\n", e.what());
      ui.imguiInterface->createMsgDlg("Exception", e.what(), Flags<ig::MessageButtons>{ig::MessageButtons::Ok},
                                      [](auto) { return true; });
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

  GlobalThreadPool().cancelAndStop();
  return 0;
}
