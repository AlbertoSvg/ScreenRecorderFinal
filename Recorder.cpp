#include "Recorder.h"

/** Initialize resources **/
Recorder::Recorder() {
    avdevice_register_all();

#ifdef WIN32
    SetProcessDPIAware();
    width = GetSystemMetrics(SM_CXSCREEN);
    height = GetSystemMetrics(SM_CYSCREEN);
#elif linux
    Display* disp = XOpenDisplay(NULL);
    Screen*  scrn = DefaultScreenOfDisplay(disp);
    width = scrn->width;
    height = scrn->height;

#else

#endif

}

/** Destroy resources **/
Recorder::~Recorder() {
    if (!exited) {
        av_write_trailer(oFormatCtx);
        if (!(oformat->flags & AVFMT_NOFILE)) {
            int err = avio_close(oFormatCtx->pb);
            if (err < 0) {
                std::cout << "Failed to close file" << err << std::endl;
            }
        }

        avformat_close_input(&iFormatCtx);
        av_dict_free(&options);
        avcodec_close(DecoderCodecCtx);
        avcodec_close(EncoderCodecCtx);

        if (audio) {

            avformat_close_input(&AudioInFCtx);
            av_dict_free(&AudioOptions);
            avcodec_close(AudioDecoderCodecCtx);
            avcodec_close(AudioEncoderCodecCtx);
        }

        avformat_free_context(oFormatCtx);
        outFile.close();
    }
}

void Recorder::menu() {
    string output_file;
    char audio;
    int x1 = 0, x2 = 0, y1 = 0, y2 = 0;

    cout << "Specify an output file: ";
    cin >> output_file;
    endl(cout);

    this->output_file = (char *) calloc(output_file.length(), sizeof(char));
    output_file.copy(this->output_file, output_file.length());

    do {
        cout << "Do you want audio recording? (Y,N): ";
        cin >> audio;
        endl(cout);
    } while (audio != 'Y' && audio != 'y' && audio != 'N' && audio != 'n');

    this->audio = audio == 'Y' || audio == 'y';

    cout << " Insert offset X1: ";
    cin >> x1;
    while (!cin || x1 < 0 || x1 % 2 != 0) {
        if (!cin) // or if(cin.fail())
        {
            // user didn't input a number
            cout << "Please insert a valid offset" << endl;
            cin.clear(); // reset failbit
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skip bad input
            // next, request user reinput
        } else
            cout << "Offset X1 out of range!" << endl;

        cout << "Insert offset X1: ";
        cin >> x1;
    }


    cout << "Insert offset Y1: ";
    cin >> y1;
    while (!cin || y1 < 0 || y1 % 2 != 0) {
        if (!cin) // or if(cin.fail())
        {
            // user didn't input a number
            cout << "Please insert a valid offset" << endl;
            cin.clear(); // reset failbit
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skip bad input
            // next, request user reinput
        } else
            cout << "Offset Y1 out of range!" << endl;


        cout << "Insert offset Y1: ";
        cin >> y1;
    }

    cout << "Insert offset X2: ";
    cin >> x2;
    while (!cin || x2 < 0 || x2 % 2 != 0) {
        if (!cin) // or if(cin.fail())
        {
            // user didn't input a number
            cout << "Please insert a valid offset" << endl;
            cin.clear(); // reset failbit
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skip bad input
            // next, request user reinput
        } else
            cout << "Offset X2 out of range!" << endl;

        cout << "Insert offset X2: ";
        cin >> x2;
    }

    cout << "Insert offset Y2: ";
    cin >> y2;
    while (!cin || y2 < 0 || y2 % 2 != 0) {
        if (!cin) // or if(cin.fail())
        {
            // user didn't input a number
            cout << "Please insert a valid offset" << endl;
            cin.clear(); // reset failbit
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skip bad input
            // next, request user reinput
        } else
            cout << "Offset Y2 out of range!" << endl;

        cout << "Insert offset Y2: ";
        cin >> y2;
    }

    while (width - x1 - x2 <= 0 || height - y1 - y2 <= 0) {

        cout << "Offsets NOT valid! Please insert again" << endl;

        cout << "Insert offset X1: ";
        cin >> x1;
        while (!cin || x1 < 0 || x1 % 2 != 0) {
            if (!cin) // or if(cin.fail())
            {
                // user didn't input a number
                cout << "Please insert a valid offset" << endl;
                cin.clear(); // reset failbit
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skip bad input
                // next, request user reinput
            } else
                cout << "Offset X1 out of range!" << endl;

            cout << "Insert offset X1: ";
            cin >> x1;
        }

        cout << "Insert offset Y1: ";
        cin >> y1;
        cout.flush();
        while (!cin || y1 < 0 || y1 % 2 != 0) {
            if (!cin) // or if(cin.fail())
            {
                // user didn't input a number
                cout << "Please insert a valid offset" << endl;
                cin.clear(); // reset failbit
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skip bad input
                // next, request user reinput
            } else
                cout << "Offset Y1 out of range!" << endl;

            cout << "Insert offset Y1: ";
            cin >> y1;
            cout.flush();
        }

        cout << "Insert offset X2: ";
        cin >> x2;
        cout.flush();
        while (!cin || x2 < 0 || x2 % 2 != 0) {
            if (!cin) // or if(cin.fail())
            {
                // user didn't input a number
                cout << "Please insert a valid offset" << endl;
                cin.clear(); // reset failbit
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skip bad input
                // next, request user reinput
            } else
                cout << "Offset X2 out of range!" << endl;

            cout << "Insert offset X2: ";
            cin >> x2;
            cout.flush();
        }

        cout << "Insert offset Y2: ";
        cin >> y2;
        cout.flush();
        while (!cin || y2 < 0 || y2 % 2 != 0) {
            if (!cin) // or if(cin.fail())
            {
                // user didn't input a number
                cout << "Please insert a valid offset" << endl;
                cin.clear(); // reset failbit
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skip bad input
                // next, request user reinput
            } else
                cout << "Offset Y2 out of range!" << endl;

            cout << "Insert offset Y2: ";
            cin >> y2;
            cout.flush();
        }
    }

    width = width - x1 - x2;
    height = height - y1 - y2;
    offset_x = x1;
    offset_y = y1;
    offset_x_v2 = x2;
    offset_y_v2 = y2;
}

