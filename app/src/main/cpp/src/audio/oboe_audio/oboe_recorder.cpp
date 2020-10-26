//
// Created by hanpfei on 20-10-21.
//

#include "oboe_recorder.h"

#include "android_debug.h"
#include "audio/audio_device_buffer.h"

class RecorderCallback : public oboe::AudioStreamCallback {
 public:
  explicit RecorderCallback(OboeRecorder *oboe_rcorder) :
          oboe_rcorder_(oboe_rcorder) {
  }

  virtual ~RecorderCallback() = default;

  oboe::DataCallbackResult onAudioReady(oboe::AudioStream *oboe_stream, void *audio_data,
                                        int32_t num_frames) override {
    if (oboe_rcorder_) {
      oboe_rcorder_->ProcessRecordedData(oboe_stream, audio_data, num_frames);
    }
    return oboe::DataCallbackResult::Continue;
  }

  void onErrorBeforeClose(oboe::AudioStream *oboe_stream, oboe::Result error) override {
    if (oboe_rcorder_) {
      oboe_rcorder_->ProcessRecordingError(oboe_stream, error);
    }
  }

  void onErrorAfterClose(oboe::AudioStream *oboe_stream, oboe::Result error) override {
    if (oboe_rcorder_) {
      oboe_rcorder_->ProcessRecordingError(oboe_stream, error);
    }
  }

 private:
  OboeRecorder *oboe_rcorder_;
};

OboeRecorder::OboeRecorder() :
        lock_(),
        initialized_(false),
        recording_(false) {
  record_parameters_.sample_rate_hz_ = kDefaultSampleRate;
  record_parameters_.channels_ = kDefaultChannels;
  record_parameters_.audio_source_ = 0;
}

OboeRecorder::~OboeRecorder() {
  Terminate();
}

int OboeRecorder::SetRecordParameters(const RecordParameters &params) {
  std::lock_guard<std::mutex> lk(lock_);
  record_parameters_ = params;
  return 0;
}

void OboeRecorder::AttachAudioBuffer(std::shared_ptr<AudioDeviceBuffer> audio_buffer) {
  std::lock_guard<std::mutex> lk(lock_);
  audio_device_buffer_ = audio_buffer;
  audio_device_buffer_->SetRecordingSampleRate(record_parameters_.sample_rate_hz_);
  audio_device_buffer_->SetRecordingChannels(record_parameters_.channels_);
}

int OboeRecorder::Init() {
  std::lock_guard<std::mutex> lk(lock_);
  if (!stream_builder_) {
    recorder_callback_ = std::make_unique<RecorderCallback>(this);

    stream_builder_ = std::make_unique<oboe::AudioStreamBuilder>();
    stream_builder_->setSharingMode(oboe::SharingMode::Shared)
            ->setPerformanceMode(oboe::PerformanceMode::LowLatency)
            ->setDirection(oboe::Direction::Input)
            ->setFormat(oboe::AudioFormat::I16)
            ->setCallback(recorder_callback_.get());
  }

  return 0;
}

int OboeRecorder::Terminate() {
  std::lock_guard<std::mutex> lk(lock_);
  if (audio_stream_) {
    audio_stream_->close();
    audio_stream_.reset();
  }

  stream_builder_.reset();
  recorder_callback_.reset();
  audio_device_buffer_.reset();

  initialized_ = false;

  return 0;
}

oboe::InputPreset OboeRecorder::GetInputPresetFromAudioSource(uint32_t audio_source) {
  oboe::InputPreset preset;
  switch (audio_source) {
    case kAndroidMediaRecorderAudioSourceDefault:
    case kAndroidMediaRecorderAudioSourceMic:
      preset = oboe::InputPreset::Generic;
      break;
    case kAndroidMediaRecorderAudioSourceCamcorder:
      preset = oboe::InputPreset::Unprocessed;
      break;
    case kAndroidMediaRecorderAudioSourceVoiceRecognition:
      preset = oboe::InputPreset::VoiceRecognition;
      break;
    case kAndroidMediaRecorderAudioSourceVoiceCommunication:
      preset = oboe::InputPreset::VoiceCommunication;
      break;
    default:
      preset = oboe::InputPreset::Generic;
      break;
  }
  return preset;
}

