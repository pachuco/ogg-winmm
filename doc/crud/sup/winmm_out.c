#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <windows.h>  // for mixer stream
#include <mmsystem.h> // for mixer stream

#include "winmm_out.h"

static char isAudioActive;
static HANDLE hThread, hAudioSem;
static LPCRITICAL_SECTION critDothings;
static HWAVEOUT hWaveOut;
static WAVEFORMATEX wfx;
static WAVEHDR* headers;
WinmmCallBack* callBack;
static WinmmFormat frmt;
static int bufCnt;

static DWORD WINAPI mixThread(LPVOID lpParam) {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    while(isAudioActive) {
        EnterCriticalSection(critDothings);
        callBack(headers[bufCnt].lpData, frmt.bufLength);
        LeaveCriticalSection(critDothings);
        
        waveOutPrepareHeader(hWaveOut, &headers[bufCnt], sizeof(WAVEHDR));
        waveOutWrite(hWaveOut, &headers[bufCnt], sizeof(WAVEHDR));
        bufCnt = (bufCnt + 1) % frmt.bufNr;
        WaitForSingleObject(hAudioSem, INFINITE);
    }
    return (0);
    (void)lpParam;
}

static void CALLBACK waveProc(HWAVEOUT hWaveOut, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
    if (uMsg == WOM_DONE) ReleaseSemaphore(hAudioSem, 1, NULL);
    (void)hWaveOut;
    (void)dwInstance;
    (void)dwParam1;
    (void)dwParam2;
}

void winmmout_closeMixer() {
    int i;
    
    isAudioActive = FALSE;
    if (hThread) {
        ReleaseSemaphore(hAudioSem, 1, NULL);
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
        hThread = NULL;
    }
    if (critDothings) {
        DeleteCriticalSection(critDothings);
        free(critDothings);
        critDothings = NULL;
    }
    if (hAudioSem) {
        CloseHandle(hAudioSem);
        hAudioSem = NULL;
    }
    if (hWaveOut) {
        waveOutReset(hWaveOut);
        for (i = 0; i < frmt.bufNr; ++i) {
            if (headers[i].dwUser != 0xFFFF) waveOutUnprepareHeader(hWaveOut, &headers[i], sizeof (WAVEHDR));
        }
        waveOutClose(hWaveOut);
        hWaveOut = NULL;
    }
    if (headers) {
        free(headers);
        headers = NULL;
    }
    callBack = NULL;
    bufCnt = 0;
}

BOOL winmmout_openMixer(WinmmFormat* wf, WinmmCallBack* cb) {
    int i;
    DWORD threadID;
    
    winmmout_closeMixer();
    
    callBack = cb;
	memcpy(&frmt, wf, sizeof(WinmmFormat));
    
    wfx.nSamplesPerSec = frmt.sampleRate;
    wfx.wBitsPerSample = frmt.bits;
    wfx.nChannels = frmt.chanNr;

    wfx.cbSize = 0;
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nBlockAlign = (wfx.wBitsPerSample * wfx.nChannels) / 8;
    wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;

    headers = calloc(frmt.bufLength, sizeof(WAVEHDR));
    if (!headers) goto _ERR;
    if(waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, (DWORD_PTR)(&waveProc), 0, CALLBACK_FUNCTION) != MMSYSERR_NOERROR) goto _ERR;
    
    for (i = 0; i < frmt.bufNr; i++) headers[i].dwUser = 0xFFFF;
    for (i = 0; i < frmt.bufNr; i++) {
        LPSTR mbuf = calloc(frmt.bufLength, wfx.nBlockAlign);
        if (mbuf == NULL) goto _ERR;
        
        headers[i].dwUser = 0;
        headers[i].dwBufferLength = wfx.nBlockAlign * frmt.bufLength;
        headers[i].lpData = mbuf;
        waveOutPrepareHeader(hWaveOut, &headers[i], sizeof(WAVEHDR));
        waveOutWrite(hWaveOut, &headers[i], sizeof(WAVEHDR));
    }
    
    isAudioActive = TRUE;
    
    hAudioSem = CreateSemaphore(NULL, frmt.bufNr - 1, frmt.bufNr, NULL);
    if (!hAudioSem) goto _ERR;
    critDothings = calloc(1, sizeof(CRITICAL_SECTION));
    if (!critDothings) goto _ERR;
    InitializeCriticalSection(critDothings);
    hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(mixThread), NULL, 0, &threadID);
    if (!hThread) goto _ERR;
    
    return TRUE;
    
    _ERR:
        winmmout_closeMixer();
        return FALSE;
}

void winmmout_enterCrit() { EnterCriticalSection(critDothings); }
void winmmout_leaveCrit() { LeaveCriticalSection(critDothings); }