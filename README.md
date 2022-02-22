# ScreenRecorderProject

***
###Group:
* Alberto Solavagione
* Davide Scovotto

***

###Description:
**ScreenRecorderProject** is a library based on the famous third party library **FFmpeg** which offers 
the possibility to record the desktop and optionally the audio from the system microphone.
It is possible to interact with this library through a CLI integrated in this project.

####CLI
Using the **command line** a user can:
* Specify an audio device (microphone)
* Specify the location where to save the video recorded (with .mp4 format)
* Specify if the audio recording is requested
* Specify the portion of the screen that you want to record

Once the user has set all the initial options then can:
* Start the recording
* Pause/Resume the recording 
* Stop the recording
* Exit

####Library

The **ScreenRecorderProject library** is composed by two files **Recorder.cpp** and **Recorder.h**.
The major APIs offered by this library are:
* **OpenVideoDevice()/OpenAudioDevice()**: these APIs open the device for recording the desktop and for
capturing the audio from the microphone. In Windows is used _gdigrab_ for the screen and _dshow_ for the audio.
In Linux instead is used _x11grab_ for the screen and _alsa_ for the audio.
  <br><br/>
* **InitOutputFile()**: allocate and prepares the context for the output file based from the file extension
  <br><br/>
* **InitializeVideoDecoder()/InitializeAudioDecoder()**: prepare and allocate the resources for the video/audio decoder contexts and opens them
  <br><br/>
* **SetUp_VideoEncoder()/SetUp_AudioEncoder()**: prepare and allocate the resources for the video/audio encoder contexts and opens them
  <br><br/>
* **lastSetUp()**: write the stream header to the output media file
  <br><br/>

* **startCapture()**: this is the core API, it calls the above APIs for open the video/audio device and initialize the
video/audio decoder and encoder and sets up the output file of the resulting video.<br />
After the initialization/configuration process it starts **4 threads**:
  1. _manageCapture_ is a lambda function that internally calls **captureMenu()** 
  2. _acquireFrames_ is a lambda function that calls **acquireVideoFrames()**
  3. _encodeDecodeVideo_ is a lambda function that calls **encodeDecodeVideoStream()**
  4. _captureAudio_ is a lambda function that calls **AudioDecEnc()**
  
  <br/> 

* **captureMenu()**: is an API that permits to start/stop and pause/resume the recording by setting
shared global variables with the other threads in order to synchronize the above actions.
<br><br/>
* **acquireVideoFrames()**: this API acquires from the video device raw packets and inserts them in
a queue
<br><br/>
* **encodeDecodeVideoStream()**: this API performs the decoding and the encoding. It takes a raw packet from the
queue and passes it to the decoder which outputs the decoded frame then the frame is scaled and re-encoded in a mpeg4 container and wrote 
on the output file
<br><br/>
* **AudioDecEnc()**: this API performs similar actions to the ones of the video but for the audio

###How To Build/Install:

####Linux

####Windows

###Run:


