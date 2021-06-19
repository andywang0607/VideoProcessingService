#include <iostream>
#include <mutex>

#include "ffmpegdecoder.h"

extern "C" {
#include<libavcodec/avcodec.h>
}

using namespace std;

struct FFMPEGDecoder::impl
{
    AVCodecContext *codec = nullptr;
    std::mutex mux;
};

FFMPEGDecoder::FFMPEGDecoder() : pimpl(std::make_shared<impl>())
{

}

FFMPEGDecoder::~FFMPEGDecoder()
{

}

bool FFMPEGDecoder::open(AVCodecParameters *para, int threadNum)
{
    close();
    lock_guard<std::mutex> lck (pimpl->mux);
    if (!para)
        return false;
    AVCodec *vcodec = avcodec_find_decoder(para->codec_id);
    if (!vcodec) {
        avcodec_parameters_free(&para);
        cout << "can't find the codec id " << para->codec_id << endl;
        return false;
    }
    cout << "find the AVCodec " << para->codec_id << endl;

    pimpl->codec = avcodec_alloc_context3(vcodec);

    avcodec_parameters_to_context(pimpl->codec, para);
    avcodec_parameters_free(&para);

    pimpl->codec->thread_count = threadNum;

    int re = avcodec_open2(pimpl->codec, 0, 0);
    if (re != 0) {
        avcodec_free_context(&pimpl->codec);
        char buf[1024] = { 0 };
        av_strerror(re, buf, sizeof(buf) - 1);
        cout << "avcodec_open2  failed! :" << buf << endl;
        return false;
    }
    cout << " avcodec_open2 success!" << endl;
    return true;
}

bool FFMPEGDecoder::push2DecodeQueue(AVPacket *pkt)
{
    if (!pkt || pkt->size <= 0 || !pkt->data)
        return false;
    pimpl->mux.lock();
    if (!pimpl->codec) {
        pimpl->mux.unlock();
        return false;
    }
    int re = avcodec_send_packet(pimpl->codec, pkt);
    pimpl->mux.unlock();
    av_packet_free(&pkt);
    if (re != 0)
        return false;
    return true;
}

AVFrame *FFMPEGDecoder::getDecodeData()
{
    pimpl->mux.lock();
    if (!pimpl->codec) {
        pimpl->mux.unlock();
        return nullptr;
    }
    AVFrame *frame = av_frame_alloc();
    int re = avcodec_receive_frame(pimpl->codec, frame);
    pimpl->mux.unlock();
    if (re != 0) {
        av_frame_free(&frame);
        return nullptr;
    }
    this->pts = frame->pts;
    cout << "["<<frame->linesize[0] << "] " << flush;
    return frame;
}

void FFMPEGDecoder::close()
{
    lock_guard<std::mutex> lck (pimpl->mux);
    if (pimpl->codec) {
        avcodec_flush_buffers(pimpl->codec);
        avcodec_close(pimpl->codec);
        avcodec_free_context(&pimpl->codec);
    }
}

void FFMPEGDecoder::freePacket(AVPacket *pkt)
{
    if(!pkt)
        return;
    av_packet_free(&pkt);
}

void FFMPEGDecoder::freeFrame(AVFrame *frame)
{
    if(!frame)
        return;
    av_frame_free(&frame);
}
