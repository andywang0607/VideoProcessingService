#ifndef AUDIOPROCESSINGABSTRACT_H
#define AUDIOPROCESSINGABSTRACT_H

/*
 *   Defined interface of audio processing
 *   Include decode and resample
 *   This interface is designed according to ffmpeg
 *   Maybe refactor to more general form future
 */


#include <atomic>
#include <vector>
#include <thread>

struct AVCodecParameters;

class AudioProcessingAbstract
{
public:

    /**
     * Open audio processing module with codec parameter
     * @param para is audio codec parameter can be obtained from demux module
     * @return success or failed
     */
    virtual bool open(AVCodecParameters *para, int channel, int sampleRate) = 0;

    /**
     * Start audio processing
     */
    virtual void start() = 0;

    /**
     * Stop audio processing
     */
    virtual void stop() = 0;


    /**
     * Clear queue include demuxed data and resampled pcm data
     */
    virtual void clearQueue() = 0;

    // Sample rate after resample
    int sampleRate = 0;

    // Channel number after resample
    int channel = 0;

    // pts of current demuxed data
    std::atomic_int64_t pts = 0;

protected:
    std::atomic_bool isRunning;
    std::vector<std::thread> audioThread;

};

#endif // AUDIOPROCESSINGABSTRACT_H
