//
// Created by hanpfei on 20-10-21.
//

#ifndef NATIVE_AUDIO_OBOEPLAYER_H
#define NATIVE_AUDIO_OBOEPLAYER_H

#include <cstdint>
#include <mutex>
#include <oboe/Oboe.h>

#include "audio/audio_device_defines.h"

class AudioDeviceBuffer;

class PlayerCallback;

class OboePlayer {
 public:
  OboePlayer();

  ~OboePlayer();

  int Init();

  int Terminate();

  int InitPlayout();

  bool PlayoutIsInitialized() const { return initialized_; }

  int StartPlayout();

  int StopPlayout();

  bool Playing() const { return playing_; }

  int SpeakerVolumeIsAvailable(bool &available);

  int SetSpeakerVolume(uint32_t volume);

  int SpeakerVolume(uint32_t &volume) const;

  int MaxSpeakerVolume(uint32_t &maxVolume) const;

  int MinSpeakerVolume(uint32_t &minVolume) const;

  int32_t SetPlayoutParameters(const PlayoutParameters &params);

  void AttachAudioBuffer(std::shared_ptr<AudioDeviceBuffer> audioBuffer);

 private:
  int ProcessPlayoutDataRequest(oboe::AudioStream *oboe_stream, void *audio_data,
                                int32_t num_frames);
  int ProcessPlayoutError(oboe::AudioStream *oboe_stream, oboe::Result error);

 private:
  friend class PlayerCallback;

  static const int kDefaultSampleRate = 48000;
  static const int kDefaultChannels = 1;

  mutable std::mutex lock_;
  std::shared_ptr<AudioDeviceBuffer> audio_device_buffer_;
  PlayoutParameters playout_parameters_;
  std::unique_ptr<PlayerCallback> player_callback_;
  std::unique_ptr<oboe::AudioStreamBuilder> stream_builder_;
  oboe::ManagedStream audio_stream_;

  bool initialized_;
  bool playing_;
};

#endif //NATIVE_AUDIO_OBOEPLAYER_H
