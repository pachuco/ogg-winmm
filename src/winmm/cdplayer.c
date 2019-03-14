#include <vorbis/vorbisfile.h>
#include <windows.h>
#include <stdint.h>

#include "sup/winmm_out.h"
#include "sup/winfile.h"
#include "sup/util.h"
#include "cdplayer.h"

#define OPENFILEWITHFAVPARAMS(x) fprx_CreateFileA(x, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL)

#define MAX_TRACKS 99
#define FRAMES_PER_SECOND 75
#define SAMPLES_PER_FRAME 588

#define CDMUSICBASE "\\music"
#define MAXINIBUF 3072

static WinmmFormat wf = {44100, 2, 16, SAMPLES_PER_FRAME, 8};//render one CD frame at a time
static OggVorbis_File vf;

typedef enum CDP_FLAGS {
    CDPF_ISAUDIO    = 1<<0
} CDP_FLAGS;
typedef struct Track {
    uint32_t frameOff;      //offset on disk
    uint32_t frameLen;      //length of track in whole frames
    uint8_t  flags;         //track property bitfield
} Track;

Track cdtracks[MAX_TRACKS];
static uint8_t maxTrackNr;

static BOOL    isPlaying;
static uint8_t trkCurrent;
static uint8_t gapFrameCount;

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

static void audioCB(LPSTR buf, int sampNr) {
    
}

void cdplayer_play(DWORD_PTR cmd, DWORD format, DWORD from, DWORD to) {
    
    if        (format == MCI_FORMAT_BYTES) {
    } else if (format == MCI_FORMAT_FRAMES) {
    } else if (format == MCI_FORMAT_HMS) {
    } else if (format == MCI_FORMAT_MILLISECONDS) {
    } else if (format == MCI_FORMAT_MSF) {
    } else if (format == MCI_FORMAT_SAMPLES) {
    } else if (format == MCI_FORMAT_TMSF) {
    }
    
    if (cmd & MCI_FROM) {
    } else {
    }
    
    if (cmd & MCI_TO) {
    } else {
    }
}

void cdplayer_stop() {
    isPlaying = TRUE;
}



CDP_ERR cdplayer_init() {
    #define FAIL(x) {err = x; goto _ERR;}
    WIN32_FIND_DATAA ffd;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    CDP_ERR err;
    
    isPlaying = FALSE;
    //if (!winmmout_openMixer(&wf, &audioCB)) FAIL(CDPE_DEVICE); //this locks up
    
    for (int i=0; i<MAX_TRACKS; i++) {
        cdtracks[i].frameOff = 0;
        cdtracks[i].frameLen = 0;
        cdtracks[i].flags  = 0;
    }
    maxTrackNr = 1;
    gapFrameCount = 2 * MAX_TRACKS;
    trkCurrent = 0;
    
    //check if folder is there
    hFind = fprx_FindFirstFileA(CDMUSICBASE, &ffd);
    if (hFind != INVALID_HANDLE_VALUE) {
        FindClose(hFind);
    } else FAIL(CDPE_NOMEDIA);
    
    //load ini with songlength infos, if available
    static char pathInfoIni[] = CDMUSICBASE "\\cdinfo.ini";
    hFind = fprx_FindFirstFileA(pathInfoIni, &ffd);
    if (hFind != INVALID_HANDLE_VALUE) {
        char  iniBuf[MAXINIBUF];
        char* rp = iniBuf;
        
        fprx_GetPrivateProfileSectionA("cdinfo", iniBuf, MAXINIBUF, pathInfoIni);
        while (rp[0]!='\0') {
            char* rp2 = rp;
            int   len = lstrlenA(rp2);
            int trk, mm, ss, ff;
            
            //trackXX=MM:SS:FF
            if (!(rp2 = strtokWalkA(rp2, "track"))) continue;
            if (!(rp2 = ub10FromStr(&trk, rp2, 2))) continue;
            if (!(rp2 = strtokWalkA(rp2, "="))) continue;
            if (!(rp2 = ub10FromStr(&mm,  rp2, 2))) continue;
            if (!(rp2 = strtokWalkA(rp2, ":"))) continue;
            if (!(rp2 = ub10FromStr(&ss,  rp2, 2))) continue;
            if (!(rp2 = strtokWalkA(rp2, ":"))) continue;
            if (!(rp2 = ub10FromStr(&ff,  rp2, 2))) continue;
            rp += len;
            rp++; //end of string series is double-NUL terminated
            
            cdtracks[trk].frameLen = mm*60*FRAMES_PER_SECOND + ss*FRAMES_PER_SECOND + ff;
        }
        
        FindClose(hFind);
    }
    
    //get audio files
    static char pathTrackModel[] = CDMUSICBASE "\\track??.ogg";
    hFind = fprx_FindFirstFileA(pathTrackModel, &ffd);
    if (hFind != INVALID_HANDLE_VALUE) { //if folder is not there, we have a CD with no tracks.
        do {
            HANDLE hFile;
            uint32_t tnum;
            
            if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
            if (!ub10FromStr(&tnum, ffd.cFileName + 5, 2)) continue;
            if (tnum == 0) continue; //track 00 is not valid
            
            ub10ToStr(pathTrackModel+12, tnum, 2, TRUE);
            hFile = OPENFILEWITHFAVPARAMS(pathTrackModel);
            if (hFile == INVALID_HANDLE_VALUE) continue;
            if (ov_open_callbacks(hFile, &vf, NULL, 0, ovCB)) continue;
            
            vorbis_info *vi = ov_info(&vf, -1);
            //lengths from ini file override ogg lengths
            if (vi && vi->channels == 2 && vi->rate == 44100) {
                ogg_int64_t lenRaw = ov_pcm_total(&vf, -1);
                if (lenRaw != OV_EINVAL && lenRaw <= UINT32_MAX) {
                    DWORD smpLen = lenRaw;
                    if (cdtracks[tnum].frameLen == 0) { //ini override
                        //CD audio is always aligned to frames
                        cdtracks[tnum].frameLen  = smpLen / SAMPLES_PER_FRAME + (smpLen%SAMPLES_PER_FRAME > 0 ? 1 : 0);
                        cdtracks[tnum].flags    |= CDPF_ISAUDIO;
                        if (tnum > maxTrackNr) maxTrackNr = tnum;
                    }
                    printf("%d\n", cdtracks[tnum].frameLen);
                } else {
                    cdtracks[tnum].flags &= ~CDPF_ISAUDIO; //clear flag
                }
            } else {
                cdtracks[tnum].flags &= ~CDPF_ISAUDIO; //clear flag
            }
            ov_clear(&vf);
            
        } while (fprx_FindNextFileA(hFind, &ffd) != 0);
        //if (GetLastError() != ERROR_NO_MORE_FILES)
        FindClose(hFind);
    }
    
    //process offsets
    for (int i=0; i <= maxTrackNr; i++) {
        Track* t = &cdtracks[i];
    }
    
    return CDPE_OK;
    
    _ERR: {
        char* em;
        if (err == CDPE_NOMEDIA) em = "Cannot open music folder.\n";
        if (err == CDPE_DEVICE)  em = "Cannot open audio mixer.\n";
        winmmout_closeMixer();
        OutputDebugStringA(em);
        return err;
    }
    #undef FAIL
}

void cdplayer_volume(uint16_t lVol, uint16_t rVol) {
}