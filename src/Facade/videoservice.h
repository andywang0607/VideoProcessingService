#ifndef VIDEOSERVICE_H
#define VIDEOSERVICE_H

/*
 *   A Facade class for video service
 *   Provide all required function for client
 */

#include <thread>
#include <memory>
#include <vector>
#include <atomic>

#include "src/Thread/ffmpegaudioprocessing.h"
#include "src/Thread/ffmpegvideoprocessing.h"
#include "src/Demuxer/ffmpegdemuxer.h"


enum ParamInt{
    VideoWidth,
    VideoHeight,
    VideoLength,
};


class VideoService
{
public:
    VideoService();
    ~VideoService();

    /**
     * Open all required module with video url
     * @param url can be a video path or network streaming url
     * @return success or failed
     */
    bool open(const char *url);

    /**
     * Start all module
     */
    void start();

    /**
     * Stop all module
     */
    void stop();

    /**
     * Seek to the key frame
     * @param pos is in range 0~1
     */
    void seek(double pos);

    /**
     * Get rgb data and its pts of a video frame
     * @return rgb data and pts from queue
     */
    std::shared_ptr<std::pair<std::vector<char>, int64_t>> getVideoRGBFrame();

    /**
     * Get audio data and its pts
     * @return pcm data and pts from queue
     */
    std::shared_ptr<std::pair<std::vector<char>, int64_t>> getPcmData();

    /**
     * Get parameter value (int type limited)
     * @param param can be selected in ParamInt
     * @return value from member variable
     */
    uint64_t getParameterInt(ParamInt param);
private:
    std::shared_ptr<FFMPEGVideoProcessing> vp;
    std::shared_ptr<FFMPEGAudioProcessing> ap;
    std::shared_ptr<FFMPEGDemuxer> demuxer;

    std::atomic_bool isRunning;
    std::vector<std::thread> controlThread;

    // Parameter
    /// Total time of video (milliseconds)
    uint64_t totalMs = 0;

    /// Video width and height
    int width = 0;
    int height = 0;

    /// sample rate and channel num of audio player
    int sampleRate = 44100;
    int channelNum = 2;
};

#endif // VIDEOSERVICE_H
