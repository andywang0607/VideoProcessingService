#include "ffmpegcolorspaceconverter.h"

extern "C" {
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
}


struct FFMPEGColorSpaceConverter::impl
{
    impl(){}
    ~impl()
    {
        if (swsContext)
        {
            sws_freeContext(swsContext);
            swsContext = nullptr;
        }
    }
    // Context for size and format transformation
    SwsContext *swsContext = nullptr;

    // video param
    int videoWidth = 0;
    int videoHeight = 0;

};

FFMPEGColorSpaceConverter::FFMPEGColorSpaceConverter(): pimpl(std::make_shared<impl>())
{

}

void FFMPEGColorSpaceConverter::initParam(int width, int height, int format)
{
    // Store width and height value
    pimpl->videoWidth = width;
    pimpl->videoHeight = height;

    // Init swsContext
    pimpl->swsContext = sws_getCachedContext(pimpl->swsContext,
                                             width, height, (AVPixelFormat)format,
                                             width, height, AV_PIX_FMT_BGR24,
                                             SWS_BICUBIC,
                                             0, 0, 0
                                             );
}

std::vector<char> FFMPEGColorSpaceConverter::yuv2Rgb(AVFrame *frame)
{
    AVFrame *video_frameBGR = NULL;
    video_frameBGR = av_frame_alloc();
    uint8_t *outBuff = nullptr;

    int frameSize = av_image_get_buffer_size(AV_PIX_FMT_BGR24, pimpl->videoWidth, pimpl->videoHeight, 1);
    outBuff = (uint8_t*)av_malloc(frameSize);
    av_image_fill_arrays(video_frameBGR->data, video_frameBGR->linesize, outBuff,  AV_PIX_FMT_BGR24, pimpl->videoWidth, pimpl->videoHeight,1);

    sws_scale(pimpl->swsContext, frame->data,
              frame->linesize, 0, pimpl->videoHeight,
              video_frameBGR->data, video_frameBGR->linesize);

    av_frame_free(&frame);
    std::vector<char> rgbData(frameSize, 0);
    memcpy(rgbData.data(), outBuff, frameSize);
    av_frame_free(&video_frameBGR);
    av_free(outBuff);
    return rgbData;
}

void FFMPEGColorSpaceConverter::close()
{
    if (pimpl->swsContext) {
        sws_freeContext(pimpl->swsContext);
        pimpl->swsContext = nullptr;
    }
}
