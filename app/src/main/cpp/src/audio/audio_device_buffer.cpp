//
// Created by hanpfei on 20-10-21.
//

#include "audio_device_buffer.h"

#include "android_debug.h"

AudioDeviceBuffer::AudioDeviceBuffer() :
        audio_transport_cb_(nullptr),
        rec_sample_rate_(0),
        play_sample_rate_(0),
        rec_channels_(0),
        play_channels_(0),
        playing_(0),
        recording_(0),
        recorded_data_buffer_(nullptr),
        recorded_data_pos_(0) {
}

AudioDeviceBuffer::~AudioDeviceBuffer() {
}

int32_t AudioDeviceBuffer::RegisterAudioCallback(AudioTransport* audio_callback) {
  std::lock_guard<std::mutex> lk(lock_);
  if (playing_ || recording_) {
    LOGE("Failed to set audio transport since media was active");
    return -1;
  }
  audio_transport_cb_ = audio_callback;
  return 0;
}

void AudioDeviceBuffer::StartPlayout() {
  std::lock_guard<std::mutex> lk(lock_);
  if (playing_) {
    return;
  }

  playing_ = true;
}

void AudioDeviceBuffer::StartRecording() {
  std::lock_guard<std::mutex> lk(lock_);
  if (recording_) {
    return;
  }
  recorded_data_buffer_.reset(new int16_t[kRecordedDataBufferLength]);

  recording_ = true;
}

void AudioDeviceBuffer::StopPlayout() {
  std::lock_guard<std::mutex> lk(lock_);
  if (!playing_) {
    return;
  }

  playing_ = false;
}

void AudioDeviceBuffer::StopRecording() {
  std::lock_guard<std::mutex> lk(lock_);
  if (!recording_) {
    return;
  }

  recording_ = false;
}

int32_t AudioDeviceBuffer::SetRecordingSampleRate(uint32_t fsHz) {
  std::lock_guard<std::mutex> lk(lock_);
  rec_sample_rate_ = fsHz;
  return 0;
}

int32_t AudioDeviceBuffer::SetPlayoutSampleRate(uint32_t fsHz) {
  std::lock_guard<std::mutex> lk(lock_);
  play_sample_rate_ = fsHz;
  return 0;
}

int32_t AudioDeviceBuffer::RecordingSampleRate() const {
  std::lock_guard<std::mutex> lk(lock_);
  return rec_sample_rate_;
}

int32_t AudioDeviceBuffer::PlayoutSampleRate() const {
  std::lock_guard<std::mutex> lk(lock_);
  return play_sample_rate_;
}

int32_t AudioDeviceBuffer::SetRecordingChannels(size_t channels) {
  std::lock_guard<std::mutex> lk(lock_);
  rec_channels_ = channels;
  return 0;
}

int32_t AudioDeviceBuffer::SetPlayoutChannels(size_t channels) {
  std::lock_guard<std::mutex> lk(lock_);
  play_channels_ = channels;
  return 0;
}

size_t AudioDeviceBuffer::RecordingChannels() const {
  std::lock_guard<std::mutex> lk(lock_);
  return rec_channels_;
}

size_t AudioDeviceBuffer::PlayoutChannels() const {
  std::lock_guard<std::mutex> lk(lock_);
  return play_channels_;
}

int32_t AudioDeviceBuffer::SetRecordedBuffer(const void *audio_buffer,
                                             size_t samples_per_channel) {
  std::lock_guard<std::mutex> lk(lock_);
  if (rec_channels_ == 0) {
    LOGE("No recording channels initialized.");
    return -1;
  }

  if (!recorded_data_buffer_) {
    LOGE("No recording data buffer initialized.");
    return -1;
  }

  int64_t num_frames = samples_per_channel * rec_channels_;
  if (recorded_data_pos_ + num_frames > kRecordedDataBufferLength) {
    LOGE("No space left for recorded data.");
    return -1;
  }
  memcpy(recorded_data_buffer_.get() + recorded_data_pos_,
          audio_buffer, sizeof(int16_t) * num_frames);
  recorded_data_pos_ += num_frames;

  return 0;
}

int32_t AudioDeviceBuffer::DeliverRecordedData() {
  std::lock_guard<std::mutex> lk(lock_);
  if (rec_channels_ == 0 || rec_sample_rate_ == 0) {
    LOGE("No recording channels or sample rate initialized.");
    return -1;
  }

  if (!recorded_data_buffer_) {
    LOGE("No recording data buffer initialized.");
    return -1;
  }

  if (!audio_transport_cb_) {
    LOGE("No audio transport callback registered.");
    return -1;
  }

  if (recorded_data_pos_ == 0) {
    LOGE("No recoded data needed to deliver.");
    return -1;
  }

  uint32_t newMicLevel = 0;
  audio_transport_cb_->RecordedDataIsAvailable(recorded_data_buffer_.get(),
                                               recorded_data_pos_ / rec_channels_,
                                               sizeof(int16_t) * rec_channels_,
                                               rec_channels_,
                                               rec_sample_rate_,
                                               0,
                                               0,
                                               0,
                                               0,
                                               newMicLevel);

  recorded_data_pos_ = 0;

  return 0;
}

int32_t AudioDeviceBuffer::RequestPlayoutData(size_t samples_per_channel) {
  std::lock_guard<std::mutex> lk(lock_);
  return 0;
}

int32_t AudioDeviceBuffer::GetPlayoutData(void *audio_buffer) {
  std::lock_guard<std::mutex> lk(lock_);
  return 0;
}