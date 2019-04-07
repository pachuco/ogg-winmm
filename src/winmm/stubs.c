#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

static HINSTANCE hRealWinmm = NULL;

__attribute__((used)) void* pGetProcAddress     = &GetProcAddress;
__attribute__((used)) void* pOutputDebugStringA = &OutputDebugStringA;

#define STUBFUNC(FN) \
__asm__(".globl _"#FN"; _"#FN":;"\
    "mov eax, [1f];"\
    "test eax, eax; jnz 10f;"\
    "call _loadRealDLL;"\
    "push offset 3f; push eax; call _GetProcAddress;"\
    "mov [1f], eax;"\
    "test eax, eax; jnz 10f;"\
    "push offset 2f; call _OutputDebugStringA;"\
    "int 3;"\
    "10:;"\
    "jmp [1f];"\
    ".data;"\
    "1: .int 0;"\
    "2: .ascii \"Stub fail! \";"\
    "3: .asciz \""#FN"\";"\
".section .drectve; .ascii \" -export:\\\""#FN"\\\"\"; .text;");

//watches for the app to close, unloads the library when it does
//since FreeLibrary is dangerous in DllMain
void ExitMonitor(LPVOID DLLHandle) {
    WaitForSingleObject(DLLHandle, INFINITE);
    FreeLibrary(hRealWinmm);
    hRealWinmm = NULL;
}

//if winmm.dll is already loaded, return its handle
//otherwise, load it
HINSTANCE loadRealDLL() {
    //if (hRealWinmm) return hRealWinmm;

    char winmm_path[MAX_PATH];

    GetSystemDirectoryA(winmm_path, MAX_PATH);
    strncat(winmm_path, "\\winmm.DLL", MAX_PATH);
    hRealWinmm = LoadLibraryA(winmm_path);
    
    //start watcher thread to close the library
    CreateThread(NULL, 500, (LPTHREAD_START_ROUTINE)ExitMonitor, GetCurrentThread(), 0, NULL);

    return hRealWinmm;
}

//non-standard functions
//Windows XP
STUBFUNC( gfxAddGfx );
STUBFUNC( gfxBatchChange );
STUBFUNC( gfxCreateGfxFactoriesList );
STUBFUNC( gfxCreateZoneFactoriesList );
STUBFUNC( gfxDestroyDeviceInterfaceList );
STUBFUNC( gfxEnumerateGfxs );
STUBFUNC( _gfxLogoff@0 );
STUBFUNC( _gfxLogon@4 );
STUBFUNC( gfxModifyGfx );
STUBFUNC( gfxOpenGfx );
STUBFUNC( gfxRemoveGfx );
STUBFUNC( MigrateAllDrivers );
STUBFUNC( MigrateSoundEvents );
STUBFUNC( winmmDbgOut );
STUBFUNC( WinmmLogoff );
STUBFUNC( WinmmLogon );
STUBFUNC( winmmSetDebugLevel );
//Windows 2000
STUBFUNC( MigrateMidiUser );
//Windows 98
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

//standard winmm functions
STUBFUNC( aux32Message );
//STUBFUNC( auxGetDevCapsA );
STUBFUNC( auxGetDevCapsW ); //TODO
STUBFUNC( auxGetNumDevs );
//STUBFUNC( auxGetVolume );
STUBFUNC( auxOutMessage );
//STUBFUNC( auxSetVolume );
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
STUBFUNC( mciExecute ); //TODO
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
//STUBFUNC( mciSendCommandA );
STUBFUNC( mciSendCommandW );
//STUBFUNC( mciSendStringA );
STUBFUNC( mciSendStringW ); //TODO
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