void Recorder::captureMenu() {
    int _read;

    endl(cout);
    cout << "Menu options: " << endl;
    do {
        cin.clear(); // reset failbit
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skip bad input
        cout << "\t[1] Start recording" << endl;
        cout << "\t[2] Exit" << endl;
        cout << "\nInsert choice: ";
        cin >> _read;
        endl(cout);
    } while (!cin && _read != 1 && _read != 2);

    {
        lock_guard<mutex> lg(_lock);
        if (_read == 2) {
            cout << "EXITED" << endl;
            exited = true;
        } else
            startRecording = true;
    }
    cv_start.notify_all();

    if (exited)
        return;

    while (true) {
        cout << "\n ......RECORDING......\n\n";
        cout << "Press:  " << endl;
        cout << "\t[1] Stop recording";

        if (pauseRecording) cout << "\t[2] Resume recording" << endl;
        else cout << "\t[2] Pause recording" << endl;

        cout << "Insert choice: ";
        cin >> _read;
        endl(cout);
        if (!cin) {
            cin.clear(); // reset failbit
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skip bad input
        } else {
            if (_read == 1) {
                lock_guard<mutex> lg(_lock);
                stopRecording = true;
                cout << "...STOPPED" << endl;
                cv_pause.notify_all();
                break;
            } else if (_read == 2 && pauseRecording) {
                lock_guard<mutex> lg(_lock);
                pauseRecording = false;
                cout << "...RESUMED" << endl;
                cv_pause.notify_all();
            } else if (_read == 2 && !pauseRecording) {
                lock_guard<mutex> lg(_lock);
                pauseRecording = true;
                cout << "...PAUSED" << endl;
            }
        }
    }
}

/**           VIDEO            **/
int Recorder::OpenVideoDevice() {
    value = 0;
    options = nullptr;

    string video_size = to_string(width) + "*" + to_string(height);

#ifdef WIN32
    av_dict_set(&options, "offset_x", to_string(offset_x).c_str(), 0);
    av_dict_set(&options, "offset_y", to_string(offset_y).c_str(), 0);
#endif
    av_dict_set(&options, "video_size", video_size.c_str(), 0);
    av_dict_set(&options, "probesize", "30M", 0);
    av_dict_set(&options, "framerate", to_string(fps).c_str(), 0);
    av_dict_set(&options, "preset", "ultrafast", 0);
    av_dict_set(&options, "pixel_format", "uyvy422", 0);
    av_dict_set(&options, "show_region", "1", 0);

    iFormatCtx = avformat_alloc_context();
    if (iFormatCtx == nullptr) {
        cout << "Could not allocate memory for Input Format Context\n";
        exit(1);
    }

#ifdef WIN32
    iformat = av_find_input_format("gdigrab");
    value = avformat_open_input(&iFormatCtx, "desktop", iformat, &options);
    if (value < 0) {
        cout << "Cannot open input device\n";
        exit(1);
    }
#elif defined linux
    iformat = av_find_input_format("x11grab");
    if(const char* env = std::getenv("DISPLAY")) {
        regex rgx(":[0-9].[0-9]");
        string display = env;
        if(!regex_match(display,rgx))
            display = display + ".0";
        value = avformat_open_input(&iFormatCtx, (display+"+" + to_string(offset_x) + "," + to_string(offset_y)).c_str(),
                                    iformat, &options);
        if (value < 0) {
            cout << "Cannot open input device\n";
            exit(1);
        }
    }
    else {
        cerr << "Cannot open input device\n";
        exit(2);
    }
#endif

    return 1;
}

