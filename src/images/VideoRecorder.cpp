//
// Created by xflajs00 on 09.11.2021.
//

#include "VideoRecorder.h"
#include <fmt/format.h>
#include <iostream>

namespace pf {

VideoEncoder::VideoEncoder(std::uint32_t frameWidth, std::uint32_t frameHeight, std::uint32_t frameRate,
                           AVPixelFormat pixelFormat, const std::filesystem::path &dst)
    : formatContext(AVFormatContextPtr(nullptr, [](auto *) {})), packet(AVPacketPtr{av_packet_alloc(), [](auto *) {}}),
      outputStream(AVStreamPtr{nullptr, [](auto *) {}}), pixelFormat(pixelFormat) {
  {
    auto tmpPtr = formatContext.get();
    avformat_alloc_output_context2(&tmpPtr, nullptr, nullptr, dst.string().c_str());
    formatContext = std::unique_ptr<AVFormatContext, AVFormatContextDeleter>{tmpPtr, [](auto *) {}};
  }

  auto codec = avcodec_find_encoder(AV_CODEC_ID_H264);

  outputStream =
      std::unique_ptr<AVStream, AVStreamDeleter>{avformat_new_stream(formatContext.get(), nullptr), [](auto *) {}};
  outputStream->id = static_cast<int>(formatContext->nb_streams - 1);

  codecContext = std::unique_ptr<AVCodecContext, AVCodecContextDeleter>{avcodec_alloc_context3(codec), [](auto *) {}};
  avcodec_get_context_defaults3(codecContext.get(), codec);

  codecContext->codec_id = codec->id;
  codecContext->width = static_cast<int>(frameWidth);
  codecContext->height = static_cast<int>(frameHeight);
  codecContext->bit_rate = frameWidth * frameHeight * frameRate;
  codecContext->time_base = outputStream->time_base = {.num = 1, .den = static_cast<int>(frameRate)};
  codecContext->gop_size = 10;
  codecContext->max_b_frames = 1;
  codecContext->pix_fmt = AV_PIX_FMT_YUV420P;
  codecContext->profile = FF_PROFILE_H264_EXTENDED;

  av_opt_set(codecContext->priv_data, "preset", "slow", 0);

  avcodec_open2(codecContext.get(), codec, nullptr);

  frame = std::unique_ptr<AVFrame, AVFrameDeleter>{av_frame_alloc(), [](auto) {}};
  frame->format = codecContext->pix_fmt;
  frame->width = codecContext->width;
  frame->height = codecContext->height;
  av_image_alloc(frame->data, frame->linesize, frame->width, frame->height, static_cast<AVPixelFormat>(frame->format),
                 32);
  frame->pts = 0;

  avcodec_parameters_from_context(outputStream->codecpar, codecContext.get());

  av_dump_format(formatContext.get(), 0, dst.string().c_str(), 1);

  auto tmpPtr2 = formatContext.get();
  if (!(formatContext->oformat->flags & AVFMT_NOFILE)) {
    avio_open(&tmpPtr2->pb, dst.string().c_str(), AVIO_FLAG_WRITE);
  }

  [[maybe_unused]] auto a = avformat_write_header(formatContext.get(), nullptr);
  av_frame_make_writable(frame.get());
}

void VideoEncoder::write(std::span<const std::byte> data) {
  const int in_linesize[1] = {4 * frame->width};
  static struct SwsContext *swsContext = nullptr;
  swsContext = sws_getCachedContext(swsContext, frame->width, frame->height, pixelFormat, frame->width, frame->height,
                                    AV_PIX_FMT_YUV420P, 0, nullptr, nullptr, nullptr);
  auto a = data.data();
  sws_scale(swsContext, (const uint8_t *const *) &a, in_linesize, 0, frame->height, frame->data, frame->linesize);
  int returnValue = 0;
  av_init_packet(packet.get());
  packet->data = nullptr;
  packet->size = 0;

  returnValue = avcodec_send_frame(codecContext.get(), frame.get());
  if (returnValue < 0) { throw std::runtime_error{fmt::format("Error sending frame: {}", av_err2str(returnValue))}; }

  while (returnValue >= 0) {
    returnValue = avcodec_receive_packet(codecContext.get(), packet.get());
    if (returnValue == AVERROR(EAGAIN) || returnValue == AVERROR_EOF) break;
    else if (returnValue < 0) {
      throw std::runtime_error{fmt::format("Error encoding a frame: {}", av_err2str(returnValue))};
    }
    av_packet_rescale_ts(packet.get(), codecContext->time_base, outputStream->time_base);
    packet->stream_index = outputStream->index;
    returnValue = av_interleaved_write_frame(formatContext.get(), packet.get());
    if (returnValue < 0) { throw std::runtime_error{fmt::format("Error writing frame: {}", av_err2str(returnValue))}; }
    av_packet_unref(packet.get());
  }
  ++frame->pts;
}

VideoEncoder::~VideoEncoder() {
  av_write_trailer(formatContext.get());
  avcodec_close(codecContext.get());
  if (!(formatContext->flags & AVFMT_NOFILE)) avio_closep(&formatContext->pb);
}

std::optional<std::string> VideoRecorder::start(std::uint32_t frameWidth, std::uint32_t frameHeight,
                                                std::uint32_t frameRate, AVPixelFormat pixelFormat,
                                                const std::filesystem::path &dst) {
  if (encoder != nullptr) { return "Recording not finished"; }
  stopped = false;
  if (recorderThread != nullptr) {
    recorderThread->join();
    recorderThread = nullptr;
  }
  frameQueue = nullptr;
  writingFailed = false;
  try {
    videoPath = dst;
    encoder = std::make_unique<VideoEncoder>(frameWidth, frameHeight, frameRate, pixelFormat, dst);
    frameQueue = std::make_unique<SafeQueue<std::vector<std::byte>>>();
    recorderThread = std::make_unique<std::thread>(std::bind_front(&VideoRecorder::runThread, this));
  } catch (const std::exception &e) {
    writingFailed = true;
    return e.what();
  }
  return std::nullopt;
}

void VideoRecorder::runThread() {
  try {
    while (!stopped) {
      if (auto data = frameQueue->dequeue(); data.has_value()) {
        encoder->write(data.value());
      } else {
        break;
      }
    }
    synchronize([this] { onDone(videoPath); });
  } catch (const std::exception &e) {
    writingFailed = true;
    synchronize([this, what = e.what()] { onException(what); });
  }
  frameQueue->shutdown();
  encoder = nullptr;
  stopped = true;
}

bool VideoRecorder::isRecording() const { return encoder != nullptr; }

void VideoRecorder::stop() {
  frameQueue->shutdown();
  stopped = true;
}

bool VideoRecorder::didLastRecordingFail() const { return writingFailed; }

void VideoRecorder::write(std::vector<std::byte> data) { frameQueue->enqueue(std::move(data)); }

VideoRecorder::~VideoRecorder() {
  if (recorderThread != nullptr) {
    stop();
    recorderThread->join();
  }
}
}// namespace pf
