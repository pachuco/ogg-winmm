#include <windows.h>
#include <stdint.h>
#include "player.h"

#define NAKED __attribute__((naked))
//#define NAKED __declspec(naked)
static HINSTANCE realWinmmDLL = NULL;

enum STUBS {
    SE_gfxAddGfx,
    SE_gfxBatchChange,
    SE_gfxCreateGfxFactoriesList,
    SE_gfxCreateZoneFactoriesList,
    SE_gfxDestroyDeviceInterfaceList,
    SE_gfxEnumerateGfxs,
    //SE__gfxLogoff@0,
    //SE__gfxLogon@4,
    SE_gfxModifyGfx,
    SE_gfxOpenGfx,
    SE_gfxRemoveGfx,
    SE_MigrateAllDrivers,
    SE_MigrateSoundEvents,
    SE_winmmDbgOut,
    SE_WinmmLogoff,
    SE_WinmmLogon,
    SE_winmmSetDebugLevel,
    SE_MigrateMidiUser,
    SE_DrvClose,
    SE_DrvDefDriverProc,
    SE_DrvOpen,
    SE_DrvOpenA,
    SE_DrvSendMessage,
    SE_GetDriverFlags,
    SE_mmioInstallIOProc16,
    SE_OpenDriverA,
    SE_winmmf_ThunkData32,
    SE_winmmsl_ThunkData32,
    
