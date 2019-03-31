#include <vorbis/vorbisfile.h>
#include <windows.h>
#include <stdint.h>
#include <assert.h>

#include "sup/winmm_out.h"
#include "sup/winfile.h"
#include "sup/util.h"
#include "cdplayer.h"

#define OPENFILEWITHFAVPARAMS(x) fprx_CreateFileA(x, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL)

#define VALID_MILI(x) ((x) >= 2000 && (x) < 568320000)
#define VALID_MSF(x)  (MCI_MSF_MINUTE(x) < 60 && MCI_MSF_SECOND(x) < 60 && MCI_MSF_FRAME(x) < 75)
#define VALID_TMSF(x) (MCI_TMSF_TRACK(x) <= p_maxTrackNr && MCI_TMSF_MINUTE(x) < 60 && MCI_TMSF_SECOND(x) < 60 && MCI_TMSF_FRAME(x) < 75)
#define MILI2F(x) ((x*1000)/FRAMES_PER_SECOND)
#define MSF2F(x)  ((60*MCI_MSF_MINUTE(x))/FRAMES_PER_SECOND + MCI_MSF_SECOND(x)/FRAMES_PER_SECOND + MCI_MSF_FRAME(x))
#define TMSF2F(x) (cdtracks[MCI_TMSF_TRACK(x)].frameOff + (60*MCI_TMSF_MINUTE(x))/FRAMES_PER_SECOND + MCI_TMSF_SECOND(x)/FRAMES_PER_SECOND + MCI_TMSF_FRAME(x))

#define MAX_TRACKS 99
#define FRAMES_PER_SECOND 75
#define SAMPLES_PER_FRAME 588

#define CDMUSICBASE "\\music"
#define PATHTRACKMODEL CDMUSICBASE "\\track??.ogg";
#define MAXINIBUF 3072

static WinmmFormat wf = {44100, 2, 16, SAMPLES_PER_FRAME, 8};//render one CD frame at a time
static OggVorbis_File vf;

typedef enum CDP_TYPES {
    CDPF_DATA       = 1<<0,
    CDPF_AUDIO_OGG  = 1<<1,
    CDPF_AUDIO_FLAC = 1<<2
} CDP_TYPES;
typedef struct Track {
    uint32_t frameOff;      //offset on disk
    uint32_t frameLen;      //length of track in whole frames
    uint8_t  type;          //track property bitfield
} Track;
Track cdtracks[MAX_TRACKS];
static uint8_t p_maxTrackNr;

//player states
static BOOL     p_isWinmmInit, p_isPlaying, p_isSeekWanted;
static uint8_t  p_curTrack;
static uint32_t p_curFrame;
static uint32_t p_fromFrame, p_toFrame;
static uint16_t p_volL, p_volR;
//the various states of chaos we have to control
static DWORD    p_timeFormat;
static BOOL     p_isCDinDrive, p_isCDTrayClosed;
static BOOL     p_isDevOpen, p_isDevPlayPaused, p_hasDevPlayBeenPausedBefore;

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

