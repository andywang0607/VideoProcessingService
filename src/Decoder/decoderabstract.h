#ifndef DECODERABSTRACT_H
#define DECODERABSTRACT_H

/*
 *   Defined interface of decoder module
 *   This interface is designed according to ffmpeg
 *   Maybe refactor to more general form future
 */

#include <atomic>

struct AVCodecParameters;
struct AVFrame;
struct AVPacket;

class DecoderAbstract
{
public:
    /**
     * Open decoder module with codec parameter
     * @param para can be obtained from demuxer
     * @param threadNum is num of thread for decode
     * @return success or failed
     */
    virtual bool open(AVCodecParameters *para, int threadNum) = 0;

    /**
     * Push demuxed data to decode queue
     * @param pkt is data after demux
     * @return success or failed
     */
    virtual bool push2DecodeQueue(AVPacket *pkt) = 0;

    /**
     * Get decode data
     * @return decoded data
     */
    virtual AVFrame* getDecodeData() = 0;

    /**
     * Clear and close decoder
     * @return decoded data
     */
    virtual void close() = 0;

    // pts: for synchonize
    std::atomic_uint64_t pts = 0;

};

#endif // DECODERABSTRACT_H
