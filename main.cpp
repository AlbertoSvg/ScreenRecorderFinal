#include <iostream>
#include "Recorder.h"



int main() {
    /* Create recorder object */
    Recorder recorder;
    /* Initial menu to get user information */
    recorder.menu();
    /* Launch multi-threaded capture process */
    recorder.startCapture();

    return 0;
}