static void audioCB(LPSTR buf, int sampWanted) {
    HANDLE hFile;
    int numsamp = 0;
    int16_t* buf16 = (int16_t*)buf;
    
    assert(sampWanted == SAMPLES_PER_FRAME);
    if (p_curFrame >= p_toFrame) p_isPlaying = FALSE;
    
    if (p_isSeekWanted) {
        for (int i=1; i <= p_maxTrackNr; i++) {
            Track* trk = &cdtracks[i];
            
            if (p_fromFrame >= trk->frameOff && p_fromFrame < trk->frameOff+trk->frameLen) {
                if (trk->type != CDPF_DATA) {
                    if (p_curFrame < trk->frameOff || p_curFrame >= trk->frameOff+trk->frameLen) {
                        char pathTrackModel[] = PATHTRACKMODEL;
                        //load and seek
                        ub10ToStr(pathTrackModel+12, i, 2, TRUE);
                        hFile = OPENFILEWITHFAVPARAMS(pathTrackModel);
                        if (hFile == INVALID_HANDLE_VALUE) continue;
                        ov_open_callbacks(hFile, &vf, NULL, 0, ovCB);
                    }
                } else {
                }
                ov_pcm_seek(&vf, (trk->frameOff - p_fromFrame) * SAMPLES_PER_FRAME);
                break;
            }
        }
        p_isSeekWanted = FALSE;
    }
    
    if (p_isCDinDrive && p_isCDTrayClosed && p_isPlaying) {
        while (numsamp < SAMPLES_PER_FRAME) {
            long bytes = ov_read(&vf, buf, SAMPLES_PER_FRAME*4, 0, 2, 1, NULL);
            int len = len / 4;
            if (bytes == OV_HOLE) continue;
            
            if (bytes == OV_EBADLINK || bytes == OV_EINVAL || bytes == 0) {
                p_isSeekWanted = TRUE;
                break;
            }
            for (int i=numsamp; i<numsamp+len; numsamp++) { //apply volume
                int ind = numsamp*2;
                
                buf16[ind+0] = ((int32_t)buf16[ind+0] + p_volL)>>16;
                buf16[ind+1] = ((int32_t)buf16[ind+1] + p_volR)>>16;
            }
        }
    }
    for (; numsamp < SAMPLES_PER_FRAME; numsamp++) { //pad up to full frame
        buf16[numsamp*2 + 0] = 0;
        buf16[numsamp*2 + 1] = 0;
    }
    
    p_curFrame++;
}

static BOOL checkInitWinmm() {
    if (!p_isWinmmInit) {
        if (!winmmout_openMixer(&wf, &audioCB)) return FALSE;
        p_isWinmmInit = TRUE;
    }
    return TRUE;
}

MCIERROR cdplayer_play(DWORD_PTR cmd, DWORD from, DWORD to) {
    uint32_t fromFr;
    uint32_t toFr;
    
    //some weird results for millisecond FROM times
    //0000000000 to 0000001999 //parameter out of range 
    //0000002000 to 0568319999 //normal seek 
    //0568320000 to 0568321999 //parameter out of range
    //0568322000 to 4294967295 //overflowed seek
    //4294967296 to 4294967299 //parameter out of range
    //4294967300 to xxxxxxxxxx //invalid integer
    //negative numbers plays from last song(because unsigned cast)
    
    if (!p_isCDinDrive || !p_isCDTrayClosed) return MCIERR_HARDWARE;
    if (!checkInitWinmm()) return MCIERR_HARDWARE;
    //playback can be started without opening the device prior
    if (!p_isDevOpen) p_timeFormat = MCI_FORMAT_MSF;
    
    if (cmd & MCI_FROM) {
        if        (p_timeFormat == MCI_FORMAT_MILLISECONDS) {
            if (!VALID_MILI(from)) return MCIERR_OUTOFRANGE;
            fromFr = MILI2F(from);
        } else if (p_timeFormat == MCI_FORMAT_MSF) {
            if (!VALID_MSF(from))  return MCIERR_OUTOFRANGE;
            fromFr = MSF2F(from);
        } else if (p_timeFormat == MCI_FORMAT_TMSF) {
            Track* trk;
            
            if (!VALID_TMSF(from)) return MCIERR_OUTOFRANGE;
            fromFr = TMSF2F(from);
            trk = &cdtracks[MCI_TMSF_TRACK(from)];
            //if past CD length, start from last track
            if (toFr > (trk->frameOff + trk->frameLen)) fromFr = trk->frameOff;
        }
        p_fromFrame = fromFr;
        p_isSeekWanted = TRUE;
    } else {
        if (p_isDevOpen) {
            p_fromFrame = p_curFrame;
        } else {
            p_fromFrame = 2 * FRAMES_PER_SECOND;
            p_isSeekWanted = TRUE;
        }
    }
    
    if (cmd & MCI_TO) {
        if        (p_timeFormat == MCI_FORMAT_MILLISECONDS) {
            if (!VALID_MILI(to)) return MCIERR_OUTOFRANGE;
            toFr = MILI2F(to);
        } else if (p_timeFormat == MCI_FORMAT_MSF) {
            if (!VALID_MSF(to))  return MCIERR_OUTOFRANGE;
            toFr = MSF2F(to);
        } else if (p_timeFormat == MCI_FORMAT_TMSF) {
            Track* trk;
            
            if (!VALID_TMSF(to)) return MCIERR_OUTOFRANGE;
            toFr = TMSF2F(to);
            //trk = &cdtracks[MCI_TMSF_TRACK(to)];
            //if (toFr > t
        }
        p_toFrame = toFr;
    } else {
        p_toFrame = -1;
    }
    return CDPE_OK;
}

