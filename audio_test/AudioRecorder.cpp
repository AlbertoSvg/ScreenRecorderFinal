//
// Created by alber on 08/11/2021.
//

#include "AudioRecorder.h"


/** Initialize resources **/
AudioRecorder::AudioRecorder() {
    avdevice_register_all();
}

/** Destroy resources **/
AudioRecorder::~AudioRecorder() {

}


int AudioRecorder::InitOutputFile() {
    value = 0;
    output_file = "../audio_test/output.mp4";
    oFormatCtx = NULL;

    oformat = av_guess_format(NULL, output_file, NULL);
    if (!oformat) {
        cout << "Can't create output format!\n";
        exit(1);
    }

    avformat_alloc_output_context2(&oFormatCtx, oformat, NULL, output_file);
    if (!oFormatCtx) {
        cout << "Can't create output context\n";
        exit(1);
    }
    return 1;
}


/**             AUDIO            **/

int AudioRecorder::OpenAudioDevice() {
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
    av_dict_set(&AudioOptions, "async", "1", 0);
    if (value < 0) {
        cout << "Cannot set Audio Options\n";
        exit(1);
    }
#ifdef WIN32
    audioIFormat = av_find_input_format("dshow");
    value = avformat_open_input(&AudioInFCtx, "audio=Microfono (Realtek Audio)", audioIFormat, NULL);
    if (value < 0) {
        cout << "Cannot open Audio input\n";
        exit(10);
    }
#elif defined linux
    audioIFormat = av_find_input_format("alsa");
    value = avformat_open_input(&AudioInFCtx, "hw:0", audioIFormat, NULL);
    if(value < 0 ){
        cout << "Cannot open Audio input\n";
        exit(10);
    }
#endif

    return 1;
}

