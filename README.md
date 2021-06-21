# VideoProcessingService
A c++ backend rest server provide a simple api for client to demux and docde network video stream or video file 

## Description
VideoProcessingService is a rest server aim to provide a simple api for application or web developer to get information or data from a video streaming(RTSP, RTMP, HLS, etc.) or a video file, developers can get the following information and data from this service
- Video 
    - Image width and height
    - Video length with milliseconds
    - RGB data and its related pts
- Audio
    - PCM raw data and its related pts

## Feature
The following module have been implement and integrate to the processing flow
- Demux [(API)](src/Demuxer/demuxerabstract.h)
- Decode [(API)](src/Decoder/decoderabstract.h)
- Color space converter [(API)](src/YUV2RGB/colorspaceconverterabstract.h)
- Audio resample [(API)](src/Resampler/resamplerabstract.h)


## Example
- [Demo file](example/restservertest.cpp)  
Provide a sample demostrate how to get video data and information from this service and showing image with OpenCV and playback audio with RtAudio

- [Test file](test/cppbackendtest.cpp)  
Provide a sample demostrate how to get video data and information directly from C++ backend and showing image with OpenCV and playback audio with RtAudio (This method do not need the reset server, but need to compile all c++ backend and integrate to your project)

- Todo
    - Plugin for Qt
    - Plugin for flutter

## How to build this server
1. Prepare third-party library
    - Setup vcpkg
      ```
      $ git clone https://github.com/microsoft/vcpkg
      $ cd vcpkg
      $ bootstrap-vcpkg.bat
      ```
    - Install required library
      ```
      $ ./vcpkg install ffmpeg:x64-linux
      $ ./vcpkg install pistache:x64-linux
      $ ./vcpkg install nlohmann-json:x64-linux
      ```

2. Build this project
    ```
    $ cd $PROJECTPATH
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    ```

## Note
I start this project just for practice build a rest service with c++.  
Get decode video and audio data from this service is not a efficent methoud indeed,  
But it is quite **simple** and **easy** to write a video player in every ui framework
