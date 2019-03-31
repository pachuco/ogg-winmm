#include <ivorbisfile.h>
#include <stdio.h>
#include <windows.h>
#include <stdint.h>

WAVEFORMATEX    plr_fmt;
HWAVEOUT        plr_hwo         = NULL;
OggVorbis_File  plr_vf;
HANDLE          plr_ev          = NULL;
int             plr_cnt         = 0;
int             plr_lvol        = 0xFFFF;
int             plr_rvol        = 0xFFFF;
WAVEHDR         *plr_buffers[3] = { NULL, NULL, NULL };

#define OPENFILEWITHFAVPARAMS(x) CreateFileA(x, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL)
//-----------------------------vorbis crud
static size_t ovCB_read(void *ptr, size_t size, size_t nmemb, void *datasource) {
    HANDLE hFile = (HANDLE)datasource;
    DWORD len;
    
    ReadFile(hFile, ptr, size, &len, NULL);
    return len;
}
static int ovCB_seek(void *datasource, ogg_int64_t offset, int whence) {
    HANDLE hFile = (HANDLE)datasource;
    LONG olow  = offset &  0xFFFFFFFF;
    LONG ohigh = offset >> 32;
    DWORD ret;
    
    switch (whence) {
        case SEEK_CUR:
            ret = SetFilePointer(hFile, olow, &ohigh, FILE_CURRENT);
            break;
        case SEEK_END:
            ret = SetFilePointer(hFile, olow, &ohigh, FILE_END);
            break;
        case SEEK_SET:
            ret = SetFilePointer(hFile, olow, &ohigh, FILE_BEGIN);
            break;
        default:
            return -1;
    }
    return (ret == INVALID_SET_FILE_POINTER || ret == ERROR_NEGATIVE_SEEK) ? -1 : 0;
}
static int ovCB_close(void *datasource) {
    HANDLE hFile = (HANDLE)datasource;
    CloseHandle(hFile);
    return 0;
}
static long ovCB_tell(void *datasource) {
    HANDLE hFile = (HANDLE)datasource;
    return SetFilePointer(hFile, 0, NULL, FILE_CURRENT);
}
static ov_callbacks ovCB = {&ovCB_read, &ovCB_seek, &ovCB_close, &ovCB_tell};
//-----------------------------

void plr_stop() {
    plr_cnt = 0;

    if (plr_vf.datasource) ov_clear(&plr_vf);

    if (plr_ev) {
        CloseHandle(plr_ev);
        plr_ev = NULL;
    }

    if (plr_hwo) {
        waveOutReset(plr_hwo);

        int i;
        for (i = 0; i < 3; i++) {
            if (plr_buffers[i] && plr_buffers[i]->dwFlags & WHDR_DONE) {
                waveOutUnprepareHeader(plr_hwo, plr_buffers[i], sizeof(WAVEHDR));
                free(plr_buffers[i]->lpData);
                free(plr_buffers[i]);
                plr_buffers[i] = NULL;
            }
        }

        waveOutClose(plr_hwo);
        plr_hwo = NULL;
    }
}

void plr_volume(uint16_t lVol, uint16_t rVol) {
    plr_lvol = lVol;
    plr_rvol = rVol;
}

int plr_length(const char *path) {
    HANDLE hFile;
    OggVorbis_File  vf;
    
    hFile = OPENFILEWITHFAVPARAMS(path);
    if (hFile == INVALID_HANDLE_VALUE) return 0;
    if (ov_open_callbacks(hFile, &vf, NULL, 0, ovCB)) return 0;

    int ret = (int)ov_time_total(&vf, -1);

    ov_clear(&vf);

    return ret;
}