    SE_aux32Message,
    SE_auxGetDevCapsA,
    SE_auxGetDevCapsW,
    SE_auxGetNumDevs,
    SE_auxGetVolume,
    SE_auxOutMessage,
    SE_auxSetVolume,
    SE_CloseDriver,
    SE_DefDriverProc,
    SE_DriverCallback,
    SE_DrvGetModuleHandle,
    SE_GetDriverModuleHandle,
    SE_joy32Message,
    SE_joyConfigChanged,
    SE_joyGetDevCapsA,
    SE_joyGetDevCapsW,
    SE_joyGetNumDevs,
    SE_joyGetPos,
    SE_joyGetPosEx,
    SE_joyGetThreshold,
    SE_joyReleaseCapture,
    SE_joySetCapture,
    SE_joySetThreshold,
    SE_mci32Message,
    SE_mciDriverNotify,
    SE_mciDriverYield,
    SE_mciExecute,
    SE_mciFreeCommandResource,
    SE_mciGetCreatorTask,
    SE_mciGetDeviceIDA,
    SE_mciGetDeviceIDFromElementIDA,
    SE_mciGetDeviceIDFromElementIDW,
    SE_mciGetDeviceIDW,
    SE_mciGetDriverData,
    SE_mciGetErrorStringA,
    SE_mciGetErrorStringW,
    SE_mciGetYieldProc,
    SE_mciLoadCommandResource,
    SE_mciSendCommandA,
    SE_mciSendCommandW,
    SE_mciSendStringA,
    SE_mciSendStringW,
    SE_mciSetDriverData,
    SE_mciSetYieldProc,
    SE_mid32Message,
    SE_midiConnect,
    SE_midiDisconnect,
    SE_midiInAddBuffer,
    SE_midiInClose,
    SE_midiInGetDevCapsA,
    SE_midiInGetDevCapsW,
    SE_midiInGetErrorTextA,
    SE_midiInGetErrorTextW,
    SE_midiInGetID,
    SE_midiInGetNumDevs,
    SE_midiInMessage,
    SE_midiInOpen,
    SE_midiInPrepareHeader,
    SE_midiInReset,
    SE_midiInStart,
    SE_midiInStop,
    SE_midiInUnprepareHeader,
    SE_midiOutCacheDrumPatches,
    SE_midiOutCachePatches,
    SE_midiOutClose,
    SE_midiOutGetDevCapsA,
    SE_midiOutGetDevCapsW,
    SE_midiOutGetErrorTextA,
    SE_midiOutGetErrorTextW,
    SE_midiOutGetID,
    SE_midiOutGetNumDevs,
    SE_midiOutGetVolume,
    SE_midiOutLongMsg,
    SE_midiOutMessage,
    SE_midiOutOpen,
    SE_midiOutPrepareHeader,
    SE_midiOutReset,
    SE_midiOutSetVolume,
    SE_midiOutShortMsg,
    SE_midiOutUnprepareHeader,
    SE_midiStreamClose,
    SE_midiStreamOpen,
    SE_midiStreamOut,
    SE_midiStreamPause,
    SE_midiStreamPosition,
    SE_midiStreamProperty,
    SE_midiStreamRestart,
    SE_midiStreamStop,
    SE_mixerClose,
    SE_mixerGetControlDetailsA,
    SE_mixerGetControlDetailsW,
    SE_mixerGetDevCapsA,
    SE_mixerGetDevCapsW,
    SE_mixerGetID,
    SE_mixerGetLineControlsA,
    SE_mixerGetLineControlsW,
    SE_mixerGetLineInfoA,
    SE_mixerGetLineInfoW,
    SE_mixerGetNumDevs,
    SE_mixerMessage,
    SE_mixerOpen,
    SE_mixerSetControlDetails,
    SE_mmDrvInstall,
    SE_mmGetCurrentTask,
    SE_mmioAdvance,
    SE_mmioAscend,
    SE_mmioClose,
    SE_mmioCreateChunk,
    SE_mmioDescend,
    SE_mmioFlush,
    SE_mmioGetInfo,
    SE_mmioInstallIOProcA,
    SE_mmioInstallIOProcW,
    SE_mmioOpenA,
    SE_mmioOpenW,
    SE_mmioRead,
    SE_mmioRenameA,
    SE_mmioRenameW,
    SE_mmioSeek,
    SE_mmioSendMessage,
    SE_mmioSetBuffer,
    SE_mmioSetInfo,
    SE_mmioStringToFOURCCA,
    SE_mmioStringToFOURCCW,
    SE_mmioWrite,
    SE_mmsystemGetVersion,
    SE_mmTaskBlock,
    SE_mmTaskCreate,
    SE_mmTaskSignal,
    SE_mmTaskYield,
    SE_mod32Message,
    SE_mxd32Message,
    SE_NotifyCallbackData,
    SE_OpenDriver,
    SE_PlaySound,
    SE_PlaySoundA,
    SE_PlaySoundW,
    SE_SendDriverMessage,
    SE_sndPlaySoundA,
    SE_sndPlaySoundW,
    SE_tid32Message,
    SE_timeBeginPeriod,
    SE_timeEndPeriod,
    SE_timeGetDevCaps,
    SE_timeGetSystemTime,
    SE_timeGetTime,
    SE_timeKillEvent,
    SE_timeSetEvent,
    SE_waveInAddBuffer,
    SE_waveInClose,
    SE_waveInGetDevCapsA,
    SE_waveInGetDevCapsW,
    SE_waveInGetErrorTextA,
    SE_waveInGetErrorTextW,
    SE_waveInGetID,
    SE_waveInGetNumDevs,
    SE_waveInGetPosition,
    SE_waveInMessage,
    SE_waveInOpen,
    SE_waveInPrepareHeader,
    SE_waveInReset,
    SE_waveInStart,
    SE_waveInStop,
    SE_waveInUnprepareHeader,
    SE_waveOutBreakLoop,
    SE_waveOutClose,
    SE_waveOutGetDevCapsA,
    SE_waveOutGetDevCapsW,
    SE_waveOutGetErrorTextA,
    SE_waveOutGetErrorTextW,
    SE_waveOutGetID,
    SE_waveOutGetNumDevs,
    SE_waveOutGetPitch,
    SE_waveOutGetPlaybackRate,
    SE_waveOutGetPosition,
    SE_waveOutGetVolume,
    SE_waveOutMessage,
    SE_waveOutOpen,
    SE_waveOutPause,
    SE_waveOutPrepareHeader,
    SE_waveOutReset,
    SE_waveOutRestart,
    SE_waveOutSetPitch,
    SE_waveOutSetPlaybackRate,
    SE_waveOutSetVolume,
    SE_waveOutUnprepareHeader,
    SE_waveOutWrite,
    SE_wid32Message,
    SE_wod32Message,
    SE_WOW32DriverCallback,
    SE_WOW32ResolveMultiMediaHandle,
    SE_WOWAppExit
};


