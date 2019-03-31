#include <windows.h>
#include "winfile.h"
#include "util.h"
#include <stdio.h>

#define MAXPATH_A MAX_PATH
#define MAXPATH_W 32767
static const CHAR PATHMAGIC[] = "\\\\?\\";
#define MAGICLEN sizeof(PATHMAGIC)-1

static BOOL isInit;
static BOOL isWide;
static int relMax;
//static 

static LPSTR  pathBaseA;
static LPSTR  offA;
static LPWSTR pathBaseW;
static LPWSTR offW;
WIN32_FIND_DATAW tempWFDat;

static void transferWFDatA2W(LPWIN32_FIND_DATAW dest, LPWIN32_FIND_DATAA src) {
    dest->dwFileAttributes = src->dwFileAttributes;
    CopyMemory(&dest->ftCreationTime,   &src->ftCreationTime,   sizeof(FILETIME));
    CopyMemory(&dest->ftLastAccessTime, &src->ftLastAccessTime, sizeof(FILETIME));
    CopyMemory(&dest->ftLastWriteTime,  &src->ftLastWriteTime,  sizeof(FILETIME));
    dest->nFileSizeHigh = src->nFileSizeHigh;
    dest->nFileSizeLow  = src->nFileSizeLow;
    dest->dwReserved0   = src->dwReserved0;
    dest->dwReserved1   = src->dwReserved1;
    strWriteA2W(dest->cFileName,          src->cFileName,          MAX_PATH);
    strWriteA2W(dest->cAlternateFileName, src->cAlternateFileName, 14);
}
static void transferWFDatW2A(LPWIN32_FIND_DATAA dest, LPWIN32_FIND_DATAW src) {
    dest->dwFileAttributes = src->dwFileAttributes;
    CopyMemory(&dest->ftCreationTime,   &src->ftCreationTime,   sizeof(FILETIME));
    CopyMemory(&dest->ftLastAccessTime, &src->ftLastAccessTime, sizeof(FILETIME));
    CopyMemory(&dest->ftLastWriteTime,  &src->ftLastWriteTime,  sizeof(FILETIME));
    dest->nFileSizeHigh = src->nFileSizeHigh;
    dest->nFileSizeLow  = src->nFileSizeLow;
    dest->dwReserved0   = src->dwReserved0;
    dest->dwReserved1   = src->dwReserved1;
    strWriteW2A(dest->cFileName,          src->cFileName,          MAX_PATH);
    strWriteW2A(dest->cAlternateFileName, src->cAlternateFileName, 14);
}

BOOL fprx_init() {
    WCHAR wcDummy[2];
    DWORD size;
    
    //wide functions are stubs on pre-NT windows
    isWide = FALSE; //(GetModuleFileNameW(NULL, &wcDummy, 1) > 0);
    if (isWide) {
        if (!(pathBaseW = malloc(MAXPATH_W * sizeof(WCHAR)))) return FALSE;
        GetModuleFileNameW(NULL, strWriteA2W(pathBaseW, PATHMAGIC, MAGICLEN), MAXPATH_W - MAGICLEN);
        offW = fnGetParentW(pathBaseW);
        relMax = MAXPATH_W - (offW - pathBaseW);
        isInit = TRUE;
    } else {
        if (!(pathBaseA = malloc(MAXPATH_A))) return FALSE;
        GetModuleFileNameA(NULL, pathBaseA, MAXPATH_A);
        offA = fnGetParentA(pathBaseA);
        relMax = MAXPATH_A - (offA - pathBaseA) - MAGICLEN;
        isInit = TRUE;
    }
}

HANDLE fprx_CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD  dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
    HANDLE ret;
    
    if (!isInit || (lstrlenA(lpFileName) >= relMax)) return INVALID_HANDLE_VALUE;
    if (isWide) {
        strWriteA2W(offW, lpFileName, relMax);
        ret = CreateFileW(pathBaseW, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
    } else {
        strWriteA2A(offA, lpFileName, relMax);
        ret = CreateFileA(pathBaseA, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
    }
    return ret;
}

HANDLE fprx_FindFirstFileA(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData) {
    HANDLE ret;
    
    if (!isInit || (lstrlenA(lpFileName) >= relMax)) return INVALID_HANDLE_VALUE;
    if (isWide) {
        strWriteA2W(offW, lpFileName, relMax);
        transferWFDatA2W(&tempWFDat, lpFindFileData);
        ret = FindFirstFileW(pathBaseW, &tempWFDat);
        transferWFDatW2A(lpFindFileData, &tempWFDat);
    } else {
        strWriteA2A(offA, lpFileName, relMax);
        ret = FindFirstFileA(pathBaseA, lpFindFileData);
    }
    return ret;
}

BOOL fprx_FindNextFileA(HANDLE hFindFile, LPWIN32_FIND_DATAA lpFindFileData) {
    BOOL ret;
    
    if (!isInit) return FALSE;
    if (isWide) {
        transferWFDatA2W(&tempWFDat, lpFindFileData);
        ret = FindNextFileW(hFindFile, &tempWFDat);
        transferWFDatW2A(lpFindFileData, &tempWFDat);
    } else {
        ret = FindNextFileA(hFindFile, lpFindFileData);
    }
    return ret;
}

DWORD fprx_GetPrivateProfileSectionA(LPCSTR lpAppName, LPSTR lpReturnedString, DWORD nSize, LPCSTR lpFileName) {
    DWORD ret;
    
    if (!isInit) return FALSE;
    if (isWide) {
        ret = 0;
    } else {
        strWriteA2A(offA, lpFileName, relMax);
        ret = GetPrivateProfileSectionA(lpAppName, lpReturnedString, nSize, pathBaseA);
    }
    return ret;
}