cmake_minimum_required(VERSION 3.5)

if(WIN32)
    set(VCPKG_ROOT "D:/ThirdParty/vcpkg/" )
else()
    set(VCPKG_ROOT "/home/ubuntu/ThirdParty/vcpkg/" )
endif()
set( CMAKE_TOOLCHAIN_FILE ${VCPKG_ROOT}scripts/buildsystems/vcpkg.cmake )

project(VideoProcessingServiceTest LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    set(FFmpeg_DIR ${VCPKG_ROOT}installed/x64-windows/share/ffmpeg/ )
else()
    set(FFmpeg_DIR ${VCPKG_ROOT}installed/x64-linux/share/ffmpeg/)
endif()
find_package(FFMPEG COMPONENTS AVCODEC AVFORMAT AVUTIL SWSCALE SWRESAMPLE REQUIRED)
if( FFMPEG_FOUND )
    # Additional Include Directories
    include_directories( ${FFMPEG_INCLUDE_DIRS} )

    # Additional Library Directories
    link_directories( ${FFMPEG_LIBRARY_DIRS} )

endif()

set(OpenCV_DIR /usr/local/share/OpenCV)
find_package(OpenCV REQUIRED)

find_package(RtAudio CONFIG REQUIRED)


if( OpenCV_FOUND )
    # Additional Include Directories
    include_directories( ${OpenCV_INCLUDE_DIRS} )

    #    # Additional Library Directories
    link_directories( ${OpenCV_LIBRARY_DIRS} )
endif()
add_executable(VideoProcessingServiceTest
    ../src/Demuxer/ffmpegdemuxer.cpp
    ../src/Decoder/ffmpegdecoder.cpp
    ../src/Resampler/ffmpegresampler.cpp
    ../src/YUV2RGB/ffmpegcolorspaceconverter.cpp
    ../src/Thread/ffmpegvideoprocessing.cpp
    ../src/Thread/ffmpegaudioprocessing.cpp
    ../src/Facade/videoservice.cpp
    cppbackendtest.cpp
    )
target_link_libraries(VideoProcessingServiceTest
    ${FFMPEG_LIBRARIES}
    ${OpenCV_LIBS}
    RtAudio::rtaudio)
target_include_directories(VideoProcessingServiceTest PRIVATE ../)
