#ifndef FFMPEGVIDEOPROCESSING_H
#define FFMPEGVIDEOPROCESSING_H

#include <memory>

#include "src/Thread/videoprocessingabstract.h"
#include "src/datastructure/threadsafe_queue.h"

struct AVFrame;

class FFMPEGVideoProcessing : public VideoProcessingAbstract
{
public:
    FFMPEGVideoProcessing();
    ~FFMPEGVideoProcessing(){}


    // VideoProcessingAbstract interface
public:
    bool open(AVCodecParameters *para);
    void start();
    void stop();
    void clearQueue();

    threadsafe_queue<AVPacket*> ffmpegVideoPkt;
    threadsafe_queue<std::pair<AVFrame*, int64_t>> ffmpegVideoFrame;
    threadsafe_queue<std::pair<std::vector<char>, int64_t>> ffmpegVideoRgbFrame;
private:
    struct impl;
    std::shared_ptr<impl> pimpl;
};

#endif // FFMPEGVIDEOPROCESSING_H
