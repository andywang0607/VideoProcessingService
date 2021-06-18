cmake_minimum_required(VERSION 3.5)

if(WIN32)
    set(VCPKG_ROOT "D:/ThirdParty/vcpkg/" )
else()
    set(VCPKG_ROOT "/home/ubuntu/ThirdParty/vcpkg/" )
endif()
set( CMAKE_TOOLCHAIN_FILE ${VCPKG_ROOT}scripts/buildsystems/vcpkg.cmake )

project(VideoProcessingService LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
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

add_executable(VideoProcessingService
    src/Demuxer/ffmpegdemuxer.cpp
    main.cpp
    )
target_link_libraries(VideoProcessingService
    ${FFMPEG_LIBRARIES})
