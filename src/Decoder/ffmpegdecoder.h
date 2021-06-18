#ifndef FFMPEGDECODER_H
#define FFMPEGDECODER_H

#include <mutex>
#include <memory>

#include "decoderabstract.h"

class FFMPEGDecoder : public DecoderAbstract
{
public:
    FFMPEGDecoder();
    ~FFMPEGDecoder();

    // DecoderAbstract interface
    bool open(AVCodecParameters *para, int threadNum) override;
    bool push2DecodeQueue(AVPacket *pkt) override;
    AVFrame *getDecodeData() override;
    void close() override;

    void freePacket(AVPacket *pkt);
    void freeFrame(AVFrame *frame);

private:
    struct impl;
    std::shared_ptr<impl> pimpl;
    std::mutex mux;
};

#endif // FFMPEGDECODER_H
