#include "Recorder.h"

/* Initialize resources */
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

#endif

}

/* Destroy resources */
Recorder::~Recorder() {
    /* If the user exits before starting the recording there is nothing to free */
    if (!exited) {
        /* Write the stream trailer to the output file and free the file private data*/
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
        avformat_free_context(iFormatCtx);
        outFile.close();
    }
}

/* Get input information from user:
 * 1. Audio device
 * 2. output file location and format
 * 3. Choose if want audio recording
 * 4. Specify offset for the screen
 * */
void Recorder::menu() {
    string output_file;
    char audio;
    int x1 = 0, x2 = 0, y1 = 0, y2 = 0;
    string audio_dev;

    cout << "Specify an audio device: ";
    getline(cin, audio_dev);
    endl(cout);

#if defined WIN32
    this->audio_device = "audio=" + audio_dev;
#elif defined linux
    this->audio_device = audio_dev;
#endif

    cout << "Specify an output file: ";
    cin >> output_file;
    endl(cout);

    this->output_file = (char *) calloc(output_file.length(), sizeof(char));
    output_file.copy(this->output_file, output_file.length());

    do {
        cout << "Do you want audio recording? (Y,N):";
        cin >> audio;
        endl(cout);
    } while (audio != 'Y' && audio != 'y' && audio != 'N' && audio != 'n');

    this->audio = audio == 'Y' || audio == 'y';

    /* For each offset check that is not < 0 and that is a multiple of 2*/
    cout << "Insert offset X1:";
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

        cout << "Insert offset X1:";
        cin >> x1;
    }


    cout << "Insert offset Y1:";
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


        cout << "Insert offset Y1:";
        cin >> y1;
    }

    cout << "Insert offset X2:";
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

        cout << "Insert offset X2:";
        cin >> x2;
    }

    cout << "Insert offset Y2:";
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

        cout << "Insert offset Y2:";
        cin >> y2;
    }

    /* Check inserted offsets do not cross the screen size */
    while (width - x1 - x2 <= 0 || height - y1 - y2 <= 0) {

        cout << "Offsets NOT valid! Please insert again" << endl;

        cout << "Insert offset X1:";
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

            cout << "Insert offset X1:";
            cin >> x1;
        }

        cout << "Insert offset Y1:";
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

            cout << "Insert offset Y1:";
            cin >> y1;
            cout.flush();
        }

        cout << "Insert offset X2:";
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

            cout << "Insert offset X2:";
            cin >> x2;
            cout.flush();
        }

        cout << "Insert offset Y2:";
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

            cout << "Insert offset Y2:";
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

/* Menu for managing recording */
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
    } while (!cin || (_read != 1 && _read != 2));

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

        cout << "Insert choice: " << endl;
        cin >> _read;
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

/*         VIDEO            */
int Recorder::OpenVideoDevice() {
    value = 0;
    options = nullptr;

    string video_size = to_string(width) + "*" + to_string(height);

#ifdef WIN32
    av_dict_set(&options, "offset_x", to_string(offset_x).c_str(), 0);
    av_dict_set(&options, "offset_y", to_string(offset_y).c_str(), 0);
#elif defined linux
    av_dict_set(&options, "show_region", "1", 0);
#endif
    av_dict_set(&options, "video_size", video_size.c_str(), 0);
    av_dict_set(&options, "probesize", "30M", 0);
    av_dict_set(&options, "framerate", to_string(fps).c_str(), 0);
    /* encoding speed */
    av_dict_set(&options, "preset", "ultrafast", 0);

    iFormatCtx = avformat_alloc_context();
    if (iFormatCtx == nullptr) {
        throw runtime_error("Could not allocate memory for Input Format Context\n");
    }

#ifdef WIN32
    iformat = const_cast<AVInputFormat *>(av_find_input_format("gdigrab"));
    value = avformat_open_input(&iFormatCtx, "desktop", iformat, &options);
    if (value < 0) {
        throw runtime_error("Cannot open input device\n");
    }
#elif defined linux
    iformat = const_cast<AVInputFormat *>(av_find_input_format("x11grab"));
    if(const char* env = std::getenv("DISPLAY")) {
        regex rgx(":[0-9].[0-9]");
        string display = env;
        if(!regex_match(display,rgx))
            display = display + ".0";
        value = avformat_open_input(&iFormatCtx, (display+"+" + to_string(offset_x) + "," + to_string(offset_y)).c_str(),
                                    iformat, &options);
        if (value < 0) {
            throw runtime_error("Cannot open input device\n");
        }
    }
    else {
        throw runtime_error("Cannot locate environment variable DISPLAY\n");

    }
#endif

    return 1;
}

