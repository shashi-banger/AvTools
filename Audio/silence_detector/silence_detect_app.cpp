
#include <cstdio>
#include "silence_detector.h"

int main(int  argc, char  *argv[])
{
    SilenceDetector     *sd;

    if(argc < 2)
    {
        printf("Usage: ./silece_detect_app  <inp_wav_file>\n");
        exit(1);
    }
    sd = new SilenceDetector(argv[1]);
    sd->DetectAllSilenceInstances();
    return 0;
}