int Recorder::InitializeVideoDecoder() {
    value = avformat_find_stream_info(iFormatCtx, &options);
    if (value < 0) {
        cout << "Failed to retrieve input stream info\n";
        exit(1);
    }

    VideoStreamIndx = -1;
    /* find the first video stream index . Also there is an API available to do the below operations */
    for (int i = 0; i < iFormatCtx->nb_streams; i++) // find video stream position/index.
        if (iFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            AVCodecParameters *iLocalCodecParams = nullptr;
            iLocalCodecParams = iFormatCtx->streams[i]->codecpar;

            const AVCodec *iLocalCodec = nullptr;
            iLocalCodec = avcodec_find_decoder(iLocalCodecParams->codec_id);
            if (iLocalCodec == nullptr) {
                cout << "Unsupported Codec\n";
                continue;
            }
            if (iLocalCodecParams->codec_type == AVMEDIA_TYPE_VIDEO) {
                if (VideoStreamIndx == -1) {
                    VideoStreamIndx = i;
                    DecoderCodec = iLocalCodec;
                    DecoderCodecParams = iLocalCodecParams;
                }

            }

        }

    if (VideoStreamIndx == -1) {
        cout << "\nunable to find the video stream index. (-1)";
        exit(1);
    }
    DecoderCodecCtx = avcodec_alloc_context3(DecoderCodec);
    if (!DecoderCodecCtx) {
        cout << "Failed to allocate memory for AVCodecContext\n";
        exit(1);
    }
    value = avcodec_parameters_to_context(DecoderCodecCtx, DecoderCodecParams);
    if (value < 0) {
        cout << "Failed to copy decoderCodec params to decoderCodecCtx\n";
        exit(1);
    }
    av_opt_set(DecoderCodecCtx, "preset", "ultrafast", 0);
//    DecoderCodecCtx->framerate = av_guess_frame_rate(iFormatCtx, iFormatCtx->streams[VideoStreamIndx], NULL);
//    DecoderCodecCtx->framerate = (AVRational) {fps, 1};
    value = avcodec_open2(DecoderCodecCtx, DecoderCodec, nullptr);
    if (value < 0) {
        cout << "Failed to open DecoderCodec!\n";
        exit(1);
    }
    return 1;
}

int Recorder::InitOutputFile() {
    value = 0;
    oFormatCtx = nullptr;

    oformat = av_guess_format(nullptr, output_file, nullptr);
    if (!oformat) {
        cout << "Can't create output format!\n";
        exit(1);
    }

    avformat_alloc_output_context2(&oFormatCtx, oformat, nullptr, output_file);
    if (!oFormatCtx) {
        cout << "Can't create output context\n";
        exit(1);
    }
    return 1;
}

void Recorder::lastSetUp() {
    if (!(oformat->flags & AVFMT_NOFILE)) {
        if ((value = avio_open(&oFormatCtx->pb, output_file, AVIO_FLAG_WRITE)) < 0) {
            cout << "Failed to open output file\n";
            exit(1);
        }
    }

    if ((value = avformat_write_header(oFormatCtx, nullptr)) < 0) {
        cout << "Failed to write header\n";
        exit(1);
    }
}