int Recorder::InitializeVideoDecoder() {
    value = avformat_find_stream_info(iFormatCtx, &options);
    if (value < 0) {
        throw runtime_error("Failed to retrieve input stream info\n");
    }

    VideoStreamIndx = -1;
    /* find the first video stream index . Also there is an API available to do the below operations */
    for (int i = 0; i < iFormatCtx->nb_streams; i++) // find video stream position/index.
        if (iFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            AVCodecParameters *iLocalCodecParams = nullptr;
            /* Codec params describe the information relative to that stream */
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
        throw runtime_error("\nunable to find the video stream index. (-1)");
    }
    DecoderCodecCtx = avcodec_alloc_context3(DecoderCodec);
    if (!DecoderCodecCtx) {
        throw runtime_error("Failed to allocate memory for AVCodecContext\n");
    }
    value = avcodec_parameters_to_context(DecoderCodecCtx, DecoderCodecParams);
    if (value < 0) {
        throw runtime_error("Failed to copy decoderCodec params to decoderCodecCtx\n");
    }
    av_opt_set(DecoderCodecCtx, "preset", "ultrafast", 0);

    value = avcodec_open2(DecoderCodecCtx, DecoderCodec, nullptr);
    if (value < 0) {
        throw runtime_error("Failed to open DecoderCodec!\n");
    }
    return 1;
}

/* Allocates and prepares the context for the output file based on the file extension */
int Recorder::InitOutputFile() {
    value = 0;
    oFormatCtx = nullptr;

    /* Get the output format from the list of registered formats which best matches 'output_file' */
    oformat = const_cast<AVOutputFormat *>(av_guess_format(nullptr, output_file, nullptr));
    if (!oformat) {
        throw runtime_error("Can't create output format!\n");
    }

    /* Allocates the format I/O context for the given 'output_file' and 'oformat' internal parameters */
    avformat_alloc_output_context2(&oFormatCtx, oformat, nullptr, output_file);
    if (!oFormatCtx) {
        throw runtime_error("Can't create output context\n");
    }
    return 1;
}

void Recorder::lastSetUp() {
    if (!(oformat->flags & AVFMT_NOFILE)) {
        if ((value = avio_open(&oFormatCtx->pb, output_file, AVIO_FLAG_WRITE)) < 0) {
            throw runtime_error("Failed to open output file\n");
        }
    }
    /* Allocate stream private data and write the stream header into the output file */
    if ((value = avformat_write_header(oFormatCtx, nullptr)) < 0) {
        throw runtime_error("Failed to write header\n");
    }
}

