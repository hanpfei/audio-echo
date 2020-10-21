//
// Created by hanpfei on 20-10-21.
//

#ifndef NATIVE_AUDIO_AUDIO_DEVICE_DEFINES_H
#define NATIVE_AUDIO_AUDIO_DEVICE_DEFINES_H

#include <cstdint>

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
