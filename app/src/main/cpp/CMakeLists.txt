cmake_minimum_required(VERSION 3.4.1)
project(echo LANGUAGES C CXX)

add_library(echo
  SHARED
    src/audio/audio_common.cpp
    src/audio/audio_effect.cpp
    src/audio/opensles_player.cpp
    src/audio/opensles_recorder.cpp
    src/audio_jni.cpp
    src/debug_utils.cpp
    src/jni_helper.cpp)

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