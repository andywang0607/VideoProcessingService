#include <iostream>
#include <vector>
#include <chrono>
#include "../src/Facade/videoservice.h"

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"

#include "RtAudio.h"
#include "string.h"

using namespace std;

// video service
static VideoService service;

// rtaudio
static RtAudio rtAudio;
static RtAudio::StreamParameters parameters;

// record the current pts of audio
static uint64 audioPts = 0;

// lauch a array memory for storing audio data
static int audioBufferSize = 1024*128;
static std::vector<char> audioBuffer(audioBufferSize);

// flag of read position and insert position of audioBuffer
static atomic_uint32_t playIdx = 0;
static atomic_uint32_t dataFlag = 0;

// pcm length from getPcmData function
static atomic_uint32_t pcmLength = 0;

// The max value of playIdx and dataFlag
static atomic_uint32_t maxPlayIdx = audioBufferSize;

/// Callback function for rtaudio
static int callBack( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
                     double streamTime, RtAudioStreamStatus status, void *userData )
{
    if(playIdx==dataFlag) return 0;
    char* out = (char*)outputBuffer;
    memcpy(out, audioBuffer.data() + playIdx, pcmLength);
    playIdx.fetch_add(pcmLength);
    if(playIdx >= maxPlayIdx) playIdx.store(0);
    return 0;
}

int main(int argc, char *argv[])
{
    // test url
    std::string streamUrl = "https://www.rmp-streaming.com/media/big-buck-bunny-360p.mp4";
    // flag for stop get audio thread
    bool running = true;

    // Open video service
    bool ret = service.open(streamUrl.c_str());
    cout << "open: " << ret <<endl;
    if(!ret)
        return -1;

    // Open rt audio
    int deviceCount = rtAudio.getDeviceCount();
    if ( deviceCount== 0 ) {
        std::cout << "\nNo audio devices found!\n";
        return -2;
    }
    parameters.deviceId = 1;//rtAudio.getDefaultOutputDevice();
    parameters.nChannels = 2;
    parameters.firstChannel = 0;
    unsigned int bufferFrame = 1024;
    try {
        rtAudio.openStream( &parameters, NULL, RTAUDIO_FLOAT32,
                            48000, &bufferFrame, &callBack, nullptr);
    }
    catch ( RtAudioError& e ) {
        e.printMessage();
        std::cout << "open rt audio failed" << endl;
        return -3;
    }

    // A thread for get audio data from video service
    thread audioThread([&](){
        while (running) {
            if(playIdx>0 && dataFlag==0) continue;

            auto data = service.getPcmData();
            if(!data) {
                continue;
            }
            auto dataVec = (*data).first;
            audioPts = (*data).second;
            auto dataSize = dataVec.size();
            if(dataSize>0){
                pcmLength = dataSize;
                uint32_t tmp = (audioBufferSize/pcmLength) * pcmLength;
                if(tmp < maxPlayIdx)
                    maxPlayIdx = tmp ;
                memcpy(audioBuffer.data() + dataFlag, dataVec.data(), dataSize);
                dataFlag.fetch_add(dataSize);
            }
            if(dataFlag>=maxPlayIdx) dataFlag.store(0);
        }
    });

    // start video service
    service.start();

    // start audio player
    try {
        rtAudio.startStream();
    }
    catch ( RtAudioError& e ) {
        e.printMessage();
        std::cout << "start rt audio failed" << endl;
        return -4;
    }

    int imageWidth = service.getParameterInt(ParamInt::VideoWidth);
    int imageHeight = service.getParameterInt(ParamInt::VideoHeight);
    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();

    // Show image with opencv
    while (true) {
        auto data = service.getVideoRGBFrame();
        if(data) {
            cv::Mat frame(imageHeight, imageWidth, CV_8UC3, (*data).first.data());
            uint64_t videoPts = (*data).second;
            if(videoPts>audioPts){
                this_thread::sleep_for(chrono::milliseconds(videoPts-audioPts));
            }
            cv::imshow( "window",  frame );
        }
        std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();

        if(std::chrono::duration_cast<std::chrono::seconds>(t2 - t1).count() > 90){
            cv::destroyWindow("window");
            break;
        }
        cv::waitKey(30);
    }

    // Stop service and audio player
    service.stop();
    rtAudio.stopStream();
    running = false;
    audioThread.join();
    return 0;
}