int Recorder::SetUp_VideoEncoder() {
    EncoderCodec = avcodec_find_encoder(oformat->video_codec);
    if (!EncoderCodec) {
        throw runtime_error("Can't create EncoderCodec\n");
    }

    stream = avformat_new_stream(oFormatCtx, EncoderCodec);
    if (!stream) {
        throw runtime_error("Can't create stream for this format\n");
    }

    EncoderCodecCtx = avcodec_alloc_context3(EncoderCodec);
    if (!EncoderCodecCtx) {
        throw runtime_error("Can't create EncoderCodec Context\n");
    }

    stream->codecpar->codec_id = oformat->video_codec;
    stream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
    stream->codecpar->width = width;
    stream->codecpar->height = height;
    stream->codecpar->format = AV_PIX_FMT_YUV420P;
    stream->codecpar->bit_rate = 4000;
    avcodec_parameters_to_context(EncoderCodecCtx, stream->codecpar);
    /* This the unit of time in seconds in terms of which frames are represented
     * For fixed fps content, timebase should be 1/framerate
     * */
    EncoderCodecCtx->time_base.num = 1;
    EncoderCodecCtx->time_base.den = fps;
    /* B‑frames can use both previous and forward frames for data reference to get the highest amount of data compression */
    EncoderCodecCtx->max_b_frames = 10;
    /*GOP is the distance between two keyframes (I-frames). A typical GOP contains a repeating pattern of B and P frames
     * sandwiched between I frames. A longer GOP length will compress the content more efficiently, providing higher
     * video quality at lower bit rates*/
    EncoderCodecCtx->gop_size = fps * 2;
    /* Allows the encoder to achieve a certain output quality when output file size is not important */
    EncoderCodecCtx->qmin = 0;
    EncoderCodecCtx->qmax = 5;

    if (stream->codecpar->codec_id == AV_CODEC_ID_H264) {
        av_opt_set(EncoderCodecCtx, "preset", "ultrafast", 0);
    }

    if (oFormatCtx->oformat->flags & AVFMT_GLOBALHEADER) {
        oFormatCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    value = avcodec_open2(EncoderCodecCtx, EncoderCodec, nullptr);
    if (value < 0) {
        throw runtime_error("Failed to open Encoder Codec\n");
    }

    /* find free stream index */
    outVideoStreamIndx = -1;
    for (int i = 0; i < oFormatCtx->nb_streams; i++) {
        if (oFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            outVideoStreamIndx = i;
        }
    }
    if (outVideoStreamIndx < 0) {
        throw runtime_error("Error: cannot find a free stream for video on the output");
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
        throw runtime_error("Cannot allocate memory for audio input context\n");
    }

    av_dict_set(&AudioOptions, "async", "25", 0);
#ifdef WIN32
    audioIFormat = const_cast<AVInputFormat *>(av_find_input_format("dshow"));
    av_dict_set(&AudioOptions, "sample_rate", "44100", 0);
    /* Windows = Microfono (Realtek Audio) */
    value = avformat_open_input(&AudioInFCtx, audio_device.c_str(), audioIFormat, &AudioOptions);
    if (value < 0) {
        throw runtime_error("Cannot open Audio input Windows\n");
    }
#elif defined linux
    audioIFormat = const_cast<AVInputFormat *>(av_find_input_format("alsa"));
    av_dict_set(&AudioOptions, "sample_rate", "44100", 0);
    /* Linux = hw:0,0 */
    value = avformat_open_input(&AudioInFCtx, audio_device.c_str(), audioIFormat, &AudioOptions);
    if(value < 0 ){
        throw runtime_error("Cannot open Audio input\n");
    }
#endif

    return 1;
}

int Recorder::InitializeAudioDecoder() {
    value = avformat_find_stream_info(AudioInFCtx, &AudioOptions);
    if (value < 0) {
        throw runtime_error("Failed to retrieve input stream info\n");
    }
    value = av_find_best_stream(AudioInFCtx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (value < 0) {
        throw runtime_error("Failed to retrieve input audio stream info\n");
    }

    AudioStreamIndx = -1;
    /* find the first video stream index . Also there is an API available to do the below operations */
    for (int i = 0; i < AudioInFCtx->nb_streams; i++) {// find audio stream position/index.
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
        throw runtime_error("\nunable to find the Audio stream index. (-1)");
    }
    AudioDecoderCodecCtx = avcodec_alloc_context3(AudioDecoderCodec);
    if (!AudioDecoderCodecCtx) {
        throw runtime_error("Failed to allocate memory for Audio AVCodecContext\n");
    }
    value = avcodec_parameters_to_context(AudioDecoderCodecCtx, AudioDecoderCodecParams);
    if (value < 0) {
        throw runtime_error("Failed to copy Audio decoderCodec params to Audio decoderCodecCtx\n");
    }
    value = avcodec_open2(AudioDecoderCodecCtx, AudioDecoderCodec, nullptr);
    if (value < 0) {
        throw runtime_error("Failed to open Audio DecoderCodec!\n");
    }
    return 1;

}

int Recorder::SetUp_AudioEncoder() {
    AudioEncoderCodec = nullptr;

    AudioEncoderCodec = avcodec_find_encoder(oformat->audio_codec);
    if (!AudioEncoderCodec) {
        throw runtime_error("Can't create EncoderCodec\n");
    }

    /* Returns the first available "slot" in oFormatCtx->streams (here = ->streams[1] */
    audio_stream = avformat_new_stream(oFormatCtx, AudioEncoderCodec);
    if (!audio_stream) {
        throw runtime_error("Can't create stream for this format\n");
    }

    AudioEncoderCodecCtx = avcodec_alloc_context3(AudioEncoderCodec);
    if (!AudioEncoderCodecCtx) {
        throw runtime_error("Can't create EncoderCodec Context\n");
    }

    /* The sample rate is the number of times in a second an audio sample is taken: the number of instances per
    second that recording equipment is transforming sound into data */
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
    /* Bit rate is the number of bits that are recorded over each unit of time */
    AudioEncoderCodecCtx->bit_rate = 128000; //96000
    /* Time unit for which a sample is taken */
    AudioEncoderCodecCtx->time_base = (AVRational) {1, AudioDecoderCodecCtx->sample_rate};
    AudioEncoderCodecCtx->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;

    audio_stream->time_base = (AVRational) {1, AudioDecoderCodecCtx->sample_rate};
    /** Already in Video Encoder SetUp **/
    if (oFormatCtx->oformat->flags & AVFMT_GLOBALHEADER) {
        oFormatCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    if (avcodec_open2(AudioEncoderCodecCtx, AudioEncoderCodec, nullptr) < 0) {
        throw runtime_error("Error in opening the audio encoder codec!\n");
    }

    /* find free stream index */
    outAudioStreamIndx = -1;
    for (int i = 0; i < oFormatCtx->nb_streams; i++) {
        if (oFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_UNKNOWN) {
            outAudioStreamIndx = i;
        }
    }
    if (outAudioStreamIndx < 0) {
        throw runtime_error("Error: cannot find a free stream for audio on the output");
    }
    /** Copy EncoderCodecCtx codec parameters into audio_stream (= oFormatCtx->streams[1]) **/
    avcodec_parameters_from_context(audio_stream->codecpar, AudioEncoderCodecCtx);

    return 1;
}

/**         DECODING & ENCODING          **/
void Recorder::PrepareVideoDecEnc() {
    pFrame = av_frame_alloc();
    if (pFrame == nullptr) {
        throw runtime_error("Could not allocate frame \n");
    }

    sws_ctx = nullptr;
    /* set up our SWSContext to convert the image data to YUV420 */
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

    if (sws_ctx == nullptr)
        throw runtime_error("Error during allocation of sws_ctx");

    int numBytes;
    uint8_t *buffer = nullptr;

    /* Return the size in bytes of the amount of data required to store an image with the given parameters */
    numBytes = av_image_get_buffer_size(
            AV_PIX_FMT_YUV420P,
            DecoderCodecCtx->width,
            DecoderCodecCtx->height,
            32
    );

    if (numBytes < 0)
        throw runtime_error("Cannot get a buffer size\n");

    buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));

    if (buffer == nullptr)
        throw runtime_error("Cannot allocate the buffer\n");

    pict = av_frame_alloc();

    if (pict == nullptr)
        throw runtime_error("Cannot allocate pict\n");

    int err = av_image_fill_arrays(
            pict->data,
            pict->linesize,
            buffer,
            AV_PIX_FMT_YUV420P,
            DecoderCodecCtx->width,
            DecoderCodecCtx->height,
            32
    );

    if (err < 0) {
        av_frame_unref(pict);
        av_frame_free(&pict);
        av_free(&buffer);
        throw runtime_error("Cannot fill image\n");
    }

}

int Recorder::PrepareAudioDecEnc(AVAudioFifo **audio_fifo, AVPacket **pAudioPacket, AVPacket **outAudioPacket,
                                 AVFrame **pAudioFrame, AVFrame **outAudioFrame, SwrContext **resampleContext) {
    /* Create the FIFO buffer based on the specified output sample format. */
    if (!(*audio_fifo = av_audio_fifo_alloc(AudioEncoderCodecCtx->sample_fmt,
                                            AudioEncoderCodecCtx->channels, 1))) {
        throw runtime_error("Could not allocate FIFO\n");
    }

    /* allocate space for a packet */
    *pAudioPacket = (AVPacket *) av_malloc(sizeof(AVPacket));
    if (!*pAudioPacket) {
        throw runtime_error("Cannot allocate an AVPacket for encoded audio\n");
    }
    *pAudioPacket = av_packet_alloc();
    if (*pAudioPacket == nullptr) {
        throw runtime_error("Cannot allocate an AVPacket for encoded audio\n");
    }

    /* allocate space for a frame */
    *pAudioFrame = av_frame_alloc();
    if (!*pAudioFrame) {
        throw runtime_error("Cannot allocate an AVFrame for encoded audio\n");
    }

    *outAudioFrame = av_frame_alloc();
    if (!*outAudioFrame) {
        throw runtime_error("Cannot allocate an AVFrame for encoded audio\n");
    }

    *outAudioPacket = (AVPacket *) av_malloc(sizeof(AVPacket));
    if (!outAudioPacket) {
        throw runtime_error("Cannot allocate an AVPacket for encoded video\n");
    }

    /* init the resampler */
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

    if (!*resampleContext) {
        throw runtime_error("Cannot allocate the resample context\n");
    }
    if ((swr_init(*resampleContext)) < 0) {
        swr_free(resampleContext);
        throw runtime_error("Could not open resample context\n");
    }
    return 0;
}

bool Recorder::isEndVideo() {
    return videoEnd;
}

void Recorder::endVideo() {
    unique_lock<mutex> lg_videoEnd(_videoEnd);
    videoEnd = true;
    lg_videoEnd.unlock();
    if (audio)
        cv_audio.notify_all();
}

void Recorder::synchWithAudio() {
    if (audio) {
        unique_lock<mutex> ul_videoSynch(_sync);
        videoReady = true;
        std::cout << "Video Ready" << endl;
        cv_video.wait(ul_videoSynch, [this]() { return audioReady; });
        audioReady = false;
        ul_videoSynch.unlock();
        cv_audio.notify_all();
        std::cout << "Video Started" << endl;
    }
}

void Recorder::synchWithVideo() {

    unique_lock<mutex> ul_audioSynch(_sync);
    audioReady = true;
    cout << "Audio Ready" << endl;
    cv_audio.wait(ul_audioSynch, [this]() { return videoReady; });
    videoReady = false;
    ul_audioSynch.unlock();
    cv_video.notify_all();
    cout << "Audio Started" << endl;
}

void Recorder::acquireVideoFrames() {
    AVPacket *pPacket;
    int retry = 2;
    while (retry) {
        try {
            PrepareVideoDecEnc();
            break;
        } catch (const std::exception &e) {
            retry--;
            if (!retry) {
                cerr << e.what() << endl;
                exit(-3);
            }
        }
    }

    int retValue;
    int remainingPackets = 20; // to add the packets after end is pressed

    avformat_close_input(&iFormatCtx);
    if (iFormatCtx != nullptr) {
        cerr << "Failed to clear the video format context (clearing internal buffer)\n" << endl;
        exit(-5);
    }

    try {
        OpenVideoDevice();
    } catch (const std::exception &e) {
        cerr << e.what() << endl;
        exit(-2);
    }
    if (audio)
        synchWithAudio();
    else
        cout << "Video Started" << endl;

    try {
        while (remainingPackets > 0) {
            unique_lock<mutex> ul_pause(_lock);
            if (!crashed) {
                if (pauseRecording) {
                    avformat_close_input(&iFormatCtx);
                    if (iFormatCtx != nullptr) {
                        throw runtime_error("Unable to close the video input\n");
                    }
                    cv_pause.wait(ul_pause, [this]() { return !pauseRecording || stopRecording; });
                    if (stopRecording) {
                        remainingPackets = 0;
                        ul_pause.unlock();
                        cv_pause.notify_all();
                        break;
                    }
                    ul_pause.unlock();
                    cv_pause.notify_all();

                    try {
                        OpenVideoDevice();
                    } catch (const std::exception &e) {
                        throw e;
                    }

                    if (audio)
                        synchWithAudio();
                    else
                        cout << "Video Started" << endl;
                } else
                    ul_pause.unlock();

                unique_lock<mutex> ul_stop(_lock);
                if (stopRecording) {
                    if (retValue >= 0) {
                        remainingPackets--;
                    }
                }
                ul_stop.unlock();

                pPacket = av_packet_alloc();
                if (pPacket == nullptr) {
                    throw runtime_error("Could not alloc packet,\n");
                }

                retValue = av_read_frame(iFormatCtx, pPacket);
                if (retValue >= 0 && pPacket->size > 0) {
                    unique_lock<mutex> ul_queue(queue_lock);
                    ReadRawPacketsQ.push(pPacket);
                    ul_queue.unlock();
                } // end of prim while
            } else {
                remainingPackets = 0;
                ul_pause.unlock();
            }
        }
    } catch (const std::exception &e) {
        cerr << e.what() << endl;
        outFile << e.what() << endl;
//        cout << "VIDEO ENDING" << endl;
        unique_lock<mutex> lk_stop(_lock);
        stopRecording = true;
        lk_stop.unlock();
        endVideo();
        throw;
    }

    endVideo();
//    cout << "VIDEO ENDING" << endl;
}

void Recorder::encodeDecodeVideoStream() {
    AVPacket *pPacket;
    AVPacket *outPacket;
    int i = 1, j = 1, retSend, retReceive;

    try {
        while (true) {
            unique_lock<mutex> ul_queue(queue_lock);
            if (!ReadRawPacketsQ.empty()) {
                pPacket = ReadRawPacketsQ.front();
                ReadRawPacketsQ.pop();
                ul_queue.unlock();
                if (pPacket->stream_index == VideoStreamIndx) {
                    /* Supply raw packet data as input to a decoder */
                    retSend = avcodec_send_packet(DecoderCodecCtx, pPacket);
                    av_packet_unref(pPacket);
                    av_packet_free(&pPacket);
                    if (retSend < 0) {
                        throw runtime_error("Error sending packet for decoding.\n");
                    }

                }
                /* Returns decoded output data (frame) from the decoder*/
                retReceive = avcodec_receive_frame(DecoderCodecCtx, pFrame);
                if (retReceive == 0) {
                    /* We change the conversion format to PIX_FMT_YUV420P, and we
                     use sws_scale just like before. */
                    sws_scale(sws_ctx, pFrame->data, pFrame->linesize, 0, DecoderCodecCtx->height, pict->data,
                              pict->linesize);

                    pict->pts = (int64_t) j * stream->time_base.den / (int64_t) fps;
                    j++;
                    pict->format = EncoderCodecCtx->pix_fmt;
                    pict->width = EncoderCodecCtx->width;
                    pict->height = EncoderCodecCtx->height;

                    /* Send a raw video frame to the encoder */
                    retSend = avcodec_send_frame(EncoderCodecCtx, pict);
                    if (retSend >= 0) {
                        outPacket = av_packet_alloc();
                        if (outPacket == nullptr) {
                            throw runtime_error("Cannot allocate packet!\n");
                        }
                        outPacket->data = nullptr;
                        outPacket->size = 0;
                        outPacket->flags |= AV_PKT_FLAG_KEY;

                        /* Read encoded data from the encoder */
                        retReceive = avcodec_receive_packet(EncoderCodecCtx, outPacket);
                        if (retReceive == 0) {
                            outPacket->pts = (int64_t) i * stream->time_base.den / (int64_t) fps;
                            outPacket->dts = (int64_t) i * stream->time_base.den / (int64_t) fps;
                            outPacket->stream_index = outVideoStreamIndx;

                            {
                                lock_guard<mutex> w_lk(write_lock);
                                outFile << "Write video frame " << "(size= " << outPacket->size / 1000 << " )"
                                        << " PTS = "
                                        << outPacket->pts << endl;
                                /* Write a packet to an output media file */
                                if (av_interleaved_write_frame(oFormatCtx, outPacket) != 0) {
                                    cout << "\nerror in writing video frame";
                                }
                            }
                            i++;
                        }
                        av_packet_unref(outPacket);
                    }
                } else {
                    throw runtime_error("Error while decoding.\n");
                }
            } else {
                ul_queue.unlock();
                unique_lock<mutex> ul_end(_lock);
                if (stopRecording) {
                    ul_end.unlock();
                    ul_queue.lock();
                    if (ReadRawPacketsQ.empty() && isEndVideo()) {
                        ul_queue.unlock();
                        /** FLUSHING DECODER **/
                        AVPacket *pkt;
                        pkt = av_packet_alloc();
                        pkt->data = nullptr;
                        pkt->size = 0;
                        for (int k = i;; k++) {
                            avcodec_send_frame(EncoderCodecCtx, nullptr);
                            /* AVERROR_EOF: the encoder has been fully flushed, and there will be no more output packets
                             * so the else branch will let us exit */
                            if (avcodec_receive_packet(EncoderCodecCtx, pkt) == 0) {
                                pkt->pts = (int64_t) k * stream->time_base.den / (int64_t) fps;
                                pkt->dts = (int64_t) k * stream->time_base.den / (int64_t) fps;
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
                } else {
                    ul_end.unlock();
                }
            }
        }

    } catch (const exception &e) {
        cerr << e.what() << endl;
        outFile << e.what() << endl;
        if (outPacket != nullptr)
            av_packet_free(&outPacket);
        av_frame_free(&pFrame);
        av_frame_free(&pict);
        if (pPacket != nullptr)
            av_packet_free(&pPacket);
        sws_freeContext(sws_ctx);
        //        cout << "DECODING-ENCODING ENDING" << endl;
        throw;
    }

//    cout << "DECODING-ENCODING ENDING" << endl;

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

    int retry = 2;
    while (retry) {
        try {
            PrepareAudioDecEnc(&audio_fifo, &pAudioPacket, &outAudioPacket, &pAudioFrame, &outAudioFrame,
                               &resampleContext);
            break;
        } catch (const std::exception &e) {
            retry--;
            if (!retry) {
                cerr << e.what() << endl;
                exit(-4);
            }
        }
    }


    int j = 0;

    /*  clear the internal buffer because as soon as the device is opened it starts buffering audio frames */
    avformat_close_input(&AudioInFCtx);
    if (AudioInFCtx != nullptr) {
        cerr << "Failed to clear the audio format context (clearing internal buffer)\n" << endl;
        exit(-5);
    }
    try {
        OpenAudioDevice();
    } catch (const std::exception &e) {
        cerr << e.what() << endl;
        exit(-2);
    }

    synchWithVideo();
    try {
        while (true) {
            unique_lock<mutex> ul_pause(_lock);
            if (!crashed) {
                if (pauseRecording) {
                    avformat_close_input(&AudioInFCtx); //close and clear the internal buffer
                    if (AudioInFCtx != nullptr) {
                        throw runtime_error("Failed to clear the audio format context (clearing internal buffer)\n");
                    }

                    cv_pause.wait(ul_pause, [this]() { return !pauseRecording || stopRecording; });

                    if (stopRecording) {
                        ul_pause.unlock();
                        cv_pause.notify_all();
                        break;
                    }
                    ul_pause.unlock();
                    cv_pause.notify_all();

                    //reopen input audio
                    try {
                        OpenAudioDevice();
                    } catch (const std::exception &e) {
                        throw e;
                    }
                    synchWithVideo();
                } else
                    ul_pause.unlock();

                unique_lock<mutex> ul_stopAudio(_lock);
                if (stopRecording) {
                    ul_stopAudio.unlock();
                    /* wait for video acquire thread to terminate */
                    unique_lock<mutex> ul_audioSynch(_videoEnd);
                    cv_audio.wait(ul_audioSynch, [this]() { return isEndVideo(); });
                    ul_audioSynch.unlock();
                    break;
                }
                ul_stopAudio.unlock();

                if (av_read_frame(AudioInFCtx, pAudioPacket) >= 0) {
                    if (pAudioPacket->stream_index == AudioStreamIndx) {
                        /* Convert timestamps/durations in a packet from one timebase to another */
                        av_packet_rescale_ts(outAudioPacket, AudioInFCtx->streams[AudioStreamIndx]->time_base,
                                             AudioDecoderCodecCtx->time_base);
                        /* Send to decoder */
                        value = avcodec_send_packet(AudioDecoderCodecCtx, pAudioPacket);
                        if (value < 0) {
                            throw runtime_error("Cannot decode current audio packet\n");
                        }

                        while (value >= 0) {
                            value = avcodec_receive_frame(AudioDecoderCodecCtx, pAudioFrame);

                            /* AVERROR(EAGAIN): output is not available in this state - user must try to send new input
                             * AVERROR_EOF: the decoder has been fully flushed
                             */
                            if (value == AVERROR(EAGAIN) || value == AVERROR_EOF) {
                                break;
                            } else if (value < 0) {
                                throw runtime_error("Error while decoding.\n");
                            }

                            /* pts of the first frame of the stream */
                            if (oFormatCtx->streams[outAudioStreamIndx]->start_time <= 0) {
                                oFormatCtx->streams[outAudioStreamIndx]->start_time = pAudioFrame->pts;
                            }

                            /* Allocate as many pointers as there are audio channels.
                            * Each pointer will later point to the audio samples of the corresponding
                            * channels (although it may be NULL for interleaved formats).
                            */

                            if (!(resampledData = (uint8_t **) calloc(AudioEncoderCodecCtx->channels,
                                                                      sizeof(*resampledData)))) {
                                throw runtime_error("Could not allocate converted input sample pointers\n");
                            }

                            /* Allocate memory for the samples of all channels in one consecutive
                            * block for convenience. */
                            if (av_samples_alloc(resampledData, nullptr,
                                                 AudioEncoderCodecCtx->channels,
                                                 pAudioFrame->nb_samples,
                                                 AudioEncoderCodecCtx->sample_fmt, 0) < 0) {
                                throw runtime_error("Cannot allocate memory for samples\n");
                            }

                            /* convert */
                            swr_convert(resampleContext,
                                        resampledData, pAudioFrame->nb_samples,
                                        (const uint8_t **) pAudioFrame->extended_data, pAudioFrame->nb_samples);

                            /* Make the FIFO as large as it needs to be to hold both,
                            * the old and the new samples. */
                            if ((value = av_audio_fifo_realloc(audio_fifo,
                                                               av_audio_fifo_size(audio_fifo) +
                                                               pAudioFrame->nb_samples)) <
                                0) {
                                throw runtime_error("Could not reallocate FIFO\n");
                            }

                            /* Store the new samples in the FIFO buffer. */

                            if (av_audio_fifo_write(audio_fifo, (void **) resampledData, pAudioFrame->nb_samples) <
                                pAudioFrame->nb_samples) {
                                throw runtime_error("Could not write data to FIFO\n");
                            }

                            outAudioPacket = av_packet_alloc();
                            outAudioPacket->data = nullptr;
                            outAudioPacket->size = 0;

                            outAudioFrame = av_frame_alloc();
                            if (!outAudioFrame) {
                                throw runtime_error("Cannot allocate an AVFrame for encoded audio\n");
                            }

                            outAudioFrame->nb_samples = AudioEncoderCodecCtx->frame_size;
                            outAudioFrame->channel_layout = AudioEncoderCodecCtx->channel_layout;
                            outAudioFrame->format = AudioEncoderCodecCtx->sample_fmt;
                            outAudioFrame->sample_rate = AudioEncoderCodecCtx->sample_rate;
                            av_frame_get_buffer(outAudioFrame, 0);

                            while (av_audio_fifo_size(audio_fifo) >=
                                   AudioEncoderCodecCtx->frame_size) { /* frame_size = number of samples per channel in an audio frame */
                                value = av_audio_fifo_read(audio_fifo, (void **) outAudioFrame->data,
                                                           AudioEncoderCodecCtx->frame_size);
                                outAudioFrame->pts = j;
                                j += outAudioFrame->nb_samples;
                                value = avcodec_send_frame(AudioEncoderCodecCtx, outAudioFrame);
                                if (value == AVERROR(EAGAIN) || value == AVERROR_EOF)
                                    break;
                                if (value < 0) {
                                    throw runtime_error("Cannot encode current audio packet\n");
                                }
                                while (value >= 0) {
                                    value = avcodec_receive_packet(AudioEncoderCodecCtx, outAudioPacket);
                                    if (value == AVERROR(EAGAIN) || value == AVERROR_EOF)
                                        break;
                                    else if (value < 0) {
                                        throw runtime_error("Error during encoding\n");
                                    }

                                    av_packet_rescale_ts(outAudioPacket, AudioEncoderCodecCtx->time_base,
                                                         audio_stream->time_base);
                                    outAudioPacket->stream_index = outAudioStreamIndx;

                                    {
                                        lock_guard<mutex> w_lk(write_lock);
                                        outFile << "Write audio frame " << j << "(size= " << outAudioPacket->size / 1000
                                                << " )" << endl;

                                        if (av_write_frame(oFormatCtx, outAudioPacket) != 0) {
                                            cerr << "Error in writing audio frame\n";
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
            } else {
                ul_pause.unlock();
                break;
            }

        }
    } catch (const exception &e) {
        cerr << e.what() << endl;
        outFile << e.what() << endl;
        if (outAudioPacket != nullptr) {
            av_packet_unref(outAudioPacket);
            av_packet_free(&outAudioPacket);
        }
        if (outAudioFrame != nullptr)
            av_frame_free(&outAudioFrame);

        av_audio_fifo_free(audio_fifo);
        swr_free(&resampleContext);
//        cout << "AUDIO ENDING" << endl;
        throw;
    }
    av_audio_fifo_free(audio_fifo);
    swr_free(&resampleContext);
//    cout << "AUDIO ENDING" << endl;
    return 1;
}

void Recorder::startCapture() {
    av_log_set_level(AV_LOG_FATAL);
    auto acquireFrames = [this]() -> void {
        try {
            acquireVideoFrames();
        } catch (const exception &e) {
            unique_lock<mutex> lk_stop(_lock);
            stopRecording = true;
            endVideo();
            crashed = true;
            lk_stop.unlock();
            cv_pause.notify_all();
        }
    };
    auto encodeDecodeVideo = [this]() -> void {
        try {
            encodeDecodeVideoStream();
        } catch (const exception &e) {
            unique_lock<mutex> lk_stop(_lock);
            stopRecording = true;
            crashed = true;
            lk_stop.unlock();
            cv_pause.notify_all();
        }
    };
    auto captureAudio = [this]() -> void {
        try {
            AudioDecEnc();
        } catch (const exception &e) {
            unique_lock<mutex> lk_stop(_lock);
            stopRecording = true;
            crashed = true;
            lk_stop.unlock();
            cv_pause.notify_all();
        }
    };

    auto manageCapture = [this]() -> void {
        captureMenu();
    };

    /* 1. Try to open and locate the output file where the recording will be saved */
    try {
        InitOutputFile();
    } catch (const std::exception &e) {
        cerr << e.what() << endl;
        exit(-1);
    }

    try {
        /* Open the device for recording the desktop using gdigrab or x11grab */
        OpenVideoDevice();
        InitializeVideoDecoder();
        SetUp_VideoEncoder();

        if (audio) {
            /* Open the device for recording the desktop using dshow or alsa */
            OpenAudioDevice();
            InitializeAudioDecoder();
            SetUp_AudioEncoder();
        }

        /*
         * 1. Open output file
         * 2. Write header in file, to be done only once after all devices setup.
         * */
        lastSetUp();
    } catch (const std::exception &e) {
        cerr << e.what() << endl;
        outFile << e.what() << endl;
        exit(-2);
    }

    thread t4(manageCapture);

    unique_lock<mutex> ul_start(_lock);
    cv_start.wait(ul_start, [this]() { return startRecording || exited; });
    if (exited && !startRecording) {
        t4.join();
        ul_start.unlock();
        return;
    }
    ul_start.unlock();


    /* Divided Video processing in 2 threads because, by sequentially capturing raw frames from the device
     * and processing them, causes frames drop since the CPU is not fast enough to decode/encode a
     * packet before another packet would come. So we implemented an internal queue which is filled with
     * arriving packets (containing frames), giving enough time to the encodeDecode thread to process each of them.
     * */

    thread t1(acquireFrames);
    thread t2(encodeDecodeVideo);
    thread t3;
    if (audio)
        t3 = thread(captureAudio);

    t1.join();
    t2.join();
    if (audio) {
        t3.join();
    }
    if (crashed) {
        cerr << "Fatal Error Press 1 to Exit and Save" << endl;
    }
    t4.join();
    cout << "...EXITING" << endl;
}