int Recorder::SetUp_VideoEncoder() {
    EncoderCodec = avcodec_find_encoder(oformat->video_codec);
    if (!EncoderCodec) {
        cout << "Can't create EncoderCodec\n";
        exit(1);
    }

    stream = avformat_new_stream(oFormatCtx, EncoderCodec);
    if (!stream) {
        cout << "Can't create stream for this format\n";
        exit(1);
    }

    EncoderCodecCtx = avcodec_alloc_context3(EncoderCodec);
    if (!EncoderCodecCtx) {
        cout << "Can't create EncoderCodec Context\n";
        exit(1);
    }

    stream->codecpar->codec_id = oformat->video_codec;
    stream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
    stream->codecpar->width = width;
    stream->codecpar->height = height;
    stream->codecpar->format = AV_PIX_FMT_YUV420P;
//    stream->codecpar->bit_rate = 10000000;
    stream->codecpar->bit_rate = 4000;
    avcodec_parameters_to_context(EncoderCodecCtx, stream->codecpar);
    EncoderCodecCtx->time_base.num = 1;
    EncoderCodecCtx->time_base.den = fps;
    EncoderCodecCtx->max_b_frames = 10;
    EncoderCodecCtx->gop_size = fps * 2;
    EncoderCodecCtx->qmin = 0;
    EncoderCodecCtx->qmax = 5;

//    stream->time_base = EncoderCodecCtx->time_base;

    if (stream->codecpar->codec_id == AV_CODEC_ID_H264) {
        av_opt_set(EncoderCodecCtx, "preset", "ultrafast", 0);
        av_opt_set(EncoderCodecCtx, "tune", "zerolatency", 0);
        av_opt_set(EncoderCodecCtx, "cabac", "1", 0);
        av_opt_set(EncoderCodecCtx, "ref", "3", 0);
        av_opt_set(EncoderCodecCtx, "deblock", "1:0:0", 0);
        av_opt_set(EncoderCodecCtx, "analyse", "0x3:0x113", 0);
        av_opt_set(EncoderCodecCtx, "subme", "7", 0);
        av_opt_set(EncoderCodecCtx, "chroma_qp_offset", "4", 0);
        av_opt_set(EncoderCodecCtx, "rc", "crf", 0);
        av_opt_set(EncoderCodecCtx, "rc_lookahead", "40", 0);
        av_opt_set(EncoderCodecCtx, "crf", "10.0", 0);
        av_opt_set(EncoderCodecCtx, "threads", "8", 0);
    }

    if (oFormatCtx->oformat->flags & AVFMT_GLOBALHEADER) {
        oFormatCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    //av_dump_format(oFormatCtx, 0, output_file, 1);

    value = avcodec_open2(EncoderCodecCtx, EncoderCodec, nullptr);
    if (value < 0) {
        cout << "Failed to open Encoder Codec\n";
        exit(0);
    }

    //find free stream index
    outVideoStreamIndx = -1;
    for (int i = 0; i < oFormatCtx->nb_streams; i++) {
        if (oFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            outVideoStreamIndx = i;
        }
    }
    if (outVideoStreamIndx < 0) {
        cerr << "Error: cannot find a free stream for video on the output" << endl;
        exit(1);
    }

    avcodec_parameters_from_context(stream->codecpar, EncoderCodecCtx);
    return 1;
}

/**             AUDIO            **/

int Recorder::OpenAudioDevice() {
    value = 0;
    AudioOptions = nullptr;

    AudioInFCtx = avformat_alloc_context();
    if (AudioInFCtx == nullptr) {
        cout << "Cannot allocate memory for audio input context\n";
        exit(1);
    }

    av_dict_set(&AudioOptions, "sample_rate", "44100", 0);
    if (value < 0) {
        cout << "Cannot set Audio Options\n";
        exit(1);
    }
    av_dict_set(&AudioOptions, "async", "25", 0);
    if (value < 0) {
        cout << "Cannot set Audio Options\n";
        exit(1);
    }
#ifdef WIN32
    audioIFormat = av_find_input_format("dshow");
    value = avformat_open_input(&AudioInFCtx, "audio=Microfono (Realtek Audio)", audioIFormat, &AudioOptions);
    if (value < 0) {
        cout << "Cannot open Audio input Windows\n";
        exit(10);
    }
#elif defined linux
    audioIFormat = av_find_input_format("alsa");
    value = avformat_open_input(&AudioInFCtx, "hw:0,0", audioIFormat,&AudioOptions);
    if(value < 0 ){
        cout << "Cannot open Audio input\n";
        exit(10);
    }
#endif

    return 1;
}

int Recorder::InitializeAudioDecoder() {
    value = avformat_find_stream_info(AudioInFCtx, &AudioOptions);
    if (value < 0) {
        cout << "Failed to retrieve input stream info\n";
        exit(1);
    }
    value = av_find_best_stream(AudioInFCtx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (value < 0) {
        cout << "Failed to retrieve input audio stream info\n";
        exit(1);
    }
    //av_dump_format(AudioInFCtx, 0, ":0.0", 0);

    AudioStreamIndx = -1;
    /* find the first video stream index . Also there is an API available to do the below operations */
    for (int i = 0; i < AudioInFCtx->nb_streams; i++) {// find video stream position/index.
        if (AudioInFCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            AVCodecParameters *iLocalCodecParams = nullptr;
            iLocalCodecParams = AudioInFCtx->streams[i]->codecpar;

            const AVCodec *iLocalCodec = nullptr;
            iLocalCodec = avcodec_find_decoder(iLocalCodecParams->codec_id);
            if (iLocalCodec == nullptr) {
                cout << "Unsupported Codec\n";
                continue;
            }
            if (iLocalCodecParams->codec_type == AVMEDIA_TYPE_AUDIO) {
                if (AudioStreamIndx == -1) {
                    AudioStreamIndx = i;
                    AudioDecoderCodec = iLocalCodec;
                    AudioDecoderCodecParams = iLocalCodecParams;
                }
            }
        }
    }
    /** AUDIO **/
    if (AudioStreamIndx == -1) {
        cout << "\nunable to find the Audio stream index. (-1)";
        exit(1);
    }
    AudioDecoderCodecCtx = avcodec_alloc_context3(AudioDecoderCodec);
    if (!AudioDecoderCodecCtx) {
        cout << "Failed to allocate memory for Audio AVCodecContext\n";
        exit(1);
    }
    value = avcodec_parameters_to_context(AudioDecoderCodecCtx, AudioDecoderCodecParams);
    if (value < 0) {
        cout << "Failed to copy Audio decoderCodec params to Audio decoderCodecCtx\n";
        exit(1);
    }
    value = avcodec_open2(AudioDecoderCodecCtx, AudioDecoderCodec, nullptr);
    if (value < 0) {
        cout << "Failed to open Audio DecoderCodec!\n";
        exit(1);
    }
    return 1;

}

int Recorder::SetUp_AudioEncoder() {
    AudioEncoderCodec = nullptr;

    AudioEncoderCodec = avcodec_find_encoder(oformat->audio_codec);
    if (!AudioEncoderCodec) {
        cout << "Can't create EncoderCodec\n";
        exit(1);
    }

    /** Returns the first available "slot" in oFormatCtx->streams (here = ->streams[1] **/
    audio_stream = avformat_new_stream(oFormatCtx, AudioEncoderCodec);
    if (!audio_stream) {
        cout << "Can't create stream for this format\n";
        exit(1);
    }

    AudioEncoderCodecCtx = avcodec_alloc_context3(AudioEncoderCodec);
    if (!AudioEncoderCodecCtx) {
        cout << "Can't create EncoderCodec Context\n";
        exit(1);
    }

    if (AudioEncoderCodec->supported_samplerates) {
        AudioEncoderCodecCtx->sample_rate = AudioEncoderCodec->supported_samplerates[0];
        for (int i = 0; i < AudioEncoderCodec->supported_samplerates[i]; i++) {
            if (AudioEncoderCodec->supported_samplerates[i] == AudioDecoderCodecCtx->sample_rate) {
                AudioEncoderCodecCtx->sample_rate = AudioDecoderCodecCtx->sample_rate;
            }
        }
    }

    AudioEncoderCodecCtx->codec_id = AudioEncoderCodec->id;
    AudioEncoderCodecCtx->sample_fmt = AudioEncoderCodec->sample_fmts ? AudioEncoderCodec->sample_fmts[0]
                                                                      : AV_SAMPLE_FMT_FLTP;
    AudioEncoderCodecCtx->channels = AudioDecoderCodecCtx->channels;
    AudioEncoderCodecCtx->channel_layout = av_get_default_channel_layout(AudioEncoderCodecCtx->channels);
    AudioEncoderCodecCtx->bit_rate = 96000;
    AudioEncoderCodecCtx->time_base = (AVRational) {1, AudioDecoderCodecCtx->sample_rate};
    AudioEncoderCodecCtx->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;

    audio_stream->time_base = (AVRational) {1, AudioDecoderCodecCtx->sample_rate};

    /** Already in Video Encoder SetUp **/
//    if (oFormatCtx->oformat->flags & AVFMT_GLOBALHEADER) {
//        oFormatCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
//    }

    if (avcodec_open2(AudioEncoderCodecCtx, AudioEncoderCodec, nullptr) < 0) {
        cout << "Error in opening the audio encoder codec!\n";
        exit(1);
    }


    //find free stream index
    outAudioStreamIndx = -1;
    for (int i = 0; i < oFormatCtx->nb_streams; i++) {
        if (oFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_UNKNOWN) {
            outAudioStreamIndx = i;
        }
    }
    if (outAudioStreamIndx < 0) {
        cerr << "Error: cannot find a free stream for audio on the output" << endl;
        exit(1);
    }
    /** Copy EncoderCodecCtx codec parameters into audio_stream (= oFormatCtx->streams[1]) **/
    avcodec_parameters_from_context(audio_stream->codecpar, AudioEncoderCodecCtx);

    return 1;
}

/**         DECODING & ENCODING          **/
void Recorder::PrepareVideoDecEnc() {
    pFrame = av_frame_alloc();
    if (pFrame == nullptr) {
        cout << "Could not allocate frame \n";
        exit(1);
    }

    sws_ctx = nullptr;
    // set up our SWSContext to convert the image data to YUV420:
    sws_ctx = sws_getContext(
            DecoderCodecCtx->width,
            DecoderCodecCtx->height,
            DecoderCodecCtx->pix_fmt,
            EncoderCodecCtx->width,
            EncoderCodecCtx->height,
            AV_PIX_FMT_YUV420P,
            SWS_FAST_BILINEAR,
            nullptr,
            nullptr,
            nullptr
    );

    /**
     * As we said before, we are using YV12 to display the image, and getting
     * YUV420 data from ffmpeg.
     */

    int numBytes;
    uint8_t *buffer = nullptr;

    numBytes = av_image_get_buffer_size(
            AV_PIX_FMT_YUV420P,
            DecoderCodecCtx->width,
            DecoderCodecCtx->height,
            32
    );
    buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));

    pict = av_frame_alloc();

    av_image_fill_arrays(
            pict->data,
            pict->linesize,
            buffer,
            AV_PIX_FMT_YUV420P,
            DecoderCodecCtx->width,
            DecoderCodecCtx->height,
            32
    );
}

