//
// Created by xflajs00 on 10.11.2021.
//

#ifndef PHYSARUMSIM_RECORDERPANEL_H
#define PHYSARUMSIM_RECORDERPANEL_H

#include <pf_imgui/elements/Button.h>
#include <pf_imgui/elements/Text.h>
#include <pf_imgui/interface/Element.h>
#include <pf_imgui/interface/ValueObservable.h>
#include <pf_imgui/layouts/HorizontalLayout.h>
#include <pf_mainloop/MainLoop.h>
#include <optional>

namespace pf {

enum class RecordingState { Stopped, Running, Paused };

class RecorderPanel : public ui::ig::Element, public ui::ig::ValueObservable<RecordingState> {
 public:
  explicit RecorderPanel(const std::string &name);

  Subscription addRecordingStateListener(std::invocable<RecordingState> auto &&listener) {
    return addListener(std::forward<decltype(listener)>(listener));
  }

  void setRecordingState(RecordingState state);
  [[nodiscard]] RecordingState getRecordingState() const;
  void setValue(const RecordingState &newValue) override;

  void startCounter();

 protected:
  void renderImpl() override;

 private:
  void setRecordTimeText();
  // clang-format off
  ui::ig::HorizontalLayout layout;
    ui::ig::Button *startStopRecordButton;
    ui::ig::Button *pauseResumeRecordButton;
    ui::ig::Text *recordTimeText;
  // clang-format on
  std::chrono::milliseconds recordDuration;

  bool counting = false;

  std::optional<RepeatCancel> blinkCancel;
};

}// namespace pf

#endif//PHYSARUMSIM_RECORDERPANEL_H
