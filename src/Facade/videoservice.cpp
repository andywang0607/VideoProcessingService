#include <iostream>

#include "videoservice.h"

using namespace std;

VideoService::VideoService():
    vp(std::make_shared<FFMPEGVideoProcessing>())
  , ap(std::make_shared<FFMPEGAudioProcessing>())
  , demuxer(std::make_shared<FFMPEGDemuxer>())
{

}

VideoService::~VideoService()
{

}

bool VideoService::open(const char *url)
{
    // Open required module
    bool res = demuxer->open(url);
    if(!res) {
        cout << "VideoService open demuxer failed" << endl;
        return false;
    }
    res = vp->open(demuxer->getVideoCodecParameter());
    if(!res) {
        cout << "VideoService open video process failed" << endl;
        return false;
    }
    res = ap->open(demuxer->getAudioCodecParameter(), this->channelNum, this->sampleRate);
    if(!res) {
        cout << "VideoService open audio process failed" << endl;
        return false;
    }

    // Get information from demuxer
    width = demuxer->width;
    height = demuxer->height;
    totalMs = demuxer->totalMs;
    return true;
}

void VideoService::start()
{
    if(vp) vp->start();
    if(ap) ap->start();
    isRunning.store(true);
    controlThread.emplace_back([&](){
        while (isRunning.load()) {
            AVPacket * pkt = demuxer->read();
            if(!pkt){
                continue;
            }
            if(demuxer->isAudio(pkt)){
                ap->ffmpegAudioPkt.push(pkt);
            } else {
                vp->ffmpegVideoPkt.push(pkt);
            }
        }
    });
}

void VideoService::stop()
{
    cout << "VideoService stop start" << endl;
    if(vp) vp->stop();
    cout << "VideoService vp stop finish" << endl;
    if(ap) ap->stop();
    cout << "VideoService ap stop finish" << endl;

    if(controlThread.empty()) return;
    isRunning.store(false);
    for(auto &thread:controlThread) {
        thread.join();
    }
    controlThread.clear();
    if(demuxer) demuxer->close();
    cout << "VideoService demuxer close finish" << endl;
    cout << "VideoService stop finish" << endl;
}

void VideoService::seek(double pos)
{
    vp->clearQueue();
    ap->clearQueue();
    demuxer->seek(pos);
}

std::shared_ptr<std::pair<std::vector<char>, int64_t>> VideoService::getVideoRGBFrame()
{
    if(!vp)
        return nullptr;
    return vp->ffmpegVideoRgbFrame.try_pop();
}

std::shared_ptr<std::pair<std::vector<char>, int64_t> > VideoService::getPcmData()
{
    if(!ap)
        return nullptr;
    return ap->ffmpegPcmData.try_pop();
}

uint64_t VideoService::getParameterInt(ParamInt param)
{
    switch (param) {
    case ParamInt::VideoWidth: {
        return this->width;
    }
    case ParamInt::VideoHeight:{
        return this->height;
    }
    case ParamInt::VideoLength: {
        return this->totalMs;
    }
    default:
        return 0;
    }
    return 0;
}

