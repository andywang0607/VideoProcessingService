#ifndef VIDEOPROCESSINGABSTRACT_H
#define VIDEOPROCESSINGABSTRACT_H

/*
 *   Defined interface of video processing
 *   Include decode and convert yuv to rgb
 *   This interface is designed according to ffmpeg
 *   Maybe refactor to more general form future
 */

#include <atomic>
#include <thread>
#include <vector>

struct AVCodecParameters;

class VideoProcessingAbstract
{
public:
    /**
     * Open video processing module with codec parameter
     * @param para is video codec parameter can be obtained from demux module
     * @return success of failed
     */
    virtual bool open(AVCodecParameters *para) = 0;

    /**
     * Start video processing
     */
    virtual void start() = 0;

    /**
     * Stop video processing
     */
    virtual void stop() = 0;

    /**
     * Clear queue include demuxed data and rgb data
     */
    virtual void clearQueue() = 0;


    // Video width
    int width = 0;

    // Video height
    int height = 0;

    // Current pts of frame
    std::atomic_int64_t synpts = 0;


protected:
    std::atomic_bool isRunning;
    std::vector<std::thread> videoThread;

};

#endif // VIDEOPROCESSINGABSTRACT_H
