#include <iostream>

#include "ffmpegresampler.h"

extern "C" {
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
}

using namespace std;

struct FFMPEGResampler::impl
{
    std::mutex mux;
    SwrContext *swrCtx = nullptr;
};

FFMPEGResampler::FFMPEGResampler(): pimpl(std::make_shared<impl>())
{

}

bool FFMPEGResampler::open(int channelIn, int sampleRateIn, AudioSampleFormat formatIn, int channelOut, int sampleRateOut, AudioSampleFormat formatOut)
{
    lock_guard<std::mutex> lck (pimpl->mux);

    // Update member variable
    this->sampleRateOut = sampleRateOut;
    this->channelOut = channelOut;
    this->formatOut = formatOut;

    // Allocate
    pimpl->swrCtx = swr_alloc_set_opts(pimpl->swrCtx,
                                       av_get_default_channel_layout(channelOut),     // out channel num
                                       (AVSampleFormat)formatOut,                     // out sample format
                                       sampleRateOut,                                 // out sampling rate
                                       av_get_default_channel_layout(channelIn),      // in channle numʽ
                                       (AVSampleFormat)formatIn,                      // in sample format
                                       sampleRateIn,                                  // in sampling rate
                                       0, 0
                                       );
    int re = swr_init(pimpl->swrCtx);
    if (re != 0) {
        char buf[1024] = { 0 };
        av_strerror(re, buf, sizeof(buf) - 1);
        cout << "swr_init  failed! :" << buf << endl;
        return false;
    }
    return true;
}

void FFMPEGResampler::close()
{
    lock_guard<std::mutex> lck (pimpl->mux);
    if (pimpl->swrCtx)
        swr_free(&pimpl->swrCtx);

}

int FFMPEGResampler::resample(AVFrame *indata, unsigned char *data)
{
    if (!indata)
        return 0;
    if (!data) {
        av_frame_free(&indata);
        return 0;
    }
    uint8_t *dataTmp[2] = { 0 };
    dataTmp[0] = data;
    int re = swr_convert(pimpl->swrCtx,
                         dataTmp,                           // out data
                         indata->nb_samples,                // out count
                         (const uint8_t**)indata->data,     // in data
                         indata->nb_samples                 // in count
                         );

    if (re <= 0)
        return re;
    int outSize = re * indata->channels * av_get_bytes_per_sample((AVSampleFormat)formatOut);
    return outSize;
}