int plr_play(const char *path) {
    HANDLE hFile;
    plr_stop();
    
    hFile = OPENFILEWITHFAVPARAMS(path);
    if (hFile == INVALID_HANDLE_VALUE) return 0;
    if (ov_open_callbacks(hFile, &plr_vf, NULL, 0, ovCB)) return 0;

    vorbis_info *vi = ov_info(&plr_vf, -1);

    if (!vi) {
        ov_clear(&plr_vf);
        return 0;
    }

    plr_fmt.wFormatTag      = WAVE_FORMAT_PCM;
    plr_fmt.nChannels       = vi->channels;
    plr_fmt.nSamplesPerSec  = vi->rate;
    plr_fmt.wBitsPerSample  = 16;
    plr_fmt.nBlockAlign     = plr_fmt.nChannels * (plr_fmt.wBitsPerSample / 8);
    plr_fmt.nAvgBytesPerSec = plr_fmt.nBlockAlign * plr_fmt.nSamplesPerSec;
    plr_fmt.cbSize          = 0;

    plr_ev = CreateEvent(NULL, 0, 1, NULL);

    if (waveOutOpen(&plr_hwo, WAVE_MAPPER, &plr_fmt, (DWORD_PTR)plr_ev, 0, CALLBACK_EVENT) != MMSYSERR_NOERROR) {
        return 0;
    }

    return 1;
}

int plr_pump() {
    if (!plr_vf.datasource) return 0;

    int pos = 0;
    int bufsize = plr_fmt.nAvgBytesPerSec / 4; // 250ms (avg at 500ms) should be enough for everyone
    char *buf = malloc(bufsize);

    while (pos < bufsize) {
        long bytes = ov_read(&plr_vf, buf + pos, bufsize - pos, NULL);

        if (bytes == OV_HOLE) {
            free(buf);
            continue;
        }

        if (bytes == OV_EBADLINK) {
            free(buf);
            return 0;
        }

        if (bytes == OV_EINVAL) {
            free(buf);
            return 0;
        }

        if (bytes == 0) {
            free(buf);

            int i, in_queue = 0;
            for (i = 0; i < 3; i++) {
                if (plr_buffers[i] && plr_buffers[i]->dwFlags & WHDR_DONE) {
                    waveOutUnprepareHeader(plr_hwo, plr_buffers[i], sizeof(WAVEHDR));
                    free(plr_buffers[i]->lpData);
                    free(plr_buffers[i]);
                    plr_buffers[i] = NULL;
                }

                if (plr_buffers[i]) in_queue++;
            }

            Sleep(100);

            return !(in_queue == 0);
        }

        pos += bytes;
    }

    int x, end = pos / (sizeof(short)*2);
    short *sbuf = (short *)buf;
    for (x = 0; x < end; x++) {
        sbuf[x+0] = ((int32_t)sbuf[x+0] * plr_lvol)>>16;
        sbuf[x+1] = ((int32_t)sbuf[x+1] * plr_rvol)>>16;
    }

    WAVEHDR *header = malloc(sizeof(WAVEHDR));
    header->dwBufferLength   = pos;
    header->lpData           = buf;
    header->dwUser           = 0;
    header->dwFlags          = plr_cnt == 0 ? WHDR_BEGINLOOP : 0;
    header->dwLoops          = 0;
    header->lpNext           = NULL;
    header->reserved         = 0;

    waveOutPrepareHeader(plr_hwo, header, sizeof(WAVEHDR));

    if (plr_cnt > 1) {
        WaitForSingleObject(plr_ev, INFINITE);
    }

    int i, queued = 0;
    for (i = 0; i < 3; i++) {
        if (plr_buffers[i] && plr_buffers[i]->dwFlags & WHDR_DONE) {
            waveOutUnprepareHeader(plr_hwo, plr_buffers[i], sizeof(WAVEHDR));
            free(plr_buffers[i]->lpData);
            free(plr_buffers[i]);
            plr_buffers[i] = NULL;
        }

        if (!queued && plr_buffers[i] == NULL) {
            waveOutWrite(plr_hwo, header, sizeof(WAVEHDR));
            plr_buffers[i] = header;
            queued = 1;
        }
    }

    if (!queued) {
        free(header);
        free(buf);
    }

    plr_cnt++;

    return 1;
}
