//
// Created by hanpfei on 20-10-21.
//

#ifndef NATIVE_AUDIO_AUDIODEVICEBUFFER_H
#define NATIVE_AUDIO_AUDIODEVICEBUFFER_H

#include <cstdint>
#include <cstring>
#include <mutex>

#include "audio_device_defines.h"

class AudioDeviceBuffer {
 public:
  AudioDeviceBuffer();
  virtual ~AudioDeviceBuffer();

  int32_t RegisterAudioCallback(AudioTransport* audio_callback);

  void StartPlayout();
  void StartRecording();
  void StopPlayout();
  void StopRecording();

  int32_t SetRecordingSampleRate(uint32_t fsHz);
  int32_t SetPlayoutSampleRate(uint32_t fsHz);
  int32_t RecordingSampleRate() const;
  int32_t PlayoutSampleRate() const;

  int32_t SetRecordingChannels(size_t channels);
  int32_t SetPlayoutChannels(size_t channels);
  size_t RecordingChannels() const;
  size_t PlayoutChannels() const;

  virtual int32_t SetRecordedBuffer(const void* audio_buffer,
                                    size_t samples_per_channel);
  virtual int32_t DeliverRecordedData();

  virtual int32_t RequestPlayoutData(size_t samples_per_channel);
  virtual int32_t GetPlayoutData(void* audio_buffer);

 private:
  static const int64_t kRecordedDataBufferLength = 32 * 1024;
  mutable std::mutex lock_;

  AudioTransport* audio_transport_cb_;

  // Sample rate in Hertz.
  uint32_t rec_sample_rate_;
  uint32_t play_sample_rate_;

  // Number of audio channels.
  size_t rec_channels_;
  size_t play_channels_;

  bool playing_ ;
  bool recording_ ;

  std::unique_ptr<int16_t[]> recorded_data_buffer_;
  int64_t recorded_data_pos_;

  int64_t playout_request_sample_per_channel_;
};

#endif //NATIVE_AUDIO_AUDIODEVICEBUFFER_H
