#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "stubs.h"

static HINSTANCE hRealDLL = NULL;
static __attribute__((used)) void* pGetProcAddress     = &GetProcAddress;
static __attribute__((used)) void* pOutputDebugStringA = &OutputDebugStringA;

//watches for the app to close, unloads the library when it does
//since FreeLibrary is dangerous in DllMain
void ExitMonitor(LPVOID DLLHandle) {
    WaitForSingleObject(DLLHandle, INFINITE);
    FreeLibrary(hRealDLL);
    hRealDLL = NULL;
}

//if winmm.dll is already loaded, return its handle
//otherwise, load it
HINSTANCE loadRealDLL() {
    if (hRealDLL) return hRealDLL;

    char winmm_path[MAX_PATH+10];

    GetSystemDirectoryA(winmm_path, MAX_PATH);
    strncat(winmm_path, "\\winmm.DLL", MAX_PATH);
    hRealDLL = LoadLibraryA(winmm_path);
    
    //start watcher thread to close the library
    CreateThread(NULL, 500, (LPTHREAD_START_ROUTINE)ExitMonitor, GetCurrentThread(), 0, NULL);

    return hRealDLL;
}

//Routed functions
//in case we need to call the real functions
FN_IN( auxGetDevCapsA );
FN_IN( auxGetNumDevs );
FN_IN( auxGetVolume );
FN_IN( auxSetVolume );
FN_IN( mciSendCommandA );
FN_IN( mciSendStringA );

//non-standard functions
//Windows XP
FN_INOUT( gfxAddGfx );
FN_INOUT( gfxBatchChange );
FN_INOUT( gfxCreateGfxFactoriesList );
FN_INOUT( gfxCreateZoneFactoriesList );
FN_INOUT( gfxDestroyDeviceInterfaceList );
FN_INOUT( gfxEnumerateGfxs );
FN_INOUT( _gfxLogoff@0 );
FN_INOUT( _gfxLogon@4 );
FN_INOUT( gfxModifyGfx );
FN_INOUT( gfxOpenGfx );
FN_INOUT( gfxRemoveGfx );
FN_INOUT( MigrateAllDrivers );
FN_INOUT( MigrateSoundEvents );
FN_INOUT( winmmDbgOut );
FN_INOUT( WinmmLogoff );
FN_INOUT( WinmmLogon );
FN_INOUT( winmmSetDebugLevel );
//Windows 2000
FN_INOUT( MigrateMidiUser );
//Windows 98
FN_INOUT( DrvClose );
FN_INOUT( DrvDefDriverProc );
FN_INOUT( DrvOpen );
FN_INOUT( DrvOpenA );
FN_INOUT( DrvSendMessage );
FN_INOUT( GetDriverFlags );
FN_INOUT( mmioInstallIOProc16 );
FN_INOUT( OpenDriverA );
FN_INOUT( winmmf_ThunkData32 );
FN_INOUT( winmmsl_ThunkData32 );

