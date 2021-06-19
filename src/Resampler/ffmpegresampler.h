#ifndef FFMPEGRESAMPLER_H
#define FFMPEGRESAMPLER_H

#include "resamplerabstract.h"

#include <memory>
#include <mutex>

struct SwrContext;

class FFMPEGResampler : public ResamplerAbstract
{
public:
    FFMPEGResampler();

    // ResamplerAbstract interface
public:
    bool open(int channelIn, int sampleRateIn, AudioSampleFormat formatIn, int channelOut, int sampleRateOut, AudioSampleFormat formatOut);
    void close() override;
    int resample(AVFrame *indata, unsigned char *data) override;

private:
    struct impl;
    std::shared_ptr<impl> pimpl;

};

#endif // FFMPEGRESAMPLER_H
