#ifndef FFMPEGDEMUXER_H
#define FFMPEGDEMUXER_H

#include <mutex>
#include <atomic>
#include <thread>

#include "demuxerabstract.h"

class FFMPEGDemuxer : public DemuxerAbstract
{
public:
    FFMPEGDemuxer();

    // DemuxerAbstract interface
public:
    bool open(const char *url) override;

    AVCodecParameters *getVideoCodecParameter() override;
    AVCodecParameters *getAudioCodecParameter() override;
    bool seek(double pos) override;
    void close() override;
    AVPacket *read() override;
    bool isAudio(AVPacket *pkt) override;

private:
    AVFormatContext *ioContext = nullptr;
    std::mutex mtx;
    int videoStream = 0;
    int audioStream = 1;
};

#endif // FFMPEGDEMUXER_H