int Recorder::PrepareAudioDecEnc(AVAudioFifo **audio_fifo, AVPacket **pAudioPacket, AVPacket **outAudioPacket,
                                 AVFrame **pAudioFrame, AVFrame **outAudioFrame, SwrContext **resampleContext) {
    /* Create the FIFO buffer based on the specified output sample format. */
    if (!(*audio_fifo = av_audio_fifo_alloc(AudioEncoderCodecCtx->sample_fmt,
                                            AudioEncoderCodecCtx->channels, 1))) {
        cerr << "Could not allocate FIFO\n";
        return AVERROR(ENOMEM);
    }

    //allocate space for a packet
    *pAudioPacket = (AVPacket *) av_malloc(sizeof(AVPacket));
    if (!*pAudioPacket) {
        cerr << "Cannot allocate an AVPacket for encoded audio" << endl;
        exit(1);
    }
    *pAudioPacket = av_packet_alloc();

    //allocate space for a frame
    *pAudioFrame = av_frame_alloc();
    if (!*pAudioFrame) {
        cerr << "Cannot allocate an AVFrame for encoded audio" << endl;
        exit(1);
    }

    *outAudioFrame = av_frame_alloc();
    if (!*outAudioFrame) {
        cerr << "Cannot allocate an AVFrame for encoded audio" << endl;
        exit(1);
    }

    *outAudioPacket = (AVPacket *) av_malloc(sizeof(AVPacket));
    if (!outAudioPacket) {
        cerr << "Cannot allocate an AVPacket for encoded video" << endl;
        exit(1);
    }

    //init the resampler
    *resampleContext = nullptr;
    *resampleContext = swr_alloc_set_opts(*resampleContext,
                                          av_get_default_channel_layout(AudioEncoderCodecCtx->channels),
                                          AudioEncoderCodecCtx->sample_fmt,
                                          AudioEncoderCodecCtx->sample_rate,
                                          av_get_default_channel_layout(AudioDecoderCodecCtx->channels),
                                          AudioDecoderCodecCtx->sample_fmt,
                                          AudioDecoderCodecCtx->sample_rate,
                                          0,
                                          nullptr);
    /** Perform a sanity check so that the number of converted samples is
    * not greater than the number of samples to be converted.
    * If the sample rates differ, this case has to be handled differently
    **/

    if (!*resampleContext) {
        cerr << "Cannot allocate the resample context" << endl;
        exit(1);
    }
    if ((swr_init(*resampleContext)) < 0) {
        cerr << "Could not open resample context\n";
        swr_free(resampleContext);
        exit(1);
    }
    return 0;
}

