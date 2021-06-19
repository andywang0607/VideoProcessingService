#include <iostream>

#include "ffmpegaudioprocessing.h"
#include "src/Decoder/ffmpegdecoder.h"
#include "src/Demuxer/ffmpegdemuxer.h"
#include "src/Resampler/ffmpegresampler.h"

extern "C" {
#include<libavcodec/avcodec.h>
}

using namespace std;

struct FFMPEGAudioProcessing::impl
{
    impl() :
        resampler(make_shared<FFMPEGResampler>())
      , decoder(make_shared<FFMPEGDecoder>())
    {

    }
    ~impl(){}
    shared_ptr<FFMPEGResampler> resampler;
    shared_ptr<FFMPEGDecoder> decoder;
};

FFMPEGAudioProcessing::FFMPEGAudioProcessing(): pimpl(make_shared<impl>())
{

}

bool FFMPEGAudioProcessing::open(AVCodecParameters *para, int channel, int sampleRate)
{
    if(!para) return false;
    bool ret = true;
    if (!pimpl->resampler->open(para->channels, para->sample_rate, (AudioSampleFormat)para->format, channel, sampleRate, AudioSampleFormat::SAMPLE_FMT_S32)) {
        cout << "Resampler open failed!" << endl;
        ret = false;
    }
    if(!pimpl->decoder->open(para, 2)) {
        cout << "audio Decoder open failed!" << endl;
        ret = false;
    }
    pts = 0;
    cout << "AudioProcessing::Open :" << ret << endl;
    return ret;
}

void FFMPEGAudioProcessing::start()
{
    isRunning.store(true);
    audioThread.emplace_back([&](){
        unsigned char *pcm = new unsigned char[1024 * 1024 * 10];
        while(isRunning) {
            auto pkt = ffmpegAudioPkt.try_pop();
            if (!pkt) {
                this_thread::sleep_for(chrono::milliseconds(1));
                continue;
            }
            bool re = pimpl->decoder->push2DecodeQueue(*pkt);
            while (isRunning.load()) {
                AVFrame * frame = pimpl->decoder->getDecodeData();
                if (!frame) break;
                int size = pimpl->resampler->resample(frame, pcm);
                if (size <= 0) continue;
                vector<char> pcmVec(pcm, pcm+size);
                ffmpegPcmData.push(pcmVec);
                this_thread::sleep_for(chrono::milliseconds(10));
            }
            this_thread::sleep_for(chrono::milliseconds(10));
        }
        delete[] pcm;
        pcm = nullptr;
    });
}

void FFMPEGAudioProcessing::stop()
{
    isRunning.store(false);
    if(audioThread.empty()) return;
    for(auto &thread : audioThread) {
        thread.join();
    }
    audioThread.clear();
    pimpl->decoder->close();
    clearQueue();
}

void FFMPEGAudioProcessing::clearQueue()
{
    while(!ffmpegAudioPkt.empty()) {
        auto data = ffmpegAudioPkt.try_pop();
        if(data) {
            pimpl->decoder->freePacket(*data);
        }
    }
    while(!ffmpegPcmData.empty()) {
        ffmpegAudioPkt.try_pop();
    }
}
