/**
 * @file FPSCounter.h
 * @brief An FPS counter.
 * @author Petr Flajšingr
 * @date 9.11.20
 */

#ifndef REALISTIC_VOXEL_RENDERING_SRC_UTILS_FPSCOUNTER_H
#define REALISTIC_VOXEL_RENDERING_SRC_UTILS_FPSCOUNTER_H

#include <chrono>
#include <functional>

namespace pf {
/**
 * @brief An FPS counter.
 *
 * Call onFrame() to register time diff.
 */
class FPSCounter {
 public:
  using Duration = std::chrono::nanoseconds;
  FPSCounter() = default;
  /**
   * Updates FPS stats.
   */
  void onFrame();
  /**
   * Reset FPS stats.
   */
  void reset();
  /**
   * Set a callback on each FPS update.
   * @param callback
   */
  void setOnNewFrame(std::invocable<const FPSCounter &> auto callback) { onNewFrame = callback; }

  [[nodiscard]] float averageFPS() const;
  [[nodiscard]] float currentFPS() const;
  [[nodiscard]] std::size_t currentFrameNumber() const;

  /**
   * Last frame duration.
   * @return
   */
  [[nodiscard]] Duration currentDuration() const;
  /**
   * Average frame duration
   * @return
   */
  [[nodiscard]] Duration averageDuration() const;

 private:
  std::size_t totalFrameCount = 0;
  Duration frameDuration{};
  Duration totalTime{};
  std::chrono::steady_clock::time_point lastFrame = std::chrono::steady_clock::now();

  std::function<void(const FPSCounter &)> onNewFrame = [](auto) {};
};

}// namespace pf
#endif//REALISTIC_VOXEL_RENDERING_SRC_UTILS_FPSCOUNTER_H
