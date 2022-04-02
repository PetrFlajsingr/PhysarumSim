//
// Created by xflajs00 on 10.11.2021.
//

#include "RecorderPanel.h"
#include <pf_imgui/icons.h>
#include <fmt/chrono.h>

namespace pf {
using namespace ui::ig;
using namespace std::chrono_literals;

RecorderPanel::RecorderPanel(const std::string &name)
    : Element(name), ValueObservable(RecordingState::Stopped),
      layout(name + "_root", LayoutDirection::LeftToRight, Size{Width::Auto(), 25}) {
  startStopRecordButton = &layout.createChild<Button>(name + "_start_stop_btn", ICON_FK_CIRCLE);
  pauseResumeRecordButton = &layout.createChild<Button>(name + "", ICON_FK_PAUSE);
  recordTimeText = &layout.createChild<Text>(name + "_rec_test", "");

  startStopRecordButton->addClickListener([&] {
    const auto state = getValue();
    if (state == RecordingState::Stopped) {
      setRecordingState(RecordingState::Running);
    } else {
      setRecordingState(RecordingState::Stopped);
    }
  });
  pauseResumeRecordButton->addClickListener([&] {
    const auto state = getValue();
    if (state == RecordingState::Running) {
      setRecordingState(RecordingState::Paused);
    } else {
      setRecordingState(RecordingState::Running);
    }
  });
  setRecordingState(RecordingState::Stopped);
}

void RecorderPanel::setRecordingState(RecordingState state) {
  if (blinkCancel.has_value()) {
    blinkCancel->cancel();
    blinkCancel= std::nullopt;
    layout.unsetColor<style::ColorOf::ChildBackground>();
  }
  switch (state) {
    case RecordingState::Stopped:
      counting = false;
      recordDuration = 0ms;
      recordTimeText->setVisibility(Visibility::Invisible);
      startStopRecordButton->setLabel(ICON_FK_CIRCLE);
      startStopRecordButton->setColor<style::ColorOf::Text>(Color::RGB(60, 0, 0));
      startStopRecordButton->setTooltip("Start recording");
      pauseResumeRecordButton->setVisibility(Visibility::Invisible);
      break;
    case RecordingState::Running:
      recordTimeText->setVisibility(Visibility::Visible);
      startStopRecordButton->setLabel(ICON_FK_STOP);
      startStopRecordButton->setColor<style::ColorOf::Text>(Color::White);
      startStopRecordButton->setTooltip("Stop recording");
      pauseResumeRecordButton->setLabel(ICON_FK_PAUSE);
      pauseResumeRecordButton->setVisibility(Visibility::Visible);
      pauseResumeRecordButton->setTooltip("Pause recording");
      blinkCancel = MainLoop::Get()->repeat([this] {
        if (layout.getColor<style::ColorOf::ChildBackground>().has_value()) {
          layout.unsetColor<style::ColorOf::ChildBackground>();
        } else {
          layout.setColor<style::ColorOf::ChildBackground>(Color::RGB(60, 0, 0));
        }
      }, std::chrono::milliseconds{500});
      break;
    case RecordingState::Paused:
      recordTimeText->setVisibility(Visibility::Visible);
      startStopRecordButton->setLabel(ICON_FK_STOP);
      startStopRecordButton->setTooltip("Stop recording");
      pauseResumeRecordButton->setLabel(ICON_FK_PLAY);
      pauseResumeRecordButton->setVisibility(Visibility::Visible);
      pauseResumeRecordButton->setTooltip("Resume recording");
      break;
  }
  ValueObservable::setValue(state);
}

RecordingState RecorderPanel::getRecordingState() const { return getValue(); }

void RecorderPanel::setValue(const RecordingState &newValue) { setRecordingState(newValue); }

void RecorderPanel::renderImpl() {
  layout.render();
  if (getValue() == RecordingState::Running && counting) {
    recordDuration += 16ms;
    setRecordTimeText();
  }
}

void RecorderPanel::setRecordTimeText() {
  recordTimeText->setText("{:%M:%S}", recordDuration);
}
void RecorderPanel::startCounter() {
  counting = true;
}

}// namespace pf