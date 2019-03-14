#ifndef UTIL_H
#define UTIL_H

#include <windows.h>
#include <stdint.h>

#ifdef WIN32
#define snprintf _snprintf
#endif

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define COUNTOF(x) sizeof(x) / sizeof(x[0])
#define ERR_PREFIX "ERR_WMMOGG: "

#define MAXERRBUF 512
char _errbuf[MAXERRBUF];
#define DERROR(format, ...) {snprintf(_errbuf, MAXERRBUF, ERR_PREFIX format, ##__VA_ARGS__); OutputDebugStringA(_errbuf);}
#ifdef _DEBUG
#define DVERBOSE(format, ...) {snprintf(_errbuf, MAXERRBUF, "DBG_WMMOGG: " format, ##__VA_ARGS__); OutputDebugStringA(_errbuf);}
#else 
#define DVERBOSE(...)
#endif


#define MAXINT32DIG 11+1
#define MAXUINT32DIG 11
//char* ib10FromStr(int32_t* dest, char* src, int max);
char* ub10FromStr(uint32_t* dest, char* src, int max);
//char* ib10ToStr(char* dest, int32_t  num, int size, BOOL isZPad);
char* ub10ToStr(char* dest, uint32_t num, int size, BOOL isZPad);
LPSTR  fnGetParentA(LPSTR  path);
LPWSTR fnGetParentW(LPWSTR path);
LPSTR  strWriteA2A(LPSTR dest,  LPCSTR src, int max);
LPSTR  strWriteW2A(LPSTR dest, LPCWSTR src, int max);
LPWSTR strWriteA2W(LPWSTR dest, LPCSTR src, int max);
//BOOL   strtokMatchTemplA(LPCSTR src, LPCSTR templ, CHAR magic);
LPSTR strtokWalkA(LPCSTR src, LPCSTR token);
//void concatStrA2A(LPSTR dest, LPCSTR src, int max);


#endif