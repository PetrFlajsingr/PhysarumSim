//
// Created by xflajs00 on 10.11.2021.
//

#include "RecorderPanel.h"
#include <pf_imgui/icons.h>

namespace pf {
using namespace ui::ig;

// TODO: pause resume button
RecorderPanel::RecorderPanel(const std::string &name)
    : Element(name), ValueObservable(RecordingState::Stopped),
      layout(name + "_root", LayoutDirection::LeftToRight, Size{Width::Auto(), 25}) {
  startStopRecordButton = &layout.createChild<Button>(name + "_start_stop_btn", ICON_FK_CIRCLE);
  pauseResumeRecordButton = &layout.createChild<Button>(name + "", ICON_FK_PAUSE);
  recordTimeText = &layout.createChild<Text>(name + "_rec_test", "HH:MM.zzzz");

  startStopRecordButton->addClickListener([&] {
    const auto state = getValue();
    if (state == RecordingState::Stopped || state == RecordingState::Paused) {
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
  switch (state) {
    case RecordingState::Stopped:
      startStopRecordButton->setLabel(ICON_FK_CIRCLE);
      startStopRecordButton->setColor<style::ColorOf::Text>(ImVec4(60, 0, 0, 1));
      pauseResumeRecordButton->setVisibility(Visibility::Invisible);
      break;
    case RecordingState::Running:
      startStopRecordButton->setLabel(ICON_FK_STOP);
      startStopRecordButton->setColor<style::ColorOf::Text>(ImVec4(255, 255, 255, 1));
      pauseResumeRecordButton->setLabel(ICON_FK_PAUSE);
      pauseResumeRecordButton->setVisibility(Visibility::Visible);
      break;
    case RecordingState::Paused:
      startStopRecordButton->setLabel(ICON_FK_STOP);
      pauseResumeRecordButton->setLabel(ICON_FK_PLAY);
      pauseResumeRecordButton->setVisibility(Visibility::Visible);
      break;
  }
  ValueObservable::setValue(state);
}

RecordingState RecorderPanel::getRecordingState() const { return getValue(); }

void RecorderPanel::setValue(const RecordingState &newValue) { setRecordingState(newValue); }

void RecorderPanel::renderImpl() { layout.render(); }

}// namespace pf