//standard winmm functions
FN_INOUT( aux32Message );
FN_INOUT( auxGetDevCapsW ); //TODO
FN_INOUT( auxOutMessage );
FN_INOUT( CloseDriver );
FN_INOUT( DefDriverProc );
FN_INOUT( DriverCallback );
FN_INOUT( DrvGetModuleHandle );
FN_INOUT( GetDriverModuleHandle );
FN_INOUT( joy32Message );
FN_INOUT( joyConfigChanged );
FN_INOUT( joyGetDevCapsA );
FN_INOUT( joyGetDevCapsW );
FN_INOUT( joyGetNumDevs );
FN_INOUT( joyGetPos );
FN_INOUT( joyGetPosEx );
FN_INOUT( joyGetThreshold );
FN_INOUT( joyReleaseCapture );
FN_INOUT( joySetCapture );
FN_INOUT( joySetThreshold );
FN_INOUT( mci32Message );
FN_INOUT( mciDriverNotify );
FN_INOUT( mciDriverYield );
FN_INOUT( mciExecute ); //TODO
FN_INOUT( mciFreeCommandResource );
FN_INOUT( mciGetCreatorTask );
FN_INOUT( mciGetDeviceIDA );
FN_INOUT( mciGetDeviceIDFromElementIDA );
FN_INOUT( mciGetDeviceIDFromElementIDW );
FN_INOUT( mciGetDeviceIDW );
FN_INOUT( mciGetDriverData );
FN_INOUT( mciGetErrorStringA );
FN_INOUT( mciGetErrorStringW );
FN_INOUT( mciGetYieldProc );
FN_INOUT( mciLoadCommandResource );
FN_INOUT( mciSendCommandW ); //TODO
FN_INOUT( mciSendStringW ); //TODO
FN_INOUT( mciSetDriverData );
FN_INOUT( mciSetYieldProc );
FN_INOUT( mid32Message );
FN_INOUT( midiConnect );
FN_INOUT( midiDisconnect );
FN_INOUT( midiInAddBuffer );
FN_INOUT( midiInClose );
FN_INOUT( midiInGetDevCapsA );
FN_INOUT( midiInGetDevCapsW );
FN_INOUT( midiInGetErrorTextA );
FN_INOUT( midiInGetErrorTextW );
FN_INOUT( midiInGetID );
FN_INOUT( midiInGetNumDevs );
FN_INOUT( midiInMessage );
FN_INOUT( midiInOpen );
FN_INOUT( midiInPrepareHeader );
FN_INOUT( midiInReset );
FN_INOUT( midiInStart );
FN_INOUT( midiInStop );
FN_INOUT( midiInUnprepareHeader );
FN_INOUT( midiOutCacheDrumPatches );
FN_INOUT( midiOutCachePatches );
FN_INOUT( midiOutClose );
FN_INOUT( midiOutGetDevCapsA );
FN_INOUT( midiOutGetDevCapsW );
FN_INOUT( midiOutGetErrorTextA );
FN_INOUT( midiOutGetErrorTextW );
FN_INOUT( midiOutGetID );
FN_INOUT( midiOutGetNumDevs );
FN_INOUT( midiOutGetVolume );
FN_INOUT( midiOutLongMsg );
FN_INOUT( midiOutMessage );
FN_INOUT( midiOutOpen );
FN_INOUT( midiOutPrepareHeader );
FN_INOUT( midiOutReset );
FN_INOUT( midiOutSetVolume );
FN_INOUT( midiOutShortMsg );
FN_INOUT( midiOutUnprepareHeader );
FN_INOUT( midiStreamClose );
FN_INOUT( midiStreamOpen );
FN_INOUT( midiStreamOut );
FN_INOUT( midiStreamPause );
FN_INOUT( midiStreamPosition );
FN_INOUT( midiStreamProperty );
FN_INOUT( midiStreamRestart );
FN_INOUT( midiStreamStop );
FN_INOUT( mixerClose );
FN_INOUT( mixerGetControlDetailsA );
FN_INOUT( mixerGetControlDetailsW );
FN_INOUT( mixerGetDevCapsA );
FN_INOUT( mixerGetDevCapsW );
FN_INOUT( mixerGetID );
FN_INOUT( mixerGetLineControlsA );
FN_INOUT( mixerGetLineControlsW );
FN_INOUT( mixerGetLineInfoA );
FN_INOUT( mixerGetLineInfoW );
FN_INOUT( mixerGetNumDevs );
FN_INOUT( mixerMessage );
FN_INOUT( mixerOpen );
FN_INOUT( mixerSetControlDetails );
FN_INOUT( mmDrvInstall );
FN_INOUT( mmGetCurrentTask );
FN_INOUT( mmioAdvance );
FN_INOUT( mmioAscend );
FN_INOUT( mmioClose );
FN_INOUT( mmioCreateChunk );
FN_INOUT( mmioDescend );
FN_INOUT( mmioFlush );
FN_INOUT( mmioGetInfo );
FN_INOUT( mmioInstallIOProcA );
FN_INOUT( mmioInstallIOProcW );
FN_INOUT( mmioOpenA );
FN_INOUT( mmioOpenW );
FN_INOUT( mmioRead );
FN_INOUT( mmioRenameA );
FN_INOUT( mmioRenameW );
FN_INOUT( mmioSeek );
FN_INOUT( mmioSendMessage );
FN_INOUT( mmioSetBuffer );
FN_INOUT( mmioSetInfo );
FN_INOUT( mmioStringToFOURCCA );
FN_INOUT( mmioStringToFOURCCW );
FN_INOUT( mmioWrite );
FN_INOUT( mmsystemGetVersion );
FN_INOUT( mmTaskBlock );
FN_INOUT( mmTaskCreate );
FN_INOUT( mmTaskSignal );
FN_INOUT( mmTaskYield );
FN_INOUT( mod32Message );
FN_INOUT( mxd32Message );
FN_INOUT( NotifyCallbackData );
FN_INOUT( OpenDriver );
FN_INOUT( PlaySound );
FN_INOUT( PlaySoundA );
FN_INOUT( PlaySoundW );
FN_INOUT( SendDriverMessage );
FN_INOUT( sndPlaySoundA );
FN_INOUT( sndPlaySoundW );
FN_INOUT( tid32Message );
FN_INOUT( timeBeginPeriod );
FN_INOUT( timeEndPeriod );
FN_INOUT( timeGetDevCaps );
FN_INOUT( timeGetSystemTime );
FN_INOUT( timeGetTime );
FN_INOUT( timeKillEvent );
FN_INOUT( timeSetEvent );
FN_INOUT( waveInAddBuffer );
FN_INOUT( waveInClose );
FN_INOUT( waveInGetDevCapsA );
FN_INOUT( waveInGetDevCapsW );
FN_INOUT( waveInGetErrorTextA );
FN_INOUT( waveInGetErrorTextW );
FN_INOUT( waveInGetID );
FN_INOUT( waveInGetNumDevs );
FN_INOUT( waveInGetPosition );
FN_INOUT( waveInMessage );
FN_INOUT( waveInOpen );
FN_INOUT( waveInPrepareHeader );
FN_INOUT( waveInReset );
FN_INOUT( waveInStart );
FN_INOUT( waveInStop );
FN_INOUT( waveInUnprepareHeader );
FN_INOUT( waveOutBreakLoop );
FN_INOUT( waveOutClose );
FN_INOUT( waveOutGetDevCapsA );
FN_INOUT( waveOutGetDevCapsW );
FN_INOUT( waveOutGetErrorTextA );
FN_INOUT( waveOutGetErrorTextW );
FN_INOUT( waveOutGetID );
FN_INOUT( waveOutGetNumDevs );
FN_INOUT( waveOutGetPitch );
FN_INOUT( waveOutGetPlaybackRate );
FN_INOUT( waveOutGetPosition );
FN_INOUT( waveOutGetVolume );
FN_INOUT( waveOutMessage );
FN_INOUT( waveOutOpen );
FN_INOUT( waveOutPause );
FN_INOUT( waveOutPrepareHeader );
FN_INOUT( waveOutReset );
FN_INOUT( waveOutRestart );
FN_INOUT( waveOutSetPitch );
FN_INOUT( waveOutSetPlaybackRate );
FN_INOUT( waveOutSetVolume );
FN_INOUT( waveOutUnprepareHeader );
FN_INOUT( waveOutWrite );
FN_INOUT( wid32Message );
FN_INOUT( wod32Message );
FN_INOUT( WOW32DriverCallback );
FN_INOUT( WOW32ResolveMultiMediaHandle );
FN_INOUT( WOWAppExit );