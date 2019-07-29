#ifndef DEBUG_H
#define DEBUG_H

#include <windows.h>

#define ERR_PREFIX "ERR_WMMOGG: "
#define MAXERRBUF 512
char _errbuf[MAXERRBUF];
#define DERROR(format, ...) {_snprintf(_errbuf, MAXERRBUF, ERR_PREFIX format, ##__VA_ARGS__); OutputDebugStringA(_errbuf);}
#ifdef _DEBUG
#define DVERBOSE(format, ...) {_snprintf(_errbuf, MAXERRBUF, "DBG_WMMOGG: " format, ##__VA_ARGS__); OutputDebugStringA(_errbuf);}
#else 
#define DVERBOSE(...)
#endif


#endif