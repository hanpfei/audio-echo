//
// Created by hanpfei on 20-10-23.
//

#ifndef NATIVE_AUDIO_WAV_RECORDED_DATA_WRITER_AUDIO_TRANSPORT_H
#define NATIVE_AUDIO_WAV_RECORDED_DATA_WRITER_AUDIO_TRANSPORT_H

#include <string>

#include "audio/audio_device_defines.h"
#include "audio/wav_pcm_file_writer.h"

class WavRecordedDataWriterAudioTransport : public AudioTransport {
 public:
  explicit WavRecordedDataWriterAudioTransport(const std::string &file_path);
  ~WavRecordedDataWriterAudioTransport() override;

  int32_t RecordedDataIsAvailable(const void *audioSamples,
                                  const size_t nSamples,
                                  const size_t nBytesPerSample,
                                  const size_t nChannels,
                                  const uint32_t samplesPerSec,
                                  const uint32_t totalDelayMS,
                                  const int32_t clockDrift,
                                  const uint32_t currentMicLevel,
                                  const bool keyPressed,
                                  uint32_t &newMicLevel) override;

  // Implementation has to setup safe values for all specified out parameters.
  int32_t NeedMorePlayData(const size_t nSamples,
                           const size_t nBytesPerSample,
                           const size_t nChannels,
                           const uint32_t samplesPerSec,
                           void *audioSamples,
                           size_t &nSamplesOut,  // NOLINT
                           int64_t *elapsed_time_ms,
                           int64_t *ntp_time_ms) override;

  // Method to pull mixed render audio data from all active VoE channels.
  // The data will not be passed as reference for audio processing internally.
  void PullRenderData(int bits_per_sample,
                      int sample_rate,
                      size_t number_of_channels,
                      size_t number_of_frames,
                      void *audio_data,
                      int64_t *elapsed_time_ms,
                      int64_t *ntp_time_ms) override;

  size_t send_num_channels() const override;

  int send_sample_rate_hz() const override;

 private:
  const std::string file_path_;
  size_t recording_channels_;
  uint32_t recording_sample_rate_;
  std::unique_ptr<WavPcmFileWriter> file_writer_;
};

#endif //NATIVE_AUDIO_WAV_RECORDED_DATA_WRITER_AUDIO_TRANSPORT_H