int OboeRecorder::InitRecording() {
  std::lock_guard<std::mutex> lk(lock_);
  if (initialized_) {
    LOGW("Oboe recording has initialized.");
    return -1;
  }
  if (!stream_builder_) {
    LOGW("Record audio stream builder has not been created.");
    return -1;
  }

  int32_t sample_rate = record_parameters_.sample_rate_hz_;
  int channels = record_parameters_.channels_;
  if (sample_rate == 0 || (channels != 1 && channels != 2)) {
    LOGW("Invalid recording parameters: sample rate %d, channels %d", sample_rate, channels);
    return -1;
  }

  if (audio_stream_) {
    LOGW("Oboe recorder has been initialized.");
    return -1;
  }

  if (channels == 1) {
    stream_builder_->setChannelCount(oboe::ChannelCount::Mono);
  } else if (channels == 2) {
    stream_builder_->setChannelCount(oboe::ChannelCount::Stereo);
  }
//  stream_builder_->setChannelConversionAllowed(false);
  stream_builder_->setSampleRate(sample_rate);
  stream_builder_->setFramesPerCallback(sample_rate / 100);
  stream_builder_->setUsage(oboe::Usage::VoiceCommunication);

  uint32_t audio_source = record_parameters_.audio_source_;
  stream_builder_->setInputPreset(GetInputPresetFromAudioSource(audio_source));

  if (stream_builder_->willUseAAudio()) {
    LOGI("Recording will use aaudio");
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

int OboeRecorder::StartRecording() {
  std::lock_guard<std::mutex> lk(lock_);
  if (recording_) {
    LOGW("Recording has been started.");
    return -1;
  }
  if (!audio_stream_) {
    LOGW("Recording audio stream has not been created");
    return -1;
  }
  auto recorder_state = audio_stream_->getState();
  LOGI("Recorder state %d in start recording", static_cast<int>(recorder_state));
  if (recorder_state >= oboe::StreamState::Open
      && (recorder_state != oboe::StreamState::Starting
          && recorder_state != oboe::StreamState::Started)
      && recorder_state < oboe::StreamState::Closing) {
    auto result = audio_stream_->requestStart();
    if (result != oboe::Result::OK) {
      LOGW("Request start recording failed %d", static_cast<int>(result));
      return -1;
    }
  }
  LOGI("Frames per burst %d in recorder", audio_stream_->getFramesPerBurst());

  recording_ = true;

  return 0;
}

int OboeRecorder::StopRecording() {
  std::lock_guard<std::mutex> lk(lock_);
  if (!audio_stream_) {
    LOGW("Recording audio stream has not been created");
    return -1;
  }
  auto recorder_state = audio_stream_->getState();
  LOGI("Recorder state %d in stop recording", static_cast<int>(recorder_state));
  if (recorder_state < oboe::StreamState::Stopping) {
    auto result = audio_stream_->requestStop();
    if (result != oboe::Result::OK) {
      LOGW("Request stop recording failed %d", static_cast<int>(result));
      return -1;
    }
  }

  recording_ = false;

  return 0;
}

int OboeRecorder::EnableBuiltInAEC(bool enable) { return 0; }

int OboeRecorder::EnableBuiltInAGC(bool enable) { return 0; }

int OboeRecorder::EnableBuiltInNS(bool enable) { return 0; }

int OboeRecorder::ProcessRecordedData(oboe::AudioStream *oboe_stream, void *audio_data,
                                      int32_t num_frames) {
  if (oboe_stream != audio_stream_.get()) {
    LOGW("ProcessRecordedData: invalid audio stream");
  }
  if (audio_device_buffer_) {
    audio_device_buffer_->SetRecordedBuffer(audio_data, num_frames / record_parameters_.channels_);
    audio_device_buffer_->DeliverRecordedData();
  }
  return 0;
}

int OboeRecorder::ProcessRecordingError(oboe::AudioStream *oboe_stream, oboe::Result error) {
  if (oboe_stream != audio_stream_.get()) {
    LOGW("ProcessRecordedData: invalid audio stream");
  }
  return 0;
}
