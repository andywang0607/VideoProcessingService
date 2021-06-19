#ifndef CONVERTERABSTRACT_H
#define CONVERTERABSTRACT_H

/*
 *   Defined interface of color space converter module
 *   This module define yuv to rgb function for now
 *   This interface is designed according to ffmpeg
 *   Maybe refactor to more general form future
 */

#include <vector>

struct AVFrame;

class ColorSpaceConverterAbstract
{
public:

    /**
     * Init module with required parameter
     * @param width is vidoe witdh
     * @param height is vidoe height
     * @param format is vidoe format
     */
    virtual void initParam(int width, int height, int format) = 0;

    /**
     * Convert yuv to rgb
     * @param frame is yuv data
     * @return rgb data
     */
    virtual std::vector<char> yuv2Rgb(AVFrame *frame) = 0;

    /**
     * Close module and free memory
     */
    virtual void close() = 0;
};


#endif // CONVERTERABSTRACT_H
