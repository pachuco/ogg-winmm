#ifndef WINMM_OUT_H
#define WINMM_OUT_H

#include <windows.h>

typedef void (WinmmCallBack)(LPSTR buf, int sampNr);

typedef struct WinmmFormat WinmmFormat;
struct WinmmFormat {
    int sampleRate;
    int chanNr;
    int bits;
    int bufLength;
    int bufNr;
};

BOOL winmmout_openMixer(WinmmFormat* wf, WinmmCallBack* cb);
void winmmout_closeMixer();
void winmmout_enterCrit();
void winmmout_leaveCrit();

#endif
