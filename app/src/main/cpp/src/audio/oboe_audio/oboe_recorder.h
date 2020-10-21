//
// Created by hanpfei on 20-10-21.
//

#ifndef NATIVE_AUDIO_OBOERECORDER_H
#define NATIVE_AUDIO_OBOERECORDER_H

#include <oboe/Oboe.h>

#include "audio/audio_device_defines.h"

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
  int SetRecordParameters(RecordParameters* params);

 private:
  std::unique_ptr<RecorderCallback> recorder_callback_;
  oboe::ManagedStream audio_stream_;

  bool initialized_;
  bool recording_;
};


#endif //NATIVE_AUDIO_OBOERECORDER_H
