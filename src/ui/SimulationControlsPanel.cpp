//
// Created by xflajs00 on 08.11.2021.
//

#include "SimulationControlsPanel.h"
#include <pf_imgui/icons.h>

namespace pf {
using namespace ui::ig;

SimulationControlsPanel::SimulationControlsPanel(const std::string &name) : Element(name),
                                                                            layout({name + "_root", LayoutDirection::TopToBottom, Size::Auto(), AllowCollapse::No, ShowBorder::Yes}) {
  createElements();
  registerListeners();
  addTooltips();
}

std::vector<Renderable *> SimulationControlsPanel::getRenderables() {
  return layout.getRenderables();
}

void SimulationControlsPanel::renderImpl() {
  layout.render();
}

void SimulationControlsPanel::createElements() {
  buttonsLayout = &layout.createChild<BoxLayout>(getName() + "button_layout", LayoutDirection::LeftToRight, Size{Width::Auto(), 30});
  playPauseButton = &buttonsLayout->createChild<Button>(getName() + "play_pause_button", ICON_FK_PLAY);
  restartSimButton = &buttonsLayout->createChild<Button>(getName() + "restart_button", ICON_FK_REFRESH);
  simSpeedDrag = &buttonsLayout->createChild<DragInput<int>>(getName() + "sim_speed_drag", "Simulation speed", 1, 1, 10, 1, Persistent::Yes);
}

void SimulationControlsPanel::registerListeners() {
  playPauseButton->addClickListener([&] {
    setSimRunning(!running);
    runningObservable.notify(running);
  });
}

void SimulationControlsPanel::addTooltips() {
  playPauseButton->setTooltip("Pause simulation");
  restartSimButton->setTooltip("Restart simulation");
  simSpeedDrag->setTooltip("Set simulation speed");
}

bool SimulationControlsPanel::isSimRunning() const { return running; }

void SimulationControlsPanel::setSimRunning(bool isRunning) {
  running = isRunning;
  if (running) {
    playPauseButton->setLabel(ICON_FK_PAUSE);
    playPauseButton->setTooltip("Resume simulation");
  } else {
    playPauseButton->setLabel(ICON_FK_PLAY);
    playPauseButton->setTooltip("Pause simulation");
  }
}

int SimulationControlsPanel::getSimSpeed() const {
  return simSpeedDrag->getValue();
}

void SimulationControlsPanel::setSimSpeed(int speed) {
  simSpeedDrag->setValue(speed);
}

}// namespace pf