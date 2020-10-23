//
// Created by hanpfei on 20-10-23.
//

#include "wav_recorded_data_writer_audio_transport.h"

#include "android_debug.h"

WavRecordedDataWriterAudioTransport::WavRecordedDataWriterAudioTransport(
        const std::string &file_path) :
        file_path_(file_path),
        recording_channels_(0),
        recording_sample_rate_(0),
        file_writer_(nullptr) {
}

WavRecordedDataWriterAudioTransport::~WavRecordedDataWriterAudioTransport() {
  if (file_writer_) {
    file_writer_->closeWriter();
  }
}

int32_t WavRecordedDataWriterAudioTransport::RecordedDataIsAvailable(const void *audioSamples,
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
      auto file_writer = std::make_unique<WavPcmFileWriter>(file_path_, recording_channels_,
                                                            recording_sample_rate_);
      if (file_writer->openWriter()) {
        file_writer_ = std::move(file_writer);
      } else {
        LOGW("Create file writer failed: %s.", file_path_.c_str());
      }
    }
  }

  if (file_writer_) {
    file_writer_->writeAudioPcmFrame(audioSamples, nSamples * recording_channels_, nBytesPerSample);
  }

  return 0;
}

// Implementation has to setup safe values for all specified out parameters.
int32_t WavRecordedDataWriterAudioTransport::NeedMorePlayData(const size_t nSamples,
                                                              const size_t nBytesPerSample,
                                                              const size_t nChannels,
                                                              const uint32_t samplesPerSec,
                                                              void *audioSamples,
                                                              size_t &nSamplesOut,  // NOLINT
                                                              int64_t *elapsed_time_ms,
                                                              int64_t *ntp_time_ms) {
  return -1;
}

// Method to pull mixed render audio data from all active VoE channels.
// The data will not be passed as reference for audio processing internally.
void WavRecordedDataWriterAudioTransport::PullRenderData(int bits_per_sample,
                                                         int sample_rate,
                                                         size_t number_of_channels,
                                                         size_t number_of_frames,
                                                         void *audio_data,
                                                         int64_t *elapsed_time_ms,
                                                         int64_t *ntp_time_ms) {

}

size_t WavRecordedDataWriterAudioTransport::send_num_channels() const {
  return recording_channels_;
}

int WavRecordedDataWriterAudioTransport::send_sample_rate_hz() const {
  return recording_sample_rate_;
}