MCIERROR cdplayer_set(DWORD_PTR cmd, DWORD data) {
    //Command carries out regardless
    if        (cmd == MCI_SET_TIME_FORMAT) {
        if (p_isDevOpen || (!p_hasDevPlayBeenPausedBefore && p_isPlaying)) {
            p_timeFormat = data;
        }
    } else if (cmd == MCI_SET_AUDIO) {
    }
    
    return 0;
}

MCIERROR cdplayer_status(DWORD_PTR cmd) {
    //Command carries out regardless
    if        (cmd == MCI_STATUS_POSITION) {
    } else if (cmd == MCI_STATUS_LENGTH) {
    } else if (cmd == MCI_STATUS_NUMBER_OF_TRACKS) {
    } else if (cmd == MCI_STATUS_READY) {
    } else if (cmd == MCI_STATUS_MODE) {
    } else if (cmd == MCI_STATUS_MEDIA_PRESENT) {
    } else if (cmd == MCI_STATUS_TIME_FORMAT) {
    } else if (cmd == MCI_STATUS_CURRENT_TRACK) {
    }
    
    return 0;
}

MCIERROR cdplayer_stop() {
    if (p_isCDinDrive || !p_isCDTrayClosed) return MCIERR_HARDWARE;
    if (p_isPlaying) {
        p_hasDevPlayBeenPausedBefore = p_isDevPlayPaused = FALSE;
        p_isPlaying = FALSE;
        //winmmout_closeMixer();
    }
    return 0;
}
MCIERROR cdplayer_pause() {
    if (p_isCDinDrive || !p_isCDTrayClosed) return MCIERR_HARDWARE;
    if (p_isPlaying) {
        p_hasDevPlayBeenPausedBefore = p_isDevPlayPaused = TRUE;
        if (!p_isDevOpen) p_timeFormat = MCI_FORMAT_MSF;
        p_isPlaying = FALSE;
        //winmmout_closeMixer();
    }
    return 0;
}
MCIERROR cdplayer_resume() {
    if (p_isCDinDrive || !p_isCDTrayClosed) return MCIERR_HARDWARE;
    if (!p_isDevPlayPaused) return MCIERR_HARDWARE;
    if (!checkInitWinmm()) return MCIERR_HARDWARE;
    p_isDevPlayPaused = FALSE;
    p_isPlaying = TRUE;
    
    return 0;
}

MCIERROR cdplayer_volume(uint16_t lVol, uint16_t rVol) {
    p_volL = lVol;
    p_volR = rVol;
    return 0;
}

