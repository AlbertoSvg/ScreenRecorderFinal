#include <iostream>
#include "AudioRecorder.h"

int main() {
    AudioRecorder recorder;

    recorder.OpenAudioDevice();

    recorder.InitializeAudioDecoder();

    recorder.InitOutputFile();

    recorder.SetUp_AudioEncoder();

    recorder.DecEnc();
    return 0;
}