int AudioRecorder::InitializeAudioDecoder() {
    value = avformat_find_stream_info(AudioInFCtx, NULL);
    if (value < 0) {
        cout << "Failed to retrieve input stream info\n";
        exit(1);
    }
    value = av_find_best_stream(AudioInFCtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (value < 0) {
        cout << "Failed to retrieve input audio stream info\n";
        exit(1);
    }
    av_dump_format(AudioInFCtx, 0, ":0.0", 0);

    AudioStreamIndx = -1;
    /* find the first video stream index . Also there is an API available to do the below operations */
    for (int i = 0; i < AudioInFCtx->nb_streams; i++) {// find video stream position/index.
        if (AudioInFCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            AVCodecParameters *iLocalCodecParams = NULL;
            iLocalCodecParams = AudioInFCtx->streams[i]->codecpar;

            const AVCodec *iLocalCodec = NULL;
            iLocalCodec = avcodec_find_decoder(iLocalCodecParams->codec_id);
            if (iLocalCodec == NULL) {
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
    value = avcodec_open2(AudioDecoderCodecCtx, AudioDecoderCodec, NULL);
    if (value < 0) {
        cout << "Failed to open Audio DecoderCodec!\n";
        exit(1);
    }
    return 1;

}

int AudioRecorder::SetUp_AudioEncoder() {
    AudioEncoderCodec = NULL;

    /** Returns the first available "slot" in oFormatCtx->streams (here = ->streams[1] **/
    audio_stream = avformat_new_stream(oFormatCtx, nullptr);
    if (!audio_stream) {
        cout << "Can't create stream for this format\n";
        exit(1);
    }

    AudioEncoderCodec = avcodec_find_encoder(oformat->audio_codec);
    if (!AudioEncoderCodec) {
        cout << "Can't create EncoderCodec\n";
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
                break;
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

    audio_stream->time_base = AudioEncoderCodecCtx->time_base;
    /** Already in Video Encoder SetUp **/
    if (oFormatCtx->oformat->flags & AVFMT_GLOBALHEADER) {
        oFormatCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    if (avcodec_open2(AudioEncoderCodecCtx, AudioEncoderCodec, nullptr) < 0) {
        cerr << "Error in opening the audio encoder codec!\n";
        exit(1);
    }

    //find free stream index
    AudioStreamIndx = -1;
    for (int i = 0; i < oFormatCtx->nb_streams; i++) {
        if (oFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_UNKNOWN) {
            AudioStreamIndx = i;
        }
    }
    if (AudioStreamIndx < 0) {
        cerr << "Error: cannot find a free stream for audio on the output" << endl;
        exit(1);
    }
    /** Copy EncoderCodecCtx codec parameters into audio_stream (= oFormatCtx->streams[1]) **/
    avcodec_parameters_from_context(oFormatCtx->streams[AudioStreamIndx]->codecpar, AudioEncoderCodecCtx); //TODO: PROBLEMA
    //avcodec_parameters_from_context(audio_stream->codecpar, AudioEncoderCodecCtx); //TODO: PROBLEMA

    if (!(oformat->flags & AVFMT_NOFILE)) {
        if ((value = avio_open(&oFormatCtx->pb, output_file, AVIO_FLAG_WRITE)) < 0) {
            cout << "Failed to open output file\n";
            exit(1);
        }
    }

    if ((value = avformat_write_header(oFormatCtx, NULL)) < 0) {
        cout << "Failed to write header\n";
        exit(1);
    }
    return 1;
}


int AudioRecorder::PrepareAudioDecEnc()
{
    /* Create the FIFO buffer based on the specified output sample format. */
    if (!(audio_fifo = av_audio_fifo_alloc(AudioEncoderCodecCtx->sample_fmt,
                                     AudioEncoderCodecCtx->channels, 1))) {
        cerr<<"Could not allocate FIFO\n";
        return AVERROR(ENOMEM);
    }

    //allocate space for a packet
    pAudioPacket = (AVPacket*)av_malloc(sizeof(AVPacket));
    if (!pAudioPacket) {
        cerr << "Cannot allocate an AVPacket for encoded audio" << endl;
        exit(1);
    }
    pAudioPacket = av_packet_alloc();

    //allocate space for a frame
    pAudioFrame = av_frame_alloc();
    if (!pAudioFrame) {
        cerr << "Cannot allocate an AVFrame for encoded audio" << endl;
        exit(1);
    }

    outAudioFrame = av_frame_alloc();
    if (!outAudioFrame) {
        cerr << "Cannot allocate an AVFrame for encoded audio" << endl;
        exit(1);
    }

    outAudioPacket = (AVPacket*)av_malloc(sizeof(AVPacket));
    if (!outAudioPacket) {
        cerr << "Cannot allocate an AVPacket for encoded video" << endl;
        exit(1);
    }

    //init the resampler
    resampleContext = nullptr;
    resampleContext = swr_alloc_set_opts(resampleContext,
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
    av_assert0(AudioEncoderCodecCtx->sample_rate == AudioDecoderCodecCtx->sample_rate);
    if (!resampleContext) {
        cerr << "Cannot allocate the resample context" << endl;
        exit(1);
    }
    if ((swr_init(resampleContext)) < 0) {
        cerr<<"Could not open resample context\n";
        swr_free(&resampleContext);
        exit(1);
    }


    return 0;
}

int AudioRecorder::DecEnc() {
    uint8_t** resampledData;

    PrepareAudioDecEnc();

    int maxFramesToDecode = 200;

    int i = 0, j = 0;
    while (av_read_frame(AudioInFCtx, pAudioPacket) >= 0) {
        if (pAudioPacket->stream_index == AudioStreamIndx) {
            av_packet_rescale_ts(outAudioPacket,AudioInFCtx->streams[AudioStreamIndx]->time_base, AudioDecoderCodecCtx->time_base);
            value = avcodec_send_packet(AudioDecoderCodecCtx, pAudioPacket);
            if (value < 0) {
                cerr << "Cannot decode current audio packet"<< value << endl;
                return -1;
            }

            while (value >= 0) {
                value = avcodec_receive_frame(AudioDecoderCodecCtx, pAudioFrame);

                if (value == AVERROR(EAGAIN) || value == AVERROR_EOF) {
                    break;
                } else if (value < 0) {
                    cerr<<"Error while decoding.\n";
                    exit(1);
                }

                if (oFormatCtx->streams[AudioStreamIndx]->start_time <= 0) {
                    oFormatCtx->streams[AudioStreamIndx]->start_time = pAudioFrame->pts;
                }

                //initConvertedSamples

                /* Allocate as many pointers as there are audio channels.
                * Each pointer will later point to the audio samples of the corresponding
                * channels (although it may be NULL for interleaved formats).
                */

                if(!(resampledData = //TODO: TO CHECK
                        (uint8_t**)calloc(AudioEncoderCodecCtx->channels,
                                          sizeof (*resampledData))) ){
                    cerr<<"Could not allocate converted input sample pointers\n";
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
                            (const uint8_t**)pAudioFrame->extended_data, pAudioFrame->nb_samples);


                //add_sample_to_fifo
                /* Make the FIFO as large as it needs to be to hold both,
                * the old and the new samples. */
                if((value = av_audio_fifo_realloc(audio_fifo, av_audio_fifo_size(audio_fifo)+pAudioFrame->nb_samples)) < 0){
                    cerr<<"Could not reallocate FIFO\n";
                    exit(value);
                }

                /* Store the new samples in the FIFO buffer. */

                if(av_audio_fifo_write(audio_fifo, (void **) resampledData, pAudioFrame->nb_samples) < pAudioFrame->nb_samples) {
                    cerr << "Could not write data to FIFO\n";
                    exit(AVERROR_EXIT);
                }

                //pAudioFrame ready
                outAudioPacket = av_packet_alloc();
                outAudioPacket->data = nullptr;
                outAudioPacket->size = 0;

                outAudioFrame = av_frame_alloc();
                if(!outAudioFrame) {
                    cerr << "Cannot allocate an AVFrame for encoded audio\n";
                    exit(1);
                }

                outAudioFrame->nb_samples = AudioEncoderCodecCtx->frame_size;
                outAudioFrame->channel_layout = AudioEncoderCodecCtx->channel_layout;
                outAudioFrame->format = AudioEncoderCodecCtx->sample_fmt;
                outAudioFrame->sample_rate = AudioEncoderCodecCtx->sample_rate;
                av_frame_get_buffer(outAudioFrame,0);


                while(av_audio_fifo_size(audio_fifo) >= AudioEncoderCodecCtx->frame_size){ //frame_size = number of samples per channel in an audio frame.
                    value = av_audio_fifo_read(audio_fifo, (void **) outAudioFrame->data, AudioEncoderCodecCtx->frame_size);
                    outAudioFrame->pts = j;
                    j+= outAudioFrame->nb_samples;
                    value = avcodec_send_frame(AudioEncoderCodecCtx, outAudioFrame);
                    if(value == AVERROR(EAGAIN) || value == AVERROR_EOF)
                        break;
                    if(value < 0){
                        cerr<<"Cannot encode current audio packet\n" << j << endl;
                        exit(1);
                    }


                    if(++i <= maxFramesToDecode){
                        while(value >= 0){
                            value = avcodec_receive_packet(AudioEncoderCodecCtx, outAudioPacket);
                            if(value == AVERROR(EAGAIN) || value == AVERROR_EOF)
                                break;
                            else if (value < 0){
                                cerr << "Error during encoding\n";
                                exit(1);
                            }

                            av_packet_rescale_ts(outAudioPacket, AudioEncoderCodecCtx->time_base, audio_stream->time_base);
                            outAudioPacket->stream_index = AudioStreamIndx;


                            if(av_write_frame(oFormatCtx, outAudioPacket) != 0){
                                cerr << "Error in writing audio frame\n";
                            }

                            av_packet_unref(outAudioPacket);
                        }
                        value = 0;
                    } else
                        break;
                }
                av_frame_free(&outAudioFrame);
                av_packet_unref(outAudioPacket);
            }
            if(i > maxFramesToDecode)
                break;
        }
    }
    finish();
    return 1;
}

int AudioRecorder::finish() {
    value = av_write_trailer(oFormatCtx);
    if (value < 0) {
        cerr << "Error in writing av trailer" << endl;
        exit(-1);
    }

    avformat_close_input(&AudioInFCtx);
    if(AudioInFCtx == nullptr){
        cout << "AudioInFCtx close successfully" << endl;
    }
    else{
        cerr << "Error: unable to close the AudioInFCtx" << endl;
        exit(-1);
    }
    avformat_free_context(AudioInFCtx);
    if(AudioInFCtx == nullptr){
        cout << "InAudioFormatCtx freed successfully" << endl;
    }
    else{
        cerr << "Error: unable to free AudioFormatContext" << endl;
        exit(-1);
    }

    avformat_free_context(oFormatCtx);
    return 0;
}

