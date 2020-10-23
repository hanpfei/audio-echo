cmake_minimum_required(VERSION 3.4.1)
project(echo LANGUAGES C CXX)

add_library(echo
  SHARED
    src/audio/oboe_audio/oboe_player.cpp
    src/audio/oboe_audio/oboe_player.h
    src/audio/oboe_audio/oboe_recorder.cpp
    src/audio/oboe_audio/oboe_recorder.h
    src/audio/audio_common.cpp
    src/audio/audio_common.h
    src/audio/audio_device_buffer.cpp
    src/audio/audio_device_buffer.h
    src/audio/audio_device_defines.h
    src/audio/audio_effect.cpp
    src/audio/audio_effect.h
    src/audio/buf_manager.h
    src/audio/opensles_player.cpp
    src/audio/opensles_player.h
    src/audio/opensles_recorder.cpp
    src/audio/opensles_recorder.h
    src/audio/wav_header.cpp
    src/audio/wav_header.h
    src/audio/wav_pcm_file_writer.cpp
    src/audio/wav_pcm_file_writer.h
    src/android_debug.h
    src/audio_jni.cpp
    src/debug_utils.cpp
    src/debug_utils.h
    src/jni_helper.cpp
    src/jni_helper.h
    src/wav_recorded_data_writer_audio_transport.cpp
    src/wav_recorded_data_writer_audio_transport.h)

#include libraries needed for echo lib
target_link_libraries(echo
  PRIVATE
    OpenSLES
    android
    log
    atomic
    oboe)

target_compile_options(echo
  PRIVATE
    -Wall -Werror)

# Set the path to the Oboe directory.
set (OBOE_DIR ./oboe)

# Add the Oboe library as a subdirectory in your project.
# add_subdirectory tells CMake to look in this directory to
# compile oboe source files using oboe's CMake file.
# ./oboe specifies where the compiled binaries will be stored
add_subdirectory (${OBOE_DIR} ./oboe)

# Specify the path to the Oboe header files.
# This allows targets compiled with this CMake (application code)
# to see public Oboe headers, in order to access its API.
include_directories (./src ${OBOE_DIR}/include)