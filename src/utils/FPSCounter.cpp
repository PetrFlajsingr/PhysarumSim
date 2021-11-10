/**
 * @file FPSCounter.cpp
 * @brief An FPS counter.
 * @author Petr Flajšingr
 * @date 9.11.20
 */

#include "FPSCounter.h"

namespace pf {
using namespace std::chrono_literals;

float FPSCounter::averageFPS() const {
  //return static_cast<float>(totalFrameCount) / totalTime.count() * std::chrono::duration_cast<Duration>(1s).count();
  return 1.f / rollingAverage.count() * std::chrono::duration_cast<Duration>(1s).count();
}

float FPSCounter::currentFPS() const {
  return 1.f / frameDuration.count() * std::chrono::duration_cast<Duration>(1s).count();
}

FPSCounter::Duration FPSCounter::currentDuration() const { return frameDuration; }

FPSCounter::Duration FPSCounter::averageDuration() const { return totalTime / totalFrameCount; }

void FPSCounter::onFrame() {
  ++totalFrameCount;
  const auto now = std::chrono::steady_clock::now();
  frameDuration = std::chrono::duration_cast<Duration>(now - lastFrame);
  totalTime += frameDuration;
  lastFrame = now;

  constexpr auto alpha = 0.05;
  rollingAverage = Duration(
      static_cast<unsigned long long>((alpha * frameDuration.count()) + (1.0 - alpha) * rollingAverage.count()));

  onNewFrame(*this);
}

void FPSCounter::reset() {
  totalFrameCount = 0;
  frameDuration = 0ms;
  totalTime = 0ms;
  lastFrame = std::chrono::steady_clock::now();
}

std::size_t FPSCounter::currentFrameNumber() const { return totalFrameCount; }
}// namespace pf