#define STUBFUNC(FN) char stub_##FN[8];
//#define STUBFUNC(FN)\
//static void* fptr_##FN;\
//static char fn_##FN[] = #FN;\
//void __stdcall stub_##FN(void);\
//__asm__(".globl _stub_"#FN"@0; _stub_"#FN"@0:");\
//__asm__(".intel_syntax noprefix;"\
//    "mov eax, _fptr_"#FN";"\
//    "test eax, eax; jz JANKYHOOK_END"#FN";"\
//    "call _loadRealDLL;"\
//    "push _fn_"#FN"; push eax; call _GetProcAddress;"\
//    "mov _fptr_"#FN", eax;"\
//    "JANKYHOOK_END"#FN":;"\
//    "jmp _fptr_"#FN";"\
//".att_syntax prefix;");\
//__asm__(".section .drectve .ascii \" -export:\\\"stub_"#FN"@0\\\"\"");\
//__asm__(".section .text");

//GCC doesn't let me be naked
#define STUBFUNC_OLD(FN)\
__attribute__((naked)) void stub_##FN() {\
    static void* ptr; \
    if (!ptr) ptr = (void*)GetProcAddress(loadRealDLL(), #FN); \
    asm(".intel_syntax noprefix;"\
    \
        "jmp %0;"\
    ".att_syntax prefix;" : "=m"(ptr) \
    );\
}

//watches for the app to close, unloads the library when it does
//since FreeLibrary is dangerous in DllMain
void ExitMonitor(LPVOID DLLHandle) {
    WaitForSingleObject(DLLHandle, INFINITE);
    FreeLibrary(realWinmmDLL);
    realWinmmDLL = NULL;
}

//if winmm.dll is already loaded, return its handle
//otherwise, load it
HINSTANCE loadRealDLL() {
    if (realWinmmDLL)
        return realWinmmDLL;

    char winmm_path[MAX_PATH];

    GetSystemDirectoryA(winmm_path, MAX_PATH);
    strncat(winmm_path, "//winmm.DLL", MAX_PATH);

    realWinmmDLL = LoadLibraryA(winmm_path);
    
    //start watcher thread to close the library
    CreateThread(NULL, 500, (LPTHREAD_START_ROUTINE)ExitMonitor, GetCurrentThread(), 0, NULL);

    return realWinmmDLL;
}

//
//stubs for functions to call from the real winmm.dll
//

LRESULT WINAPI oldstub_CloseDriver(HDRVR a0, LONG a1, LONG a2) {
    static LRESULT(WINAPI *funcp)(HDRVR a0, LONG a1, LONG a2) = NULL;
    if (funcp == NULL)
        funcp = (void*)GetProcAddress(loadRealDLL(), "CloseDriver");
    return (*funcp)(a0, a1, a2);
}

STUBFUNC( gfxAddGfx );
STUBFUNC( gfxBatchChange );
STUBFUNC( gfxCreateGfxFactoriesList );
STUBFUNC( gfxCreateZoneFactoriesList );
STUBFUNC( gfxDestroyDeviceInterfaceList );
STUBFUNC( gfxEnumerateGfxs );
//STUBFUNC( SE__gfxLogoff@0 );
//STUBFUNC( SE__gfxLogon@4 );
STUBFUNC( gfxModifyGfx );
STUBFUNC( gfxOpenGfx );
STUBFUNC( gfxRemoveGfx );
STUBFUNC( MigrateAllDrivers );
STUBFUNC( MigrateSoundEvents );
STUBFUNC( winmmDbgOut );
STUBFUNC( WinmmLogoff );
STUBFUNC( WinmmLogon );
STUBFUNC( winmmSetDebugLevel );
STUBFUNC( MigrateMidiUser );
STUBFUNC( DrvClose );
STUBFUNC( DrvDefDriverProc );
STUBFUNC( DrvOpen );
STUBFUNC( DrvOpenA );
STUBFUNC( DrvSendMessage );
STUBFUNC( GetDriverFlags );
STUBFUNC( mmioInstallIOProc16 );
STUBFUNC( OpenDriverA );
STUBFUNC( winmmf_ThunkData32 );
STUBFUNC( winmmsl_ThunkData32 );

