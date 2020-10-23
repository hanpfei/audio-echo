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

  int32_t SetPlayoutParameters(PlayoutParameters* params);
  void AttachAudioBuffer(std::shared_ptr<AudioDeviceBuffer> audioBuffer);

private:
  friend class PlayerCallback;
  std::unique_ptr<PlayerCallback> player_callback_;
  oboe::ManagedStream audio_stream_;

  bool initialized_;
  bool playing_;
};

#endif //NATIVE_AUDIO_OBOEPLAYER_H
