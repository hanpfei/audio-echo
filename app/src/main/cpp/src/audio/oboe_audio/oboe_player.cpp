//
// Created by hanpfei on 20-10-21.
//

#include "oboe_player.h"

#include "android_debug.h"

//static const int kChannelCount = 1;
static const int kSampleRate = 48000;

class PlayerCallback : public oboe::AudioStreamCallback {
 public:
  virtual ~PlayerCallback() = default;

  oboe::DataCallbackResult onAudioReady(
          oboe::AudioStream *oboeStream,
          void *audioData,
          int32_t numFrames) override {
    LOGI("PlayerCallback onAudioReady numFrames %d", numFrames);
    int16_t *data = reinterpret_cast<int16_t *>(audioData);
    for (int i = 0; i < numFrames; ++i) {
      data[i]= 0xF1AC;
    }
    return oboe::DataCallbackResult::Continue;
  }

  void onErrorBeforeClose(oboe::AudioStream* oboeStream, oboe::Result error) override {

  }

  void onErrorAfterClose(oboe::AudioStream* oboeStream, oboe::Result error) override {

  }
};

OboePlayer::OboePlayer() {
  player_callback_ = std::make_unique<PlayerCallback>();
}

OboePlayer::~OboePlayer() {}

int OboePlayer::Init() {
  oboe::AudioStreamBuilder builder;
  // The builder set methods can be chained for convenience.
  builder.setSharingMode(oboe::SharingMode::Exclusive)
          ->setPerformanceMode(oboe::PerformanceMode::LowLatency)
          ->setChannelCount(oboe::ChannelCount::Mono)
          ->setChannelConversionAllowed(true)
          ->setSampleRate(kSampleRate)
          ->setFramesPerCallback(kSampleRate / 100)
          ->setFormat(oboe::AudioFormat::I16)
          ->setCallback(player_callback_.get());

  builder.openManagedStream(audio_stream_);

  return 0;
}

int OboePlayer::Terminate() {
  audio_stream_->close();
  audio_stream_.reset();

  return 0;
}

int OboePlayer::InitPlayout() { return 0; }

int OboePlayer::StartPlayout() {
  audio_stream_->requestStart();
  return 0;
}

int OboePlayer::StopPlayout() {
  audio_stream_->requestStop();
  return 0;
}

int OboePlayer::SpeakerVolumeIsAvailable(bool &available) { return 0; }

int OboePlayer::SetSpeakerVolume(uint32_t volume) { return 0; }

int OboePlayer::SpeakerVolume(uint32_t &volume) const { return 0; }

int OboePlayer::MaxSpeakerVolume(uint32_t &maxVolume) const { return 0; }

int OboePlayer::MinSpeakerVolume(uint32_t &minVolume) const { return 0; }

int32_t OboePlayer::SetPlayoutParameters(PlayoutParameters* params) { return 0; }