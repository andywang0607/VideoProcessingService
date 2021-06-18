#ifndef DEMUXERINTERFACE_H
#define DEMUXERINTERFACE_H

/*
 *   Defined interface of demuxe module
 *   This interface is designed according to ffmpeg
 *   Maybe refactor to more general form future
 */

struct AVFormatContext;
struct AVPacket;
struct AVCodecParameters;

class DemuxerAbstract
{
public:
    /**
     * Open dumux module with a url
     * @param url can be a file or a network streaming url
     * @return success of failed
     */
    virtual bool open(const char *url) = 0;

    /**
     * Get data before decode
     * @return data before decode
     */
    virtual AVPacket *read() = 0;

    /**
     * Distinguish this predecode data is audio or video
     * @param pkt is data before decoding
     * @return true is audio otherwise is video
     */
    virtual bool isAudio(AVPacket *pkt) = 0;

    /**
     * Get video codec parameter
     * @return video codec parameter
     */
    virtual AVCodecParameters *getVideoCodecParameter() = 0;

    /**
     * Get audio codec parameter
     * @return audio codec parameter
     */
    virtual AVCodecParameters *getAudioCodecParameter() = 0;

    /**
     * Designed for jump to specific time position of video
     * @param pos in range 0~1
     */
    virtual bool seek(double pos) = 0;

    /**
     * Free memory and close this module
     */
    virtual void close() = 0;

    /// information
    // video information
    long long totalMs = 0;

    int width = 0;
    int height = 0;

    // audio information
    int sampleRate = 0;
    int channel = 0;

};

#endif // DEMUXERINTERFACE_H
