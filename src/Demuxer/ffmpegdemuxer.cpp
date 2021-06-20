#include <iostream>

#include "ffmpegdemuxer.h"

extern "C" {
#include "libavformat/avformat.h"
}

using namespace std;

static double r2d(AVRational r)
{
    return r.den == 0 ? 0 : (double)r.num / (double)r.den;
}

FFMPEGDemuxer::FFMPEGDemuxer()
{

}

bool FFMPEGDemuxer::open(const char *url)
{
    close();
    // Parameter setting
    AVDictionary *opts = NULL;
    av_dict_set(&opts, "stimeout", "2000000", 0);

    lock_guard<std::mutex> lck (mtx);
    int re = avformat_open_input(
                &ioContext,
                url,    //  Input url
                0,      //  Select demuxer automatically
                &opts
                );

    if (re != 0) {
        char buf[1024] = { 0 };
        av_strerror(re, buf, sizeof(buf) - 1);
        cout << "open " << url << " failed! :" << buf << endl;
        return false;
    }
    cout << "open " << url << " success! " << endl;

    re = avformat_find_stream_info(ioContext, 0);

    this->totalMs = ioContext->duration / (AV_TIME_BASE / 1000);
    cout << "totalMs = " << totalMs << endl;

    av_dump_format(ioContext, 0, url, 0);


    videoStream = av_find_best_stream(ioContext, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    AVStream *avStream = ioContext->streams[videoStream];
    width = avStream->codecpar->width;
    height = avStream->codecpar->height;

    cout << "=======================================================" << endl;
    cout << "codec_id = " << avStream->codecpar->codec_id << endl;
    cout << "format = " << avStream->codecpar->format << endl;
    cout << "width=" << avStream->codecpar->width << endl;
    cout << "height=" << avStream->codecpar->height << endl;
    cout << "video fps = " << r2d(avStream->avg_frame_rate) << endl;

    cout << "=======================================================" << endl;

    audioStream = av_find_best_stream(ioContext, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    avStream = ioContext->streams[audioStream];
    cout << "codec_id = " << avStream->codecpar->codec_id << endl;
    cout << "format = " << avStream->codecpar->format << endl;
    sampleRate = avStream->codecpar->sample_rate;
    cout << "sample_rate = " << sampleRate << endl;
    //AVSampleFormat;
    channel = avStream->codecpar->channels;
    cout << "channels = " << avStream->codecpar->channels << endl;
    cout << "frame_size = " << avStream->codecpar->frame_size << endl;

    return true;
}

AVPacket *FFMPEGDemuxer::read()
{
    if (!ioContext) {
        cout << "ioContext is nullptr" << endl;
        return 0;
    }
    AVPacket *pkt = av_packet_alloc();
    int re = av_read_frame(ioContext, pkt);
    if (re != 0) {
        av_packet_free(&pkt);
        return nullptr;
    }
    pkt->pts = pkt->pts*(1000 * (r2d(ioContext->streams[pkt->stream_index]->time_base)));
    pkt->dts = pkt->dts*(1000 * (r2d(ioContext->streams[pkt->stream_index]->time_base)));
    return pkt;
}

bool FFMPEGDemuxer::isAudio(AVPacket *pkt)
{
    if (!pkt)
        return false;
    if (pkt->stream_index == videoStream)
        return false;
    return true;
}

AVCodecParameters *FFMPEGDemuxer::getVideoCodecParameter()
{
    lock_guard<std::mutex> lck (mtx);
    if (!ioContext) {
        cout << "getVideoCodecParameter getVideoCodecParameter ic is null" << endl;
        return NULL;
    }
    AVCodecParameters *pa = avcodec_parameters_alloc();
    avcodec_parameters_copy(pa, ioContext->streams[videoStream]->codecpar);
    return pa;
}

AVCodecParameters *FFMPEGDemuxer::getAudioCodecParameter()
{
    lock_guard<std::mutex> lck (mtx);
    if (!ioContext) {
        cout << "getAudioCodecParameter getAudioCodecParameter ioContext is null" << endl;
        return NULL;
    }
    AVCodecParameters *pa = avcodec_parameters_alloc();
    avcodec_parameters_copy(pa, ioContext->streams[audioStream]->codecpar);
    return pa;
}

bool FFMPEGDemuxer::seek(double pos)
{
    lock_guard<std::mutex> lck (mtx);
    if (!ioContext) {
        cout << "ioContext is nullptr" << endl;
        return false;
    }
    avformat_flush(ioContext);

    long long seekPos = 0;
    seekPos = ioContext->streams[videoStream]->duration * pos;
    int re = av_seek_frame(ioContext, videoStream, seekPos, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
    if (re < 0) {
        cout << "seek re less than 0" << endl;
        return false;
    }
    return true;
}

void FFMPEGDemuxer::close()
{
    lock_guard<std::mutex> lck (mtx);
    if (!ioContext) {
        cout << "close ioContext is nullptr" << endl;
        return;
    }
    avformat_flush(ioContext);
    avformat_close_input(&ioContext);
    totalMs = 0;
}
