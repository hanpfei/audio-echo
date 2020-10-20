**iolib**
==========
Classes for playing src.audio data.

## Abstract
(Oboe) **iolib** contains classes implementing streaming src.audio playback and mixing from multiple sources. It's purpose is to demonstrate best practices and provide reusable code.

 Note: the more general name of "iolib" was chosen  since it is presumed that this project will eventually implement src.audio capture capability.

**iolib** is written in C++ and is intended to be called from Android native code. It is implemented as a static library.

# **iolib** project structure
* player
Contains classes to support streaming playback from (potentially) multiple src.audio sources.

## player classes
### DataSource
Declares the basic interface for src.audio data sources.

### SampleSource
Extends the `DataSource` interface for src.audio data coming from SampleBuffer objects.

### OneShotSampleSource
Extends `SampleSource` to provide data that plays through it's `SampleBuffer` and then provides silence, (i.e. a non-looping sample)

### SampleBuffer
Loads and holds (in memory) src.audio sample data and provides read-only access to that data.

### SimpleMultiPlayer
Implements an Oboe src.audio stream into which it mixes src.audio from some number of `SampleSource`s.

This class demonstrates:
* Creation and lifetime management of an Oboe src.audio stream (`ManagedStream`)
* Logic for an Oboe `AudioStreamCallback` interface.
* Logic for handling streaming restart on error (i.e. playback device changes)
