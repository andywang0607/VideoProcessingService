#ifndef RESAMPLERABSTRACT_H
#define RESAMPLERABSTRACT_H

/*
 *   Defined interface of audio resample module
 *   This interface is designed according to ffmpeg
 *   Maybe refactor to more general form future
 */


/**
 * @brief The AudioSampleFormat enum
 * @warning This enum is referenced to ffmpeg temporary
 */
enum AudioSampleFormat
{
    SAMPLE_FMT_NONE,
    SAMPLE_FMT_U8,
    SAMPLE_FMT_S16,
    SAMPLE_FMT_S32,
    SAMPLE_FMT_FLT,
    SAMPLE_FMT_DBL,
    SAMPLE_FMT_U8P,
    SAMPLE_FMT_S16P,
    SAMPLE_FMT_S32P,
    SAMPLE_FMT_FLTP,
    SAMPLE_FMT_DBLP,
    SAMPLE_FMT_NB,
};

struct AVFrame;

class ResamplerAbstract
{
public:
    /**
     * Open resample module with required parameter
     * @param channelIn is input channel num
     * @param sampleRateIn is input sampling rate
     * @param formatIn is input format
     * @param channelOut is output channel num
     * @param sampleRateOut is output sampling rate
     * @param formatOut is output format
     * @return success of failed
     */
    virtual bool open(int channelIn, int sampleRateIn, AudioSampleFormat formatIn,int channelOut=2, int sampleRateOut=44100, AudioSampleFormat formatOut=AudioSampleFormat::SAMPLE_FMT_S32) = 0;

    /**
     * Free memory and close resample module and
     */
    virtual void close() = 0;

    /**
     * Resample data with parameter setting in open function
     * @param indata is input audio data
     * @param pcm is output pcm data after resample
     * @return size of output pcm
     */
    virtual int resample(AVFrame *indata, unsigned char *pcm) = 0;

    // Channel number of output data
    int channelOut = 0;

    // Sample rate of output data
    int sampleRateOut = 0;

    // Sample format of output data
    int formatOut = AudioSampleFormat::SAMPLE_FMT_S32;
};

#endif // RESAMPLERABSTRACT_H
