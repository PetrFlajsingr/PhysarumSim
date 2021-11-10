//
// Created by xflajs00 on 09.11.2021.
//

#ifndef PHYSARUMSIM_VIDEORECORDER_H
#define PHYSARUMSIM_VIDEORECORDER_H

#include <array>
#include <filesystem>
#include <functional>
#include <optional>
#include <pf_common/parallel/SafeQueue.h>
#include <span>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/error.h>
#include <libavutil/imgutils.h>//for av_image_alloc only
#include <libavutil/opt.h>
#include <libswscale/swscale.h>
inline static char av_err_bfr[AV_ERROR_MAX_STRING_SIZE];
#undef av_err2str
#define av_err2str(errnum) av_make_error_string(av_err_bfr, AV_ERROR_MAX_STRING_SIZE, errnum)
}

namespace pf {
//TODO: fix this up, this is all kinds of wrong
class VideoEncoder {
 public:
  VideoEncoder(std::uint32_t frameWidth, std::uint32_t frameHeight, std::uint32_t frameRate, AVPixelFormat pixelFormat,
               const std::filesystem::path &dst);

  virtual ~VideoEncoder();

  void write(std::span<const std::byte> data);

 private:
  using AVFormatContextDeleter = std::function<void(AVFormatContext *)>;
  using AVCodecContextDeleter = std::function<void(AVCodecContext *)>;
  using AVFrameDeleter = std::function<void(AVFrame *)>;
  using AVPacketDeleter = std::function<void(AVPacket *)>;
  using AVStreamDeleter = std::function<void(AVStream *)>;

  using AVFormatContextPtr = std::unique_ptr<AVFormatContext, AVFormatContextDeleter>;
  using AVCodecContextPtr = std::unique_ptr<AVCodecContext, AVCodecContextDeleter>;
  using AVFramePtr = std::unique_ptr<AVFrame, AVFrameDeleter>;
  using AVPacketPtr = std::unique_ptr<AVPacket, AVPacketDeleter>;
  using AVStreamPtr = std::unique_ptr<AVStream, AVStreamDeleter>;

  std::array<uint8_t, 4> endcode{0, 0, 1, 0xb7};
  AVFormatContextPtr formatContext;
  AVCodecContextPtr codecContext{nullptr, [](auto) {}};
  AVFramePtr frame{nullptr, [](auto) {}};
  AVPacketPtr packet;
  AVStreamPtr outputStream;
  AVPixelFormat pixelFormat;

  bool hasFailed = false;
};

class VideoRecorder {
 public:
  /**
         *
         * @param synchronize synchronization function for callbacks
         * @param onException
         * @param onDone
         */
  VideoRecorder(std::invocable<std::function<void()>> auto &&synchronize,
                std::invocable<std::string> auto &&onException, std::invocable<std::filesystem::path> auto &&onDone)
      : synchronize(std::forward<decltype(synchronize)>(synchronize)),
        onException(std::forward<decltype(onException)>(onException)), onDone(std::forward<decltype(onDone)>(onDone)) {}

  virtual ~VideoRecorder();

  [[nodiscard]] std::optional<std::string> start(std::uint32_t frameWidth, std::uint32_t frameHeight,
                                                 std::uint32_t frameRate, AVPixelFormat pixelFormat,
                                                 const std::filesystem::path &dst);

  void stop();

  void write(std::vector<std::byte> data);

  [[nodiscard]] bool didLastRecordingFail() const;

  [[nodiscard]] bool isRecording() const;

 private:
  void runThread();

  std::unique_ptr<SafeQueue<std::vector<std::byte>>> frameQueue = nullptr;

  std::unique_ptr<std::thread> recorderThread{nullptr};
  std::unique_ptr<VideoEncoder> encoder{nullptr};
  std::filesystem::path videoPath;

  bool writingFailed = false;
  bool stopped = false;

  std::function<void(std::function<void()>)> synchronize;
  std::function<void(std::string)> onException;
  std::function<void(std::filesystem::path)> onDone;
};

}// namespace pf

#endif//PHYSARUMSIM_VIDEORECORDER_H
