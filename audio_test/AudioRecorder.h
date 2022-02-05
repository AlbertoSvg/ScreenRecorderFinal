//
// Created by alber on 08/11/2021.
//


#ifndef SCREENRECORDERPROJECT_AUDIORECORDER_H
#define SCREENRECORDERPROJECT_AUDIORECORDER_H



#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <cmath>
#include <cstring>
#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "SDL2/SDL_thread.h"

//#define __STDC_CONSTANT_MACROS

//FFMPEG LIBRARIES
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavcodec/avfft.h"

#include "libavdevice/avdevice.h"

#include "libavfilter/avfilter.h"
    //#include "libavfilter/avfiltergraph.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"
#include "libavformat/avformat.h"
#include "libavformat/avio.h"
    // libav resample

#include "libavutil/opt.h"
#include "libavutil/common.h"
#include "libavutil/channel_layout.h"
#include "libavutil/imgutils.h"
#include "libavutil/mathematics.h"
#include "libavutil/samplefmt.h"
#include "libavutil/opt.h"
#include "libavutil/pixdesc.h"
#include "libavutil/file.h"
#include "libavutil/audio_fifo.h"
#include "libavutil/avassert.h"

    // lib swresample

#include "libswscale/swscale.h"
#include "libswresample/swresample.h"


#ifdef WIN32
#include <windows.h>
#elif linux
#include <X11/Xlib.h>
#else

#endif

}

using namespace std;

class AudioRecorder {
    int value;
    int VideoStreamIndx;
    int width;
    int height;
    int offset_x, offset_y;
    int offset_x_v2, offset_y_v2;

    AVFormatContext *iFormatCtx;
    AVFormatContext *oFormatCtx;

    const AVInputFormat *iformat;
    const AVOutputFormat *oformat;

    AVDictionary *options;

    const AVCodec *DecoderCodec;
    AVCodecContext *DecoderCodecCtx;
    AVCodecParameters *DecoderCodecParams;

    const AVCodec *EncoderCodec;
    AVCodecContext *EncoderCodecCtx;

    AVFrame *pFrame;
    AVFrame *pict;

    AVStream *stream;

    AVPacket *pPacket;
    AVPacket *outPacket;

    /** AUDIO **/
    AVAudioFifo *audio_fifo;
    AVStream *audio_stream;
    AVPacket *pAudioPacket;
    AVPacket *outAudioPacket;
    AVFrame *pAudioFrame;
    AVFrame *outAudioFrame;




    int AudioStreamIndx;

    AVDictionary *AudioOptions;

    AVFormatContext *AudioInFCtx;
    AVFormatContext  *AudioOutFCtx;

    const AVInputFormat *audioIFormat;

    const AVCodec *AudioDecoderCodec;
    AVCodecParameters *AudioDecoderCodecParams;
    AVCodecContext  *AudioDecoderCodecCtx;

    const AVCodec *AudioEncoderCodec;
    AVCodecContext *AudioEncoderCodecCtx;




    const char *output_file;
    /** Sws CTX **/
    struct SwsContext *sws_ctx;

    /** Swr CTX **/
    SwrContext* resampleContext;

public:
    AudioRecorder();
    ~AudioRecorder();

    int InitOutputFile();
    int DecEnc();
    int finish();



    /** AUDIO **/
    int OpenAudioDevice();
    int InitializeAudioDecoder();
    int SetUp_AudioEncoder();
    int PrepareAudioDecEnc();

};



#endif //SCREENRECORDERPROJECT_AUDIORECORDER_H