STUBFUNC( aux32Message );
STUBFUNC( auxGetDevCapsA );
STUBFUNC( auxGetDevCapsW );
STUBFUNC( auxGetNumDevs );
STUBFUNC( auxGetVolume );
STUBFUNC( auxOutMessage );
STUBFUNC( auxSetVolume );
STUBFUNC( CloseDriver );
STUBFUNC( DefDriverProc );
STUBFUNC( DriverCallback );
STUBFUNC( DrvGetModuleHandle );
STUBFUNC( GetDriverModuleHandle );
STUBFUNC( joy32Message );
STUBFUNC( joyConfigChanged );
STUBFUNC( joyGetDevCapsA );
STUBFUNC( joyGetDevCapsW );
STUBFUNC( joyGetNumDevs );
STUBFUNC( joyGetPos );
STUBFUNC( joyGetPosEx );
STUBFUNC( joyGetThreshold );
STUBFUNC( joyReleaseCapture );
STUBFUNC( joySetCapture );
STUBFUNC( joySetThreshold );
STUBFUNC( mci32Message );
STUBFUNC( mciDriverNotify );
STUBFUNC( mciDriverYield );
STUBFUNC( mciExecute );
STUBFUNC( mciFreeCommandResource );
STUBFUNC( mciGetCreatorTask );
STUBFUNC( mciGetDeviceIDA );
STUBFUNC( mciGetDeviceIDFromElementIDA );
STUBFUNC( mciGetDeviceIDFromElementIDW );
STUBFUNC( mciGetDeviceIDW );
STUBFUNC( mciGetDriverData );
STUBFUNC( mciGetErrorStringA );
STUBFUNC( mciGetErrorStringW );
STUBFUNC( mciGetYieldProc );
STUBFUNC( mciLoadCommandResource );
STUBFUNC( mciSendCommandA );
STUBFUNC( mciSendCommandW );
STUBFUNC( mciSendStringA );
STUBFUNC( mciSendStringW );
STUBFUNC( mciSetDriverData );
STUBFUNC( mciSetYieldProc );
STUBFUNC( mid32Message );
STUBFUNC( midiConnect );
STUBFUNC( midiDisconnect );
STUBFUNC( midiInAddBuffer );
STUBFUNC( midiInClose );
STUBFUNC( midiInGetDevCapsA );
STUBFUNC( midiInGetDevCapsW );
STUBFUNC( midiInGetErrorTextA );
STUBFUNC( midiInGetErrorTextW );
STUBFUNC( midiInGetID );
STUBFUNC( midiInGetNumDevs );
STUBFUNC( midiInMessage );
STUBFUNC( midiInOpen );
STUBFUNC( midiInPrepareHeader );
STUBFUNC( midiInReset );
STUBFUNC( midiInStart );
STUBFUNC( midiInStop );
STUBFUNC( midiInUnprepareHeader );
STUBFUNC( midiOutCacheDrumPatches );
STUBFUNC( midiOutCachePatches );
STUBFUNC( midiOutClose );
STUBFUNC( midiOutGetDevCapsA );
STUBFUNC( midiOutGetDevCapsW );
STUBFUNC( midiOutGetErrorTextA );
STUBFUNC( midiOutGetErrorTextW );
STUBFUNC( midiOutGetID );
STUBFUNC( midiOutGetNumDevs );
STUBFUNC( midiOutGetVolume );
STUBFUNC( midiOutLongMsg );
STUBFUNC( midiOutMessage );
STUBFUNC( midiOutOpen );
STUBFUNC( midiOutPrepareHeader );
STUBFUNC( midiOutReset );
STUBFUNC( midiOutSetVolume );
STUBFUNC( midiOutShortMsg );
STUBFUNC( midiOutUnprepareHeader );
STUBFUNC( midiStreamClose );
STUBFUNC( midiStreamOpen );
STUBFUNC( midiStreamOut );
STUBFUNC( midiStreamPause );
STUBFUNC( midiStreamPosition );
STUBFUNC( midiStreamProperty );
STUBFUNC( midiStreamRestart );
STUBFUNC( midiStreamStop );
STUBFUNC( mixerClose );
STUBFUNC( mixerGetControlDetailsA );
STUBFUNC( mixerGetControlDetailsW );
STUBFUNC( mixerGetDevCapsA );
STUBFUNC( mixerGetDevCapsW );
STUBFUNC( mixerGetID );
STUBFUNC( mixerGetLineControlsA );
STUBFUNC( mixerGetLineControlsW );
STUBFUNC( mixerGetLineInfoA );
STUBFUNC( mixerGetLineInfoW );
STUBFUNC( mixerGetNumDevs );
STUBFUNC( mixerMessage );
STUBFUNC( mixerOpen );
STUBFUNC( mixerSetControlDetails );
STUBFUNC( mmDrvInstall );
STUBFUNC( mmGetCurrentTask );
STUBFUNC( mmioAdvance );
STUBFUNC( mmioAscend );
STUBFUNC( mmioClose );
STUBFUNC( mmioCreateChunk );
STUBFUNC( mmioDescend );
STUBFUNC( mmioFlush );
STUBFUNC( mmioGetInfo );
STUBFUNC( mmioInstallIOProcA );
STUBFUNC( mmioInstallIOProcW );
STUBFUNC( mmioOpenA );
STUBFUNC( mmioOpenW );
STUBFUNC( mmioRead );
STUBFUNC( mmioRenameA );
STUBFUNC( mmioRenameW );
STUBFUNC( mmioSeek );
STUBFUNC( mmioSendMessage );
STUBFUNC( mmioSetBuffer );
STUBFUNC( mmioSetInfo );
STUBFUNC( mmioStringToFOURCCA );
STUBFUNC( mmioStringToFOURCCW );
STUBFUNC( mmioWrite );
STUBFUNC( mmsystemGetVersion );
STUBFUNC( mmTaskBlock );
STUBFUNC( mmTaskCreate );
STUBFUNC( mmTaskSignal );
STUBFUNC( mmTaskYield );
STUBFUNC( mod32Message );
STUBFUNC( mxd32Message );
STUBFUNC( NotifyCallbackData );
STUBFUNC( OpenDriver );
STUBFUNC( PlaySound );
STUBFUNC( PlaySoundA );
STUBFUNC( PlaySoundW );
STUBFUNC( SendDriverMessage );
STUBFUNC( sndPlaySoundA );
STUBFUNC( sndPlaySoundW );
STUBFUNC( tid32Message );
STUBFUNC( timeBeginPeriod );
STUBFUNC( timeEndPeriod );
STUBFUNC( timeGetDevCaps );
STUBFUNC( timeGetSystemTime );
STUBFUNC( timeGetTime );
STUBFUNC( timeKillEvent );
STUBFUNC( timeSetEvent );
STUBFUNC( waveInAddBuffer );
STUBFUNC( waveInClose );
STUBFUNC( waveInGetDevCapsA );
STUBFUNC( waveInGetDevCapsW );
STUBFUNC( waveInGetErrorTextA );
STUBFUNC( waveInGetErrorTextW );
STUBFUNC( waveInGetID );
STUBFUNC( waveInGetNumDevs );
STUBFUNC( waveInGetPosition );
STUBFUNC( waveInMessage );
STUBFUNC( waveInOpen );
STUBFUNC( waveInPrepareHeader );
STUBFUNC( waveInReset );
STUBFUNC( waveInStart );
STUBFUNC( waveInStop );
STUBFUNC( waveInUnprepareHeader );
STUBFUNC( waveOutBreakLoop );
STUBFUNC( waveOutClose );
STUBFUNC( waveOutGetDevCapsA );
STUBFUNC( waveOutGetDevCapsW );
STUBFUNC( waveOutGetErrorTextA );
STUBFUNC( waveOutGetErrorTextW );
STUBFUNC( waveOutGetID );
STUBFUNC( waveOutGetNumDevs );
STUBFUNC( waveOutGetPitch );
STUBFUNC( waveOutGetPlaybackRate );
STUBFUNC( waveOutGetPosition );
STUBFUNC( waveOutGetVolume );
STUBFUNC( waveOutMessage );
STUBFUNC( waveOutOpen );
STUBFUNC( waveOutPause );
STUBFUNC( waveOutPrepareHeader );
STUBFUNC( waveOutReset );
STUBFUNC( waveOutRestart );
STUBFUNC( waveOutSetPitch );
STUBFUNC( waveOutSetPlaybackRate );
STUBFUNC( waveOutSetVolume );
STUBFUNC( waveOutUnprepareHeader );
STUBFUNC( waveOutWrite );
STUBFUNC( wid32Message );
STUBFUNC( wod32Message );
STUBFUNC( WOW32DriverCallback );
STUBFUNC( WOW32ResolveMultiMediaHandle );
STUBFUNC( WOWAppExit );