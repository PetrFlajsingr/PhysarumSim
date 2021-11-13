//
// Created by xflajs00 on 08.11.2021.
//

#ifndef PHYSARUMSIM_SRC_UI_SIMULATIONCONTROLSPANEL_H
#define PHYSARUMSIM_SRC_UI_SIMULATIONCONTROLSPANEL_H

#include <pf_imgui/elements/Button.h>
#include <pf_imgui/elements/DragInput.h>
#include <pf_imgui/interface/Element.h>
#include <pf_imgui/interface/RenderablesContainer.h>
#include <pf_imgui/layouts/BoxLayout.h>

namespace pf {

class SimulationControlsPanel : public ui::ig::Element, public ui::ig::RenderablesContainer {
 public:
  explicit SimulationControlsPanel(const std::string &name);

  std::vector<Renderable *> getRenderables() override;

  Subscription addStepsPerFrameListener(std::invocable<int> auto &&listener) {
    return simSpeedDrag->addValueListener(std::forward<decltype(listener)>(listener));
  }

  Subscription addRestartClickListener(std::invocable auto &&listener) {
    return restartSimButton->addClickListener(std::forward<decltype(listener)>(listener));
  }

  Subscription addSimStateListener(std::invocable<bool> auto &&listener) {
    return runningObservable.addListener(std::forward<decltype(listener)>(listener));
  }

  Subscription addTimeMultiplierListener(std::invocable<float> auto &&listener) {
    return timeMultiplierDrag->addValueListener(std::forward<decltype(listener)>(listener));
  }

  [[nodiscard]] bool isSimRunning() const;
  void setSimRunning(bool isRunning);

  [[nodiscard]] int getStepsPerFrame() const;
  void setStepsPerFrame(int steps);

  [[nodiscard]] float getTimeMultiplier() const;
  void setTimeMultiplier(float multiplier);

 protected:
  void renderImpl() override;

 private:
  void createElements();
  void registerListeners();
  void addTooltips();

  // clang-format off
  ui::ig::BoxLayout layout;
    ui::ig::BoxLayout *buttonsLayout;
      ui::ig::Button *playPauseButton;
      ui::ig::Button *restartSimButton;
      ui::ig::DragInput<int> *simSpeedDrag;
    ui::ig::DragInput<float> *timeMultiplierDrag;
  // clang-format on

  bool running = false;
  ui::ig::Observable_impl<bool> runningObservable;
};

}// namespace pf

#endif//PHYSARUMSIM_SRC_UI_SIMULATIONCONTROLSPANEL_H
