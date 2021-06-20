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

set(OpenCV_DIR /usr/local/share/OpenCV)
find_package(OpenCV REQUIRED)
find_package(RtAudio CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(restclient-cpp CONFIG REQUIRED)

if( OpenCV_FOUND )
    # Additional Include Directories
    include_directories( ${OpenCV_INCLUDE_DIRS} )

    #    # Additional Library Directories
    link_directories( ${OpenCV_LIBRARY_DIRS} )
endif()

project(VideoProcessingServiceRestServerTest LANGUAGES CXX)

add_executable(VideoProcessingServiceRestServerTest
    restservertest.cpp
    )
target_link_libraries(VideoProcessingServiceRestServerTest
    ${OpenCV_LIBS}
    nlohmann_json
    restclient-cpp
    RtAudio::rtaudio)