bool Recorder::isEndVideo(){
    lock_guard<mutex> lg_videoEnd(_videoEnd);
    return videoEnd;
}
void Recorder::endVideo() {
    lock_guard<mutex> lg_videoEnd(_videoEnd);
    videoEnd = true;
    if (audio)
        cv_audio.notify_all();
}

void Recorder::endAudio() {
    lock_guard<mutex> lg(_audio);
    audioEnd = true;
}

bool Recorder::AudioReady() {
    lock_guard<mutex> lg(_audio);
    return audioReady;
}

bool Recorder::VideoReady() {
    lock_guard<mutex> lg(_video);
    return videoReady;
}


void Recorder::decodeVideoStream() {
    AVPacket *pPacket;
    PrepareVideoDecEnc();
    int retValue;
    int remainingPackets = 30; // to add the packets after end is pressed

    if (audio) {
        unique_lock<mutex> ul_videoSynch(_video);
        videoReady = true;
        std::cout << "Video Ready" << endl;
        cv_video.wait(ul_videoSynch, [this]() { return AudioReady(); });
        cv_audio.notify_all();
        ul_videoSynch.unlock();
        std::cout << "Video Started" << endl;
    }

    while (remainingPackets != 0) {
        if (pauseRecording) {
            unique_lock<mutex> ul_pause(pause_lock);

            cv_pause.wait(ul_pause, [this]() { return !pauseRecording || stopRecording; });

            ul_pause.unlock();
            cv_pause.notify_all();
        }

        unique_lock<mutex> ul_stop(_lock);
        if (stopRecording) {
            if(retValue >= 0)
                remainingPackets--;
        }
        ul_stop.unlock();

        pPacket = av_packet_alloc();
        if (pPacket == nullptr) {
            printf("Could not alloc packet,\n");
            exit(1);
        }

        retValue = av_read_frame(iFormatCtx, pPacket);
        if (retValue >= 0 && pPacket->size > 0) {
            unique_lock<mutex> ul_queue(queue_lock);
            ReadRawPacketsQ.push(pPacket);
            ul_queue.unlock();
        } // end of prim while
    }

    cout << "VIDEO ENDING" << endl;
    endVideo();
}

void Recorder::encodeVideoStream() {
    AVPacket *pPacket;
    AVPacket *outPacket;
    int i = 1, j = 1, retSend, retReceive;

    while (true) {
        if (pauseRecording) {
            unique_lock<mutex> ul_pause(pause_lock);

            cv_pause.wait(ul_pause, [this]() { return !pauseRecording || stopRecording; });

            ul_pause.unlock();
            cv_pause.notify_all();
        }

        unique_lock<mutex> ul_queue(queue_lock);
        if (!ReadRawPacketsQ.empty()) {
            pPacket = ReadRawPacketsQ.front();
            ReadRawPacketsQ.pop();
            ul_queue.unlock();
            if (pPacket->stream_index == VideoStreamIndx) {
                retSend = avcodec_send_packet(DecoderCodecCtx, pPacket);
                av_packet_unref(pPacket);
                av_packet_free(&pPacket);
                if (retSend < 0) {
                    printf("Error sending packet for decoding.\n");
                    exit(-1);
                }
            }
            retReceive = avcodec_receive_frame(DecoderCodecCtx, pFrame);
            if (retReceive == 0) {
                // Convert the image into YUV format that SDL uses:
                // We change the conversion format to PIX_FMT_YUV420P, and we
                // use sws_scale just like before.
                sws_scale(sws_ctx, pFrame->data, pFrame->linesize, 0, DecoderCodecCtx->height, pict->data,
                          pict->linesize);
                pict->pts = (int64_t) j * (int64_t) 30 * (int64_t) 30 * (int64_t) 100 / (int64_t) fps;
                j++;
                pict->format = EncoderCodecCtx->pix_fmt;
                pict->width = EncoderCodecCtx->width;
                pict->height = EncoderCodecCtx->height;

                retSend = avcodec_send_frame(EncoderCodecCtx, pict);
                if (retSend >= 0) {
                    outPacket = av_packet_alloc();
                    if (outPacket == nullptr) {
                        cout << "Cannot allocate packet!" << endl;
                        exit(-1);
                    }
                    outPacket->data = nullptr;
                    outPacket->size = 0;
                    outPacket->flags |= AV_PKT_FLAG_KEY;

                    retReceive = avcodec_receive_packet(EncoderCodecCtx, outPacket);
                    if (retReceive == 0) {
                        if(!firstVideoPacket)
                            firstVideoPacket = true;
                        outPacket->pts = (int64_t) i * (int64_t) 30 * (int64_t) 30 * (int64_t) 100 / (int64_t) fps;
                        outPacket->dts = (int64_t) i * (int64_t) 30 * (int64_t) 30 * (int64_t) 100 / (int64_t) fps;
                        outPacket->stream_index = outVideoStreamIndx;

                        {
                            lock_guard<mutex> w_lk(write_lock);
                            outFile << "Write video frame " << "(size= " << outPacket->size / 1000 << " )" << " PTS = "
                             << outPacket->pts << endl;
                            if (av_interleaved_write_frame(oFormatCtx, outPacket) != 0) {
                                cout << "\nerror in writing video frame";
                            }
                        }
                        i++;
                    }
                    av_packet_unref(outPacket);
                }
            }else {
                printf("Error while decoding.\n");
                exit(-1);
            }
        } else {
            ul_queue.unlock();
            unique_lock<mutex> ul_end(_lock);
            if(stopRecording){
                ul_end.unlock();
                ul_queue.lock();
                if(ReadRawPacketsQ.empty() && isEndVideo()){
                    ul_queue.unlock();
                    /** FLUSHING DECODER **/
                    AVPacket *pkt;
                    pkt = av_packet_alloc();
                    pkt->data = nullptr;
                    pkt->size = 0;
                    for (int k = i;; k++) {
                        avcodec_send_frame(EncoderCodecCtx, nullptr);
                        if (avcodec_receive_packet(EncoderCodecCtx, pkt) == 0) {
                            pkt->pts = (int64_t) k * (int64_t) 30 * (int64_t) 30 * (int64_t) 100 / (int64_t) fps;
                            pkt->dts = (int64_t) k * (int64_t) 30 * (int64_t) 30 * (int64_t) 100 / (int64_t) fps;
                            pkt->stream_index = outVideoStreamIndx;
                            av_interleaved_write_frame(oFormatCtx, pkt);
                            outFile << "--- flushing video encoder ---" << endl;
                        } else {
                            av_packet_unref(pkt);
                            av_packet_free(&pkt);
                            break;
                        }
                    }
                    break;
                }
                ul_queue.unlock();
            }
        }
    }

    cout << "END DECODING-ENCODING" << endl;

    av_packet_free(&outPacket);
    av_frame_free(&pFrame);
    av_frame_free(&pict);
    av_packet_free(&pPacket);
    sws_freeContext(sws_ctx);
}

