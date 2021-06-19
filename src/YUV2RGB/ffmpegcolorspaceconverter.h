#ifndef FFMPEGCOLORSPACECONVERTER_H
#define FFMPEGCOLORSPACECONVERTER_H

#include <memory>

#include "colorspaceconverterabstract.h"

class FFMPEGColorSpaceConverter : public ColorSpaceConverterAbstract
{
public:
    FFMPEGColorSpaceConverter();
    ~FFMPEGColorSpaceConverter(){}

    // ColorSpaceConverterAbstract interface
public:
    void initParam(int width, int height, int format) override;
    std::vector<char> yuv2Rgb(AVFrame *frame) override;
    void close() override;

private:
    struct impl;
    std::shared_ptr<impl> pimpl;

};

#endif // FFMPEGCOLORSPACECONVERTER_H
