#include <iostream>
#include <vector>
#include <chrono>
#include <atomic>
#include <thread>

#include "restclient-cpp/connection.h"
#include "restclient-cpp/restclient.h"

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"

#include "nlohmann/json.hpp"

#include "RtAudio.h"
#include "string.h"

using namespace std;
using json = nlohmann::json;

// video service

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
    json urlObject;
    urlObject["url"] = streamUrl;
    RestClient::Response r = RestClient::post("localhost:9080/open", "application/json", urlObject.dump());
    if(r.body == "Failed")
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

            RestClient::Response audio = RestClient::post("localhost:9080/getPcmData", "application/json", "getPcmData");
            if(audio.body == "null") {
                cout << "audioThread empty" << endl;
                continue;
            }
            // find size pos
            uint64_t sizePos = audio.body.rfind("pts:");
            cout << "sizePos" << sizePos << endl;

            string dataString = string(audio.body.begin(), audio.body.begin()+sizePos);
            audioPts = stoi(string(audio.body.begin()+sizePos+4, audio.body.end()));
            cout << "audioPts: " << audioPts << endl;
            auto dataSize = dataString.size() - 1;
            if(dataSize>0){
                pcmLength = dataSize;
                uint32_t tmp = (audioBufferSize/pcmLength) * pcmLength;
                if(tmp < maxPlayIdx)
                    maxPlayIdx = tmp ;
                memcpy(audioBuffer.data() + dataFlag, &dataString[0], dataSize);
                dataFlag.fetch_add(dataSize);
            }
            if(dataFlag>=maxPlayIdx) dataFlag.store(0);
        }
    });

    // start video service
    r = RestClient::post("localhost:9080/start", "application/json", "start");
    if(r.body == "Failed") {
        cout << "start service failed" << endl;
        return -3;
    }

    cout << "start service success" << endl;

    // start audio player
    try {
        rtAudio.startStream();
    }
    catch ( RtAudioError& e ) {
        e.printMessage();
        std::cout << "start rt audio failed" << endl;
        return -4;
    }

    // Get image width and height
    r = RestClient::post("localhost:9080/getParamInt", "text/plain", "0");
    int imageWidth = stoi(r.body);
    r = RestClient::post("localhost:9080/getParamInt", "text/plain", "1");
    int imageHeight =  stoi(r.body);
    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();

    cout << "width: " << imageWidth << "\t" << "height: " << imageHeight << endl;

    // Show image with opencv
    while (true) {
        RestClient::Response r = RestClient::post("localhost:9080/getVideoRGBFrame", "application/json", "getVideoRGBFrame");
        if(r.body != "null") {
            // find size pos
            uint64_t sizePos = r.body.rfind("pts:");
            cout << "video sizePos" << sizePos << endl;

            string dataString = string(r.body.begin(), r.body.begin()+sizePos);
            cv::Mat frame(imageHeight, imageWidth, CV_8UC3, &dataString[0]);
            uint64_t videoPts = stoi(string(r.body.begin()+sizePos+4, r.body.end()));
            if(videoPts>audioPts){
                this_thread::sleep_for(chrono::milliseconds(videoPts-audioPts));
            }
            cv::imshow( "window",  frame );
            cv::waitKey(30);
        } else {
            cout << "rgb data null" << endl;
            continue;
        }

        std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();

        if(std::chrono::duration_cast<std::chrono::seconds>(t2 - t1).count() > 30){
            cv::destroyWindow("window");
            break;
        }
    }

    // Stop service and audio player
    r = RestClient::post("localhost:9080/stop", "text/plain", "stop");
    rtAudio.stopStream();
    running = false;
    audioThread.join();
    return 0;
}
