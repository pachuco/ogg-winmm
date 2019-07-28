#ifndef STUBS_H
#define STUBS_H

#include <windows.h>

//creates prefixed global symbol
#define FN_IN(FN) \
__asm__(".globl _real_"#FN"; _real_"#FN":;"\
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
    "2: .ascii \"Route fail! \";"\
    "3: .asciz \""#FN"\";"\
);

//creates prefixed global symbol and EAT entry
#define FN_INOUT(FN) \
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
    "2: .ascii \"Pass fail! \";"\
    "3: .asciz \""#FN"\";"\
".section .drectve; .ascii \" -export:\\\""#FN"\\\"\"; .text;");

//defined stubs
MMRESULT WINAPI real_auxGetDevCapsA(UINT_PTR uDeviceID, LPAUXCAPS lpCaps, UINT cbCaps);
UINT WINAPI     real_auxGetNumDevs();
MMRESULT WINAPI real_auxGetVolume(UINT uDeviceID, LPDWORD lpdwVolume);
MMRESULT WINAPI real_auxSetVolume(UINT uDeviceID, DWORD dwVolume);
MCIERROR WINAPI real_mciSendCommandA(MCIDEVICEID IDDevice, UINT uMsg, DWORD_PTR fdwCommand, DWORD_PTR dwParam);
MCIERROR WINAPI real_mciSendStringA(LPCTSTR cmd, LPTSTR ret, UINT cchReturn, HANDLE hwndCallback);




#endif