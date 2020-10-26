//
// Created by hanpfei on 20-10-21.
//

#include "oboe_player.h"

#include "android_debug.h"
#include "audio/audio_device_buffer.h"

class PlayerCallback : public oboe::AudioStreamCallback {
 public:
  PlayerCallback(OboePlayer *oboe_player) : oboe_player_(oboe_player) {
  }

  virtual ~PlayerCallback() = default;

  oboe::DataCallbackResult onAudioReady(
          oboe::AudioStream *oboeStream,
          void *audioData,
          int32_t numFrames) override {
    if (oboe_player_) {
      oboe_player_->ProcessPlayoutDataRequest(oboeStream, audioData, numFrames);
    }
    return oboe::DataCallbackResult::Continue;
  }

  void onErrorBeforeClose(oboe::AudioStream *oboeStream, oboe::Result error) override {
    if (oboe_player_) {
      oboe_player_->ProcessPlayoutError(oboeStream, error);
    }
  }

  void onErrorAfterClose(oboe::AudioStream *oboeStream, oboe::Result error) override {
    if (oboe_player_) {
      oboe_player_->ProcessPlayoutError(oboeStream, error);
    }
  }

 private:
  OboePlayer *oboe_player_;
};

OboePlayer::OboePlayer() :
        lock_(),
        initialized_(false),
        playing_(false) {
  playout_parameters_.sample_rate_hz_ = kDefaultSampleRate;
  playout_parameters_.channels_ = kDefaultChannels;
  playout_parameters_.playout_bufsize_factor_ = 0.0f;
}

OboePlayer::~OboePlayer() {
  Terminate();
}

int32_t OboePlayer::SetPlayoutParameters(const PlayoutParameters &params) {
  std::lock_guard<std::mutex> lk(lock_);
  playout_parameters_ = params;
  return 0;
}

void OboePlayer::AttachAudioBuffer(std::shared_ptr<AudioDeviceBuffer> audio_buffer) {
  std::lock_guard<std::mutex> lk(lock_);
  audio_device_buffer_ = audio_buffer;
  audio_device_buffer_->SetPlayoutSampleRate(playout_parameters_.sample_rate_hz_);
  audio_device_buffer_->SetPlayoutChannels(playout_parameters_.channels_);
}

int OboePlayer::Init() {
  if (!stream_builder_) {
    player_callback_ = std::make_unique<PlayerCallback>(this);

    stream_builder_ = std::make_unique<oboe::AudioStreamBuilder>();
    stream_builder_->setSharingMode(oboe::SharingMode::Shared)
            ->setPerformanceMode(oboe::PerformanceMode::LowLatency)
            ->setDirection(oboe::Direction::Output)
            ->setFormat(oboe::AudioFormat::I16)
            ->setCallback(player_callback_.get());
  }

  return 0;
}

int OboePlayer::Terminate() {
  std::lock_guard<std::mutex> lk(lock_);
  if (audio_stream_) {
    audio_stream_->close();
    audio_stream_.reset();
  }

  stream_builder_.reset();
  player_callback_.reset();
  audio_device_buffer_.reset();

  initialized_ = false;

  return 0;
}

int OboePlayer::InitPlayout() {
  std::lock_guard<std::mutex> lk(lock_);
  if (initialized_) {
    LOGW("Oboe playout has initialized.");
    return -1;
  }
  if (!stream_builder_) {
    LOGW("Player audio stream builder has not been created.");
    return -1;
  }

  int32_t sample_rate = playout_parameters_.sample_rate_hz_;
  int channels = playout_parameters_.channels_;
  if (sample_rate == 0 || (channels != 1 && channels != 2)) {
    LOGW("Invalid playout parameters: sample rate %d, channels %d", sample_rate, channels);
    return -1;
  }

  if (audio_stream_) {
    LOGW("Oboe player has been initialized.");
    return -1;
  }

  if (channels == 1) {
    stream_builder_->setChannelCount(oboe::ChannelCount::Mono);
  } else if (channels == 2) {
    stream_builder_->setChannelCount(oboe::ChannelCount::Stereo);
  }
  stream_builder_->setSampleRate(sample_rate);
  stream_builder_->setFramesPerCallback(sample_rate / 100);

  if (stream_builder_->willUseAAudio()) {
    LOGI("Playout will use aaudio");
  }

  auto result = stream_builder_->openManagedStream(audio_stream_);
  if (result != oboe::Result::OK) {
    LOGW("Open audio stream failed %d: sample rate %d, channels %d",
         static_cast<int>(result), sample_rate, channels);
    return -1;
  }

  initialized_ = true;

  return 0;
}

int OboePlayer::StartPlayout() {
  std::lock_guard<std::mutex> lk(lock_);
  if (playing_) {
    LOGW("Playout has been started.");
    return -1;
  }
  if (!audio_stream_) {
    LOGW("Playout audio stream has not been created");
    return -1;
  }
  auto recorder_state = audio_stream_->getState();
  LOGI("Player state %d in start playout", static_cast<int>(recorder_state));
  if (recorder_state >= oboe::StreamState::Open
      && (recorder_state != oboe::StreamState::Starting
          && recorder_state != oboe::StreamState::Started)
      && recorder_state < oboe::StreamState::Closing) {
    auto result = audio_stream_->requestStart();
    if (result != oboe::Result::OK) {
      LOGW("Request start playout failed %d", static_cast<int>(result));
      return -1;
    }
  }
  LOGI("Frames per burst %d in player", audio_stream_->getFramesPerBurst());

  playing_ = true;

  return 0;
}

int OboePlayer::StopPlayout() {
  std::lock_guard<std::mutex> lk(lock_);
  if (!audio_stream_) {
    LOGW("Playout audio stream has not been created");
    return -1;
  }
  auto recorder_state = audio_stream_->getState();
  LOGI("Player state %d in stop playout", static_cast<int>(recorder_state));
  if (recorder_state < oboe::StreamState::Stopping) {
    auto result = audio_stream_->requestStop();
    if (result != oboe::Result::OK) {
      LOGW("Request stop playout failed %d", static_cast<int>(result));
      return -1;
    }
  }

  playing_ = false;
  return 0;
}

int OboePlayer::SpeakerVolumeIsAvailable(bool &available) { return 0; }

int OboePlayer::SetSpeakerVolume(uint32_t volume) { return 0; }

int OboePlayer::SpeakerVolume(uint32_t &volume) const { return 0; }

int OboePlayer::MaxSpeakerVolume(uint32_t &maxVolume) const { return 0; }

int OboePlayer::MinSpeakerVolume(uint32_t &minVolume) const { return 0; }

int OboePlayer::ProcessPlayoutDataRequest(oboe::AudioStream *oboe_stream, void *audio_data,
                                          int32_t num_frames) {
  if (oboe_stream != audio_stream_.get()) {
    LOGW("ProcessRecordedData: invalid audio stream");
  }

  if (audio_device_buffer_) {
    audio_device_buffer_->RequestPlayoutData(num_frames / playout_parameters_.channels_);
    audio_device_buffer_->GetPlayoutData(audio_data);
  }

  return 0;
}

int OboePlayer::ProcessPlayoutError(oboe::AudioStream *oboe_stream, oboe::Result error) {
  return 0;
}