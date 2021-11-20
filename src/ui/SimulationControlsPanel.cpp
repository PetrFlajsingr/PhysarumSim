//
// Created by xflajs00 on 08.11.2021.
//

#include "SimulationControlsPanel.h"
#include <pf_imgui/icons.h>

namespace pf {
using namespace ui::ig;

SimulationControlsPanel::SimulationControlsPanel(const std::string &name)
    : Element(name),
      layout({name + "_root", LayoutDirection::TopToBottom, Size::Auto(), AllowCollapse::No, ShowBorder::Yes}) {
  createElements();
  registerListeners();
  addTooltips();
}

std::vector<Renderable *> SimulationControlsPanel::getRenderables() { return layout.getRenderables(); }

void SimulationControlsPanel::renderImpl() { layout.render(); }

void SimulationControlsPanel::createElements() {
  buttonsLayout = &layout.createChild<BoxLayout>(getName() + "button_layout", LayoutDirection::LeftToRight,
                                                 Size{Width::Auto(), 30});
  playPauseButton = &buttonsLayout->createChild<Button>(getName() + "play_pause_button", ICON_FK_PLAY);
  restartSimButton = &buttonsLayout->createChild<Button>(getName() + "restart_button", ICON_FK_REFRESH);
  simSpeedDrag = &buttonsLayout->createChild<DragInput<int>>(getName() + "sim_speed_drag", "Steps per frame", 1, 1, 10,
                                                             1, Persistent::Yes);
  timeMultiplierDrag = &layout.createChild<DragInput<float>>(getName() + "time_nult_drag", "Time multiplier", .01f,
                                                             .01f, 10.f, 1.f, Persistent::Yes);

  fixedStepLayout =
      &layout.createChild<BoxLayout>(getName() + "fixed_layout", LayoutDirection::LeftToRight, Size{Width::Auto(), 30});
  fixedStepCheckbox =
      &fixedStepLayout->createChild<Checkbox>(getName() + "chkbx", "Fixed step", false, Persistent::Yes);
  fixedStepDrag = &fixedStepLayout->createChild<DragInput<float>>(getName() + "fix_drag", "Time step", 0.001f, 0.001f,
                                                                  1.f, 1.f / 60.f);
}

void SimulationControlsPanel::registerListeners() {
  playPauseButton->addClickListener([&] {
    setSimRunning(!running);
    runningObservable.notify(running);
  });
  fixedStepCheckbox->addValueListener(
      [this](const auto enabled) {
        fixedStepDrag->setVisibility(enabled ? Visibility::Visible : Visibility::Invisible);
        timeMultiplierDrag->setVisibility(enabled ? Visibility::Invisible : Visibility::Visible);
        fixedStepObservable.notify(enabled ? std::optional{fixedStepDrag->getValue()} : std::nullopt);
      },
      true);
  fixedStepDrag->addValueListener([this](const auto value) {
    if (fixedStepCheckbox->getValue()) { fixedStepObservable.notify(value); }
  });
}

void SimulationControlsPanel::addTooltips() {
  playPauseButton->setTooltip("Resum simulation");
  restartSimButton->setTooltip("Restart simulation");
  simSpeedDrag->setTooltip("Set how many simulations steps are computed per frame");
  timeMultiplierDrag->setTooltip("Set simulation time multiplier");
  fixedStepCheckbox->setTooltip("Run the simulation with fixed time step");
  fixedStepDrag->setTooltip("Fixed time step of the simulation");
}

bool SimulationControlsPanel::isSimRunning() const { return running; }

void SimulationControlsPanel::setSimRunning(bool isRunning) {
  running = isRunning;
  if (running) {
    playPauseButton->setLabel(ICON_FK_PAUSE);
    playPauseButton->setTooltip("Pause simulation");
  } else {
    playPauseButton->setLabel(ICON_FK_PLAY);
    playPauseButton->setTooltip("Resume simulation");
  }
}

int SimulationControlsPanel::getStepsPerFrame() const { return simSpeedDrag->getValue(); }

void SimulationControlsPanel::setStepsPerFrame(int steps) { simSpeedDrag->setValue(steps); }

float SimulationControlsPanel::getTimeMultiplier() const { return timeMultiplierDrag->getValue(); }

void SimulationControlsPanel::setTimeMultiplier(float multiplier) { timeMultiplierDrag->setValue(multiplier); }

}// namespace pf