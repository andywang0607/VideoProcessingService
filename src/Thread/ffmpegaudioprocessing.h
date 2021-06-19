#ifndef FFMPEGAUDIOPROCESSING_H
#define FFMPEGAUDIOPROCESSING_H

#include "audioprocessingabstract.h"
#include "src/datastructure/threadsafe_queue.h"

class FFMPEGAudioProcessing : public AudioProcessingAbstract
{
public:
    FFMPEGAudioProcessing();
    ~FFMPEGAudioProcessing(){}

    // AudioProcessingAbstract interface
public:
    bool open(AVCodecParameters *para, int channel, int sampleRate) override;
    void start() override;
    void stop() override;
    void clearQueue() override;

    // public member variable
    /// queue for storing demuxed data
    threadsafe_queue<AVPacket*> ffmpegAudioPkt;

    /// queue for stroing resample data
    threadsafe_queue<std::vector<char>> ffmpegPcmData;

private:
    struct impl;
    std::shared_ptr<impl> pimpl;
};

#endif // FFMPEGAUDIOPROCESSING_H
