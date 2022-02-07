#ifndef MY_TRIAL_RECORDER_H
#define MY_TRIAL_RECORDER_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <cmath>
#include <cstring>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <regex>

//FFMPEG LIBRARIES
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavcodec/avfft.h"

#include "libavdevice/avdevice.h"

#include "libavfilter/avfilter.h"
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

// lib swresample
#include "libswresample/swresample.h"
#include "libswscale/swscale.h"


#ifdef WIN32
#include <windows.h>
#elif linux
#include <X11/Xlib.h>
#else

#endif

}

using namespace std;

class Recorder {
    string audio_device;
    char *output_file;
    int value;
    int VideoStreamIndx;
    int outVideoStreamIndx;
    int width;
    int height;
    bool audio=false;
    int offset_x, offset_y;
    int offset_x_v2, offset_y_v2;
    int fps = 30;

    AVFormatContext *iFormatCtx;
    AVFormatContext *oFormatCtx;

    AVInputFormat *iformat;
    AVOutputFormat *oformat;

    AVDictionary *options;

    const AVCodec *DecoderCodec;
    AVCodecContext *DecoderCodecCtx;
    AVCodecParameters *DecoderCodecParams;

    const AVCodec *EncoderCodec;
    AVCodecContext *EncoderCodecCtx;

    AVStream *stream;

    /** AUDIO **/
    int AudioStreamIndx;
    int outAudioStreamIndx;

    AVDictionary *AudioOptions;

    AVFormatContext *AudioInFCtx;

    AVInputFormat *audioIFormat;

    const AVCodec *AudioDecoderCodec;
    AVCodecParameters *AudioDecoderCodecParams;
    AVCodecContext  *AudioDecoderCodecCtx;

    const AVCodec *AudioEncoderCodec;
    AVCodecContext *AudioEncoderCodecCtx;

    AVStream *audio_stream;

    /** LOCKS AND CVs **/
    bool exited = false;
    bool stopRecording = false;
    bool startRecording = false;
    bool pauseRecording = false;
    bool videoReady = false;
    bool audioReady = false;
    bool videoEnd = false;
    bool audioEnd = false;
    bool firstVideoPacket = false;

    mutex _lock;
    mutex write_lock;
    mutex pause_lock;
    mutex queue_lock;
    mutex _videoEnd;
    mutex _audio;
    mutex _video;
    condition_variable cv_start, cv_pause, cv_video, cv_audio;

    /** LOG **/
    ofstream outFile{ "./log.txt", ios::out };

    queue<AVPacket *> ReadRawPacketsQ;

    AVFrame *pFrame;
    AVFrame *pict;
    struct SwsContext *sws_ctx;

public:
    Recorder();
    ~Recorder();

    void menu();
    void captureMenu();

    void lastSetUp();

    int InitOutputFile();
    int AudioDecEnc();

    /** VIDEO **/
    int OpenVideoDevice();
    int InitializeVideoDecoder();
    int SetUp_VideoEncoder();
    void PrepareVideoDecEnc();
    void acquireVideoFrames();
    void encodeDecodeVideoStream();
    bool isEndVideo();
    void endVideo();
    void endAudio();
    void synchWithAudio();
    void synchWithVideo();

    /** AUDIO **/
    int OpenAudioDevice();
    int InitializeAudioDecoder();
    int SetUp_AudioEncoder();
    int PrepareAudioDecEnc(AVAudioFifo **audio_fifo, AVPacket **pAudioPacket, AVPacket **outAudioPacket, AVFrame **pAudioFrame, AVFrame **outAudioFrame, SwrContext **resampleContext);

    void startCapture();
};


#endif //MY_TRIAL_RECORDER_H
