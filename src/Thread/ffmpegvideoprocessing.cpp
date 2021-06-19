#include "ffmpegvideoprocessing.h"
#include "src/Demuxer/ffmpegdemuxer.h"
#include "src/Decoder/ffmpegdecoder.h"
#include "src/YUV2RGB/ffmpegcolorspaceconverter.h"

extern "C" {
#include<libavcodec/avcodec.h>
}

struct FFMPEGVideoProcessing::impl
{
    impl() :
        decoder(std::make_shared<FFMPEGDecoder>())
      , colorSpaceConverter(std::make_shared<FFMPEGColorSpaceConverter>())
    {

    }
    ~impl()
    {

    }
    std::shared_ptr<FFMPEGDecoder> decoder;
    std::shared_ptr<FFMPEGColorSpaceConverter> colorSpaceConverter;
};

FFMPEGVideoProcessing::FFMPEGVideoProcessing(): pimpl(std::make_shared<impl>())
{

}

bool FFMPEGVideoProcessing::open(AVCodecParameters *para)
{
    if(!para)
        return false;
    bool res = pimpl->decoder->open(para, 2);
    pimpl->colorSpaceConverter->initParam(para->width, para->height, para->format);
    if(!res)
        return false;
    synpts = 0;
    return true;
}

void FFMPEGVideoProcessing::start()
{
    stop();
    isRunning.store(true);
    videoThread.emplace_back([&](){
        while(isRunning) {
            auto pkt = ffmpegVideoPkt.try_pop();
            if (pkt) {
                if(!pimpl->decoder->push2DecodeQueue(*pkt)) continue;
                while(true){
                    AVFrame *frame = pimpl->decoder->getDecodeData();
                    if(!frame) break;
                    auto rgb = pimpl->colorSpaceConverter->yuv2Rgb(frame);
                    ffmpegVideoRgbFrame.push({rgb, pimpl->decoder->pts});
                }
            }
        }
    });
}

void FFMPEGVideoProcessing::stop()
{
    isRunning.store(false);
    if(videoThread.empty()) return;
    for(auto &thread : videoThread) {
        thread.join();
    }
    videoThread.clear();
    pimpl->decoder->close();
}

void FFMPEGVideoProcessing::clearQueue()
{
    while(!ffmpegVideoPkt.empty()) {
        std::shared_ptr<AVPacket*> data = ffmpegVideoPkt.try_pop();
        if(data) {
            pimpl->decoder->freePacket(*data);
        }
    }
    while(!ffmpegVideoFrame.empty()) {
        std::shared_ptr<std::pair<AVFrame*, int64_t>> data = ffmpegVideoFrame.try_pop();
        if(data) {
            pimpl->decoder->freeFrame((*data).first);
        }
    }
}