int Recorder::AudioDecEnc() {
    uint8_t **resampledData;

    AVAudioFifo *audio_fifo;
    AVPacket *pAudioPacket;
    AVPacket *outAudioPacket;
    AVFrame *pAudioFrame;
    AVFrame *outAudioFrame;

    SwrContext *resampleContext;

    bool beforeValidVideo = true;

    avformat_close_input(&AudioInFCtx); //clear the internal buffer
#ifdef WIN32
    value = avformat_open_input(&AudioInFCtx, "audio=Microfono (Realtek Audio)", audioIFormat, &AudioOptions);
#elif linux
    value = avformat_open_input(&AudioInFCtx, "hw:0,0", audioIFormat, &AudioOptions);
#else

#endif
    if (value < 0) {
        cout << "Cannot open Audio input\n";
        exit(10);
    }

    PrepareAudioDecEnc(&audio_fifo, &pAudioPacket, &outAudioPacket, &pAudioFrame, &outAudioFrame, &resampleContext);

    int j = 0;

    unique_lock<mutex> ul_audioSynch(_audio);
    audioReady = true;
    cout << "Audio Ready to start" << endl;
    cv_audio.wait(ul_audioSynch, [this]() { return VideoReady(); });
    cv_video.notify_all();
    ul_audioSynch.unlock();
    cout << "Audio Started!" << endl;

    while (true) {
        if (pauseRecording) {
            unique_lock<mutex> ul_pause(pause_lock);

            avformat_close_input(&AudioInFCtx); //close and clear the internal buffer

            cv_pause.wait(ul_pause, [this]() { return !pauseRecording || stopRecording; });

            ul_pause.unlock();
            cv_pause.notify_all();

            //reopen input audio
#ifdef WIN32
            value = avformat_open_input(&AudioInFCtx, "audio=Microfono (Realtek Audio)", audioIFormat,
                                        &AudioOptions);
#elif linux
            value = avformat_open_input(&AudioInFCtx, "hw:0,0", audioIFormat, &AudioOptions);
#else

#endif
            if (value < 0) {
                cout << "Cannot open Audio input\n";
                exit(10);
            }
        }

        unique_lock<mutex> ul_stopAudio(_lock);
        if (stopRecording) {
            endAudio();
            ul_stopAudio.unlock();
            // wait for video acquire thread to terminate
            ul_audioSynch.lock();
            cv_audio.wait(ul_audioSynch, [this]() { return isEndVideo(); });
            ul_audioSynch.unlock();
            break;
        }
        ul_stopAudio.unlock();

        if (av_read_frame(AudioInFCtx, pAudioPacket) >= 0) {
            if (pAudioPacket->stream_index == AudioStreamIndx) {
                av_packet_rescale_ts(outAudioPacket, AudioInFCtx->streams[AudioStreamIndx]->time_base,

                                     AudioDecoderCodecCtx->time_base);
                value = avcodec_send_packet(AudioDecoderCodecCtx, pAudioPacket);
                if (value < 0) {
                    cerr << "Cannot decode current audio packet" << value << endl;
                    return -1;
                }

                while (value >= 0) {
                    value = avcodec_receive_frame(AudioDecoderCodecCtx, pAudioFrame);

                    if (value == AVERROR(EAGAIN) || value == AVERROR_EOF) {
                        break;
                    } else if (value < 0) {
                        cerr << "Error while decoding.\n";
                        exit(1);
                    }

                    if (oFormatCtx->streams[outAudioStreamIndx]->start_time <= 0) {
                        oFormatCtx->streams[outAudioStreamIndx]->start_time = pAudioFrame->pts;
                    }

                    //initConvertedSamples

                    /* Allocate as many pointers as there are audio channels.
                    * Each pointer will later point to the audio samples of the corresponding
                    * channels (although it may be NULL for interleaved formats).
                    */

                    if (!(resampledData =
                                  (uint8_t **) calloc(AudioEncoderCodecCtx->channels,
                                                      sizeof(*resampledData)))) {
                        cerr << "Could not allocate converted input sample pointers\n";
                        return AVERROR(ENOMEM);

                    }

                    /* Allocate memory for the samples of all channels in one consecutive
                    * block for convenience. */
                    if (av_samples_alloc(resampledData, nullptr,
                                         AudioEncoderCodecCtx->channels,
                                         pAudioFrame->nb_samples,
                                         AudioEncoderCodecCtx->sample_fmt, 0) < 0) {

                        exit(1);
                    }

                    //convert
                    swr_convert(resampleContext,
                                resampledData, pAudioFrame->nb_samples,
                                (const uint8_t **) pAudioFrame->extended_data, pAudioFrame->nb_samples);


                    //add_sample_to_fifo
                    /* Make the FIFO as large as it needs to be to hold both,
                    * the old and the new samples. */
                    if ((value = av_audio_fifo_realloc(audio_fifo,
                                                       av_audio_fifo_size(audio_fifo) + pAudioFrame->nb_samples)) <
                        0) {
                        cerr << "Could not reallocate FIFO\n";
                        exit(value);
                    }

                    /* Store the new samples in the FIFO buffer. */

                    if (av_audio_fifo_write(audio_fifo, (void **) resampledData, pAudioFrame->nb_samples) <
                        pAudioFrame->nb_samples) {
                        cerr << "Could not write data to FIFO\n";
                        exit(AVERROR_EXIT);
                    }

                    //pAudioFrame ready
                    outAudioPacket = av_packet_alloc();
                    outAudioPacket->data = nullptr;
                    outAudioPacket->size = 0;

                    outAudioFrame = av_frame_alloc();
                    if (!outAudioFrame) {
                        cerr << "Cannot allocate an AVFrame for encoded audio\n";
                        exit(1);
                    }

                    outAudioFrame->nb_samples = AudioEncoderCodecCtx->frame_size;
                    outAudioFrame->channel_layout = AudioEncoderCodecCtx->channel_layout;
                    outAudioFrame->format = AudioEncoderCodecCtx->sample_fmt;
                    outAudioFrame->sample_rate = AudioEncoderCodecCtx->sample_rate;
                    av_frame_get_buffer(outAudioFrame, 0);


                    while (av_audio_fifo_size(audio_fifo) >=
                           AudioEncoderCodecCtx->frame_size) { //frame_size = number of samples per channel in an audio frame.
                        value = av_audio_fifo_read(audio_fifo, (void **) outAudioFrame->data,
                                                   AudioEncoderCodecCtx->frame_size);
                        outAudioFrame->pts = j;
                        j += outAudioFrame->nb_samples;
                        value = avcodec_send_frame(AudioEncoderCodecCtx, outAudioFrame);
                        if (value == AVERROR(EAGAIN) || value == AVERROR_EOF)
                            break;
                        if (value < 0) {
                            cerr << "Cannot encode current audio packet\n" << j << endl;
                            exit(1);
                        }
                        while (value >= 0) {
                            value = avcodec_receive_packet(AudioEncoderCodecCtx, outAudioPacket);
                            if (value == AVERROR(EAGAIN) || value == AVERROR_EOF)
                                break;
                            else if (value < 0) {
                                cerr << "Error during encoding\n";
                                exit(1);
                            }

                            av_packet_rescale_ts(outAudioPacket, AudioEncoderCodecCtx->time_base,
                                                 audio_stream->time_base);
                            outAudioPacket->stream_index = outAudioStreamIndx;

                            {
                                lock_guard<mutex> w_lk(write_lock);
                                outFile << "Write audio frame " << j << "(size= " << outAudioPacket->size / 1000
                                        << " )" << endl;
                                if(firstVideoPacket){
                                    if(!beforeValidVideo){
                                        if (av_write_frame(oFormatCtx, outAudioPacket) != 0) {
                                            cerr << "Error in writing audio frame\n";
                                        }
                                    }else
                                        beforeValidVideo = false;
                                }
                            }
                            av_packet_unref(outAudioPacket);
                        }
                        value = 0;
                    }
                    av_frame_free(&outAudioFrame);
                    av_packet_unref(outAudioPacket);
                }
            }
        }
    }
    av_audio_fifo_free(audio_fifo);
    swr_free(&resampleContext);
    return 1;
}

void Recorder::startCapture() {

    auto decodeVideo = [this]() -> void {
//        VideoDecEnc();
//        finish();
        decodeVideoStream();
    };
    auto encodeVideo = [this]() -> void {
        encodeVideoStream();
    };
    auto captureAudio = [this]() -> void {
        AudioDecEnc();
    };

    auto manageCapture = [this]() -> void {
        captureMenu();
    };

    InitOutputFile();
    thread t4(manageCapture);

    unique_lock<mutex> ul_start(_lock);
    cv_start.wait(ul_start, [this]() { return startRecording || exited; });
    if (exited && !startRecording) {
        ul_start.unlock();
        return;
    }
    ul_start.unlock();

    OpenVideoDevice();
    InitializeVideoDecoder();
    SetUp_VideoEncoder();

    if(audio){
        OpenAudioDevice();
        InitializeAudioDecoder();
        SetUp_AudioEncoder();
    }

    lastSetUp();

    thread t1(decodeVideo);
    thread t2(encodeVideo);
    thread t3;
    if (audio)
        t3 = thread(captureAudio);
    t1.join();
    t2.join();
    if (audio)
        t3.join();
    t4.join();
    cout << "...EXITING" << endl;
}