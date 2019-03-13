#ifndef WINFILE_H
#define WINFILE_H

BOOL   fprx_init();
HANDLE fprx_CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD  dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
HANDLE fprx_FindFirstFileA(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData);
BOOL   fprx_FindNextFileA(HANDLE hFindFile, LPWIN32_FIND_DATAA lpFindFileData);

#endif