# ScreenRecorderProject


## Group:
* Alberto Solavagione
* Davide Scovotto



## Description:
**ScreenRecorderProject** is a library based on the famous third party library **FFmpeg** which offers 
the possibility to record the desktop and optionally the audio from the system microphone.
It is possible to interact with this library through a CLI integrated in this project.

### CLI
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

### Library

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

## How To Build/Install:

### Linux
- Download the required libraries and tools `sudo apt install qt5-qmake build-essential libasound2-dev libavcodec-dev libavdevice-dev libavfilter-dev libavformat-dev libavutil-dev`
- Install **c++ 20** `sudo apt install gcc-10 gcc-10-base gcc-10-doc g++-10` `sudo apt install libstdc++-10-dev libstdc++-10-doc`
- Clone the repository `git clone https://github.com/AlbertoSvg/ScreenRecorderFinal.git`
- Go in the project folder  `cd ScreenRecorderFinal`
- Generate the **Makefile** `qmake ScreenRecorder.pro`
- Compile it `make clean; make all`
- Allow the **binary** to be executed  `chmod 777 ScreenRecorder`
- Run it `./ScreenRecorder`
### Windows
- Since in Windows the whole process of installation of the required libraries and
compilation of the software is much more complicated than Linux, we suggest to simply
download the precompiled software from this [link](https://github.com/AlbertoSvg/ScreenRecorderFinal/releases/download/Latest/ScreenRecorderFinal_Win32.exe).
- If you really want to compile it from scratch, here is the [link](https://trac.ffmpeg.org/wiki/CompilationGuide/MinGW) that guides you for 
downloading and compiling the FFmpeg library.
## Run:
- Download the software from this [link](https://github.com/AlbertoSvg/ScreenRecorderFinal/releases/download/Latest/ScreenRecorderFinal_Win32.exe) **(Windows)** or this [link](https://github.com/AlbertoSvg/ScreenRecorderFinal/releases/download/Latest/ScreenRecorderFinal_Linux) **(Linux)**
- Run the software
- The first thing that the software ask you is to specify the audio device.<br/>
In **Linux** to find out what audio device you have to insert you can initially install the alsa-utils package with `sudo apt install alsa-utils` and then use the command
`arecord -l` to list all the capture devices and choose the appropriate one.<br/>
For example in the case reported in the picture the correct device to insert is `hw:0,0`.<br/>
![alt text](https://i.imgur.com/KSkwJPG.jpg) <br/>
In **Windows** to find out what audio device you have to insert, you need to type
_Device Manager_ in the search box on the Windows' taskbar, then select from the menu the **Device Manager**.
Then navigate to **Audio inputs and outputs** and copy the name of the appropriate audio
input device (like in the picture below).
![alt text](https://i.imgur.com/b7dvvIH.jpg)
- The second thing to insert is the path and name of the output file (recorded video).
For example `./output.mp4`.<br/>
_IMPORTANT:_ specify the file format`(.mp4)`.
- The third thing is to specify if you want the audio recording `(Y,N)`.
- The final thing is the definition of the screen area to record. This is possible by 
inserting _4 offsets (X1, Y1, X2, Y2)_. The offsets must be multiple of 2 and lower than the width and height of the screen.
  1. X1 is the horizontal offset starting from the left
  2. X2 is the horizontal offset starting from the right
  3. Y1 is the vertical offset starting from the top
  4. Y2 is the vertical offset starting from the bottom<br/>
For a better understanding look at picture below.
![alt text](https://i.imgur.com/koGgagu.jpg)
- After all these initial configurations you can start the recording by pressing `1`.<br/>
