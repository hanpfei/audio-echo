//
// Created by hanpfei on 20-10-21.
//

#ifndef NATIVE_AUDIO_AUDIO_DEVICE_DEFINES_H
#define NATIVE_AUDIO_AUDIO_DEVICE_DEFINES_H

#include <cstdint>

// define in
// https://developer.android.com/reference/android/media/MediaRecorder.AudioSource
const int kAndroidMediaRecorderAudioSourceDefault = 0;
const int kAndroidMediaRecorderAudioSourceMic = 1;
const int kAndroidMediaRecorderAudioSourceVoiceUplink = 2;
const int kAndroidMediaRecorderAudioSourceVoiceDownlink = 3;
const int kAndroidMediaRecorderAudioSourceVoiceCall = 4;
const int kAndroidMediaRecorderAudioSourceCamcorder = 5;
const int kAndroidMediaRecorderAudioSourceVoiceRecognition = 6;
const int kAndroidMediaRecorderAudioSourceVoiceCommunication = 7;

// ----------------------------------------------------------------------------
//  AudioTransport
// ----------------------------------------------------------------------------

class AudioTransport {
 public:
  virtual int32_t RecordedDataIsAvailable(const void* audioSamples,
                                          const size_t nSamples,
                                          const size_t nBytesPerSample,
                                          const size_t nChannels,
                                          const uint32_t samplesPerSec,
                                          const uint32_t totalDelayMS,
                                          const int32_t clockDrift,
                                          const uint32_t currentMicLevel,
                                          const bool keyPressed,
                                          uint32_t& newMicLevel) = 0;  // NOLINT

  // Implementation has to setup safe values for all specified out parameters.
  virtual int32_t NeedMorePlayData(const size_t nSamples,
                                   const size_t nBytesPerSample,
                                   const size_t nChannels,
                                   const uint32_t samplesPerSec,
                                   void* audioSamples,
                                   size_t& nSamplesOut,  // NOLINT
                                   int64_t* elapsed_time_ms,
                                   int64_t* ntp_time_ms) = 0;  // NOLINT

  // Method to pull mixed render audio data from all active VoE channels.
  // The data will not be passed as reference for audio processing internally.
  virtual void PullRenderData(int bits_per_sample,
                              int sample_rate,
                              size_t number_of_channels,
                              size_t number_of_frames,
                              void* audio_data,
                              int64_t* elapsed_time_ms,
                              int64_t* ntp_time_ms) = 0;
  virtual size_t send_num_channels() const { return 0; }
  virtual int send_sample_rate_hz() const { return 0; }

 public:
  virtual ~AudioTransport() {}
};

struct RecordParameters{
  uint32_t sample_rate_hz_;
  uint32_t channels_;
  uint32_t audio_source_;
};

struct PlayoutParameters {
  uint32_t sample_rate_hz_;
  uint32_t channels_;
  float playout_bufsize_factor_;
};

#endif //AUDIO_ECHO_AUDIO_DEVICE_DEFINES_H