void cdplayer_init() {
    WIN32_FIND_DATAA ffd;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    
    for (int i=0; i<MAX_TRACKS; i++) {
        cdtracks[i].frameOff = 0;
        cdtracks[i].frameLen = 0;
        cdtracks[i].type     = CDPF_DATA;
    }
    p_isDevOpen       = FALSE;
    p_isDevPlayPaused = FALSE;
    p_isCDTrayClosed  = TRUE;
    p_isCDinDrive     = TRUE;
    
    p_isWinmmInit  = FALSE;
    p_isPlaying    = FALSE;
    p_isSeekWanted = TRUE;
    p_fromFrame  = 2 * FRAMES_PER_SECOND;
    p_toFrame    = -1;
    p_maxTrackNr = 0;
    p_volL = p_volR = 0xFFFF;
    
    //check if folder is there, if not, CD is not inserted
    hFind = fprx_FindFirstFileA(CDMUSICBASE, &ffd);
    if (hFind != INVALID_HANDLE_VALUE) {
        FindClose(hFind);
    } else {
        p_isCDinDrive = FALSE;
        return;
    }
    
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
            int tnum, mm, ss, ff;
            
            rp += len;
            rp++; //end of string series is double-NUL terminated
            
            //trackXX=MM:SS:FF
            if (!tokWalkA(&rp2, "track")) continue;
            if (!tokReadUIntA(&tnum, &rp2, 2)) continue;
            if (tnum == 0) continue; // skip 00
            if (!tokWalkA(&rp2, "=")) continue;
            if (!tokReadUIntA(&mm,  &rp2, 2)) continue;
            if (!tokWalkA(&rp2, ":")) continue;
            if (!tokReadUIntA(&ss,  &rp2, 2)) continue;
            if (!tokWalkA(&rp2, ":")) continue;
            if (!tokReadUIntA(&ff,  &rp2, 2)) continue;
            if (*rp == '\0') continue;
            
            cdtracks[tnum].frameLen = mm*60*FRAMES_PER_SECOND + ss*FRAMES_PER_SECOND + ff;
        }
        
        FindClose(hFind);
    }
    
    //get audio files
    char pathTrackModel[] = PATHTRACKMODEL;
    hFind = fprx_FindFirstFileA(pathTrackModel, &ffd);
    if (hFind != INVALID_HANDLE_VALUE) { //if folder is there and lacks oggs, we have a CD with no tracks.
        do {
            HANDLE hFile;
            int tnum;
            char* rp = ffd.cFileName + 5;
            int ovRet;
            
            if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
            if (!tokReadUIntA(&tnum, &rp, 2)) continue;
            if (tnum == 0) continue; //track 00 is not valid
            
            ub10ToStr(pathTrackModel+12, tnum, 2, TRUE);
            hFile = OPENFILEWITHFAVPARAMS(pathTrackModel);
            if (hFile == INVALID_HANDLE_VALUE) continue;
            
            //ov_test_callbacks is significantly faster, but doesn't allow telling length
            ovRet = (cdtracks[tnum].frameLen == 0) ?
                ov_open_callbacks(hFile, &vf, NULL, 0, ovCB):
                ov_test_callbacks(hFile, &vf, NULL, 0, ovCB);
            if (ovRet) continue;
            
            vorbis_info *vi = ov_info(&vf, -1);
            //allow only CDAudio-spec ogg files
            if (vi && vi->channels == 2 && vi->rate == 44100) {
                
                //lengths from ini file override ogg lengths
                if (cdtracks[tnum].frameLen == 0) {
                    ogg_int64_t lenRaw = ov_pcm_total(&vf, -1);
                    
                    if (lenRaw != OV_EINVAL && lenRaw <= UINT32_MAX) {
                        DWORD smpLen = lenRaw;
                        
                        //CD audio is always aligned to frames
                        cdtracks[tnum].frameLen  = smpLen / SAMPLES_PER_FRAME + (smpLen%SAMPLES_PER_FRAME > 0 ? 1 : 0);
                        if (tnum > p_maxTrackNr) p_maxTrackNr = tnum;
                    }
                }
                cdtracks[tnum].type = CDPF_AUDIO_OGG;
            } else {
                DERROR("Track %02d is invalid.\n", tnum);
                cdtracks[tnum].type = CDPF_DATA;
            }
            ov_clear(&vf);
            
        } while (fprx_FindNextFileA(hFind, &ffd) != 0);
        //if (GetLastError() != ERROR_NO_MORE_FILES)
        FindClose(hFind);
    }
    
    //process offsets
    {
        int prevOff = 2 * FRAMES_PER_SECOND; //initial CD gap
        for (int i=1; i <= p_maxTrackNr; i++) {
            Track* trk = &cdtracks[i];
            trk->frameOff = prevOff;
            if (!trk->type == CDPF_DATA && trk->frameLen == 0) {
                trk->frameLen = 4 * FRAMES_PER_SECOND; //lazy default
            } else {
                trk->frameLen = trk->frameLen;
            }
            prevOff += trk->frameLen;
        }
    }
}