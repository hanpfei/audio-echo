//
// Created by hanpfei on 20-10-21.
//

#include "oboe_recorder.h"

#include "android_debug.h"

static const int kSampleRate = 48000;

class RecorderCallback : public oboe::AudioStreamCallback {
 public:
  virtual ~RecorderCallback() = default;

  oboe::DataCallbackResult onAudioReady(
          oboe::AudioStream *oboeStream,
          void *audioData,
          int32_t numFrames) override {
    LOGI("RecorderCallback onAudioReady numFrames %d", numFrames);
    return oboe::DataCallbackResult::Continue;
  }

  void onErrorBeforeClose(oboe::AudioStream* oboeStream, oboe::Result error) override {
  }

  void onErrorAfterClose(oboe::AudioStream* oboeStream, oboe::Result error) override {
  }
};

OboeRecorder::OboeRecorder() {
  recorder_callback_ = std::make_unique<RecorderCallback>();
}

OboeRecorder::~OboeRecorder() {}

int OboeRecorder::Init() {
  oboe::AudioStreamBuilder builder;
  // The builder set methods can be chained for convenience.
  builder.setSharingMode(oboe::SharingMode::Exclusive)
          ->setPerformanceMode(oboe::PerformanceMode::LowLatency)
          ->setChannelCount(oboe::ChannelCount::Mono)
          ->setChannelConversionAllowed(true)
          ->setSampleRate(kSampleRate)
          ->setDirection(oboe::Direction::Input)
          ->setFramesPerCallback(kSampleRate / 100)
          ->setFormat(oboe::AudioFormat::I16)
          ->setCallback(recorder_callback_.get());

  builder.openManagedStream(audio_stream_);

  return 0;
}

int OboeRecorder::Terminate() {
  audio_stream_->close();
  audio_stream_.reset();

  return 0;
}

int OboeRecorder::InitRecording() { return 0; }

int OboeRecorder::StartRecording() {
  audio_stream_->requestStart();
  return 0;
}

int OboeRecorder::StopRecording() {
  audio_stream_->requestStop();

  return 0;
}

int OboeRecorder::EnableBuiltInAEC(bool enable) { return 0; }

int OboeRecorder::EnableBuiltInAGC(bool enable) { return 0; }

int OboeRecorder::EnableBuiltInNS(bool enable) { return 0; }

int OboeRecorder::SetRecordParameters(RecordParameters* params) { return 0; }