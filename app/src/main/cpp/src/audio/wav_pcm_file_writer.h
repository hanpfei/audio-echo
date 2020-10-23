
#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string>

#include "audio/wav_header.h"

class WavPcmFileWriter {
 public:
  WavPcmFileWriter(const std::string& outputFilePath, size_t numberOfChannels,
                   uint32_t sampleRateHz);
  virtual ~WavPcmFileWriter();

  bool openWriter();
  bool writeAudioPcmFrame(const void* payload_data, size_t sampleCount, size_t bytesPerSample);
  void closeWriter();

 private:
  WavHeader createWAVHeader();

 private:
  std::string output_file_path_;
  size_t number_of_channels_;
  uint32_t sample_rate_hz_;
  WavHeader wav_header_;
  FILE* wav_file_;
  int64_t received_sample_count_;
};
