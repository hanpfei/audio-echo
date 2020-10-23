//
// Created by hanpfei on 20-10-21.
//

#ifndef NATIVE_AUDIO_OBOERECORDER_H
#define NATIVE_AUDIO_OBOERECORDER_H

#include <mutex>
#include <oboe/Oboe.h>

#include "audio/audio_device_defines.h"

class AudioDeviceBuffer;
class RecorderCallback;

class OboeRecorder {
 public:
  OboeRecorder();
  ~OboeRecorder();

  int Init();
  int Terminate();
  int InitRecording();
  bool RecordingIsInitialized() const { return initialized_; }
  int StartRecording();
  int StopRecording();
  bool Recording() const { return recording_; }

  int EnableBuiltInAEC(bool enable);
  int EnableBuiltInAGC(bool enable);
  int EnableBuiltInNS(bool enable);
  int SetRecordParameters(const RecordParameters& params);

  void AttachAudioBuffer(std::shared_ptr<AudioDeviceBuffer> audio_buffer);

 private:
  oboe::InputPreset GetInputPresetFromAudioSource(uint32_t audio_source);
  int ProcessRecordedData(oboe::AudioStream *oboe_stream, void *audio_data,
                          int32_t num_frames);
  int ProcessRecordingError(oboe::AudioStream *oboe_stream, oboe::Result error);

 private:
  friend class RecorderCallback;
  static const int kDefaultSampleRate = 48000;
  static const int kDefaultChannels = 1;

  mutable std::mutex lock_;
  std::shared_ptr<AudioDeviceBuffer> audio_device_buffer_;
  RecordParameters record_parameters_;
  std::unique_ptr<RecorderCallback> recorder_callback_;
  std::unique_ptr<oboe::AudioStreamBuilder> stream_builder_;
  oboe::ManagedStream audio_stream_;

  bool initialized_;
  bool recording_;
};

#endif //NATIVE_AUDIO_OBOERECORDER_H
