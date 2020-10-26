//
// Created by hanpfei on 20-10-23.
//

#include "audio_echo_transport.h"

#include "android_debug.h"

AudioEchoTransport::AudioEchoTransport() :
        recording_channels_(0),
        recording_sample_rate_(0),
        data_pos_(0) {
}

AudioEchoTransport::~AudioEchoTransport() {
}

int32_t AudioEchoTransport::RecordedDataIsAvailable(const void *audioSamples,
                                                    const size_t nSamples, // samples per channel
                                                    const size_t nBytesPerSample,
                                                    const size_t nChannels,
                                                    const uint32_t samplesPerSec,
                                                    const uint32_t totalDelayMS,
                                                    const int32_t clockDrift,
                                                    const uint32_t currentMicLevel,
                                                    const bool keyPressed,
                                                    uint32_t &newMicLevel) {
  if (recording_channels_ == 0 || recording_sample_rate_ == 0) {
    recording_channels_ = nChannels;
    recording_sample_rate_ = samplesPerSec;
    if (recording_channels_ != 0 && recording_sample_rate_ != 0) {
      data_buffer_.reset(new int16_t[kRecordedDataBufferLength]);
    }
  }

  if (data_buffer_) {
    int64_t num_frames = nSamples * nChannels;
    if (data_pos_ + num_frames > kRecordedDataBufferLength) {
      LOGE("No space left for recorded data.");
      return -1;
    }

    memcpy(data_buffer_.get() + data_pos_, audioSamples, sizeof(int16_t) * num_frames);
    data_pos_ += num_frames;
  }

  return 0;
}

// Implementation has to setup safe values for all specified out parameters.
int32_t AudioEchoTransport::NeedMorePlayData(const size_t nSamples,
                                             const size_t nBytesPerSample,
                                             const size_t nChannels,
                                             const uint32_t samplesPerSec,
                                             void *audioSamples,
                                             size_t &nSamplesOut,  // NOLINT
                                             int64_t *elapsed_time_ms,
                                             int64_t *ntp_time_ms) {
  nSamplesOut = 0;
  int64_t num_frames = nSamples * nChannels;
  memset(audioSamples, 0, sizeof(int16_t) * num_frames);
  if (data_buffer_ && data_pos_ > num_frames) {
    nSamplesOut = num_frames;
    memcpy(audioSamples, data_buffer_.get(), sizeof(int16_t) * num_frames);
    memmove(data_buffer_.get(), data_buffer_.get() + num_frames,
            sizeof(int16_t) * (data_pos_ - num_frames));
    data_pos_ -= num_frames;

    return 0;
  } else {
    LOGW("No audio data to play.");
  }
  return -1;
}

// Method to pull mixed render audio data from all active VoE channels.
// The data will not be passed as reference for audio processing internally.
void AudioEchoTransport::PullRenderData(int bits_per_sample,
                                        int sample_rate,
                                        size_t number_of_channels,
                                        size_t number_of_frames,
                                        void *audio_data,
                                        int64_t *elapsed_time_ms,
                                        int64_t *ntp_time_ms) {

}

size_t AudioEchoTransport::send_num_channels() const {
  return recording_channels_;
}

int AudioEchoTransport::send_sample_rate_hz() const {
  return recording_sample_rate_;
}