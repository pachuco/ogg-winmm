#include <windows.h>
#include <stdint.h>
#include "util.h"

#define A2I(x) ((uint8_t)(x-48))
#define I2A(x) ((uint8_t)(x+48))

/*uint8_t iLog10(int32_t num) {
    uint8_t i=0;
    do {num/=10;i++;} while(num!=0);
    return i;
}
uint8_t uLog10(uint32_t num) {
    uint8_t i=0;
    do {num/=10;i++;} while(num!=0);
    return i;
}

LPSTR ib10FromStr(int32_t* dest, LPSTR src, int max) {
    int64_t num=0;
    uint16_t i=0, ai=0;
    int altMax = lstrlenA(src);
    int sign = 1;
    
    max = MIN(max, altMax);
    if (max <= 0) return NULL;
    
    if (src[0] == '-') { sign = -1; i++; }
    while (A2I(src[i]) == 0) i++;
    for (; ai < max; ai++) {
        uint8_t dig = A2I(src[i+ai]);
        if (dig > 9) break;
        num = dig + num*10;
    }
    i += ai;
    if (sign<0) num = -num;
    if (num < INT32_MIN || num > INT32_MAX) return NULL;
    if (ai = 0) return NULL;
    
    *dest = (int32_t)num;
    return src+i;
}

LPSTR ib10ToStr(LPSTR dest, int32_t num, int size, BOOL isZPad) {
    #define BS 16
    CHAR tbuf[BS];
    int i=1;
    int sign = num < 0 ? -1 : 1;
    
    if (sign<0) size--;
    if (size==0 || size >= BS) return NULL;
    for (; i<=size; i++) {
        tbuf[BS-i] = I2A(num%10);
        num /= 10;
        if(num==0) break;
    }
    if(num!=0) return NULL;
    while (isZPad && i<size) {
        tbuf[BS-1-i] = '0';
        i++;
    }
    if (sign < 0) {
        tbuf[BS-i] = '-';
        i++;
    }
    memcpy(dest, tbuf+BS+1-i, i+1);
    return dest+i;
    #undef BS
}*/

LPSTR ub10FromStr(uint32_t* dest, LPSTR src, int max) {
    uint64_t num=0;
    uint16_t i=0, ai=0;
    int altMax = lstrlenA(src);
    uint8_t dig;
    
    max = MIN(max, altMax);
    if (max <= 0) return NULL;
    
    while (A2I(src[i]) == 0) i++;
    for (; ai < max; ai++) {
        uint8_t dig = A2I(src[i+ai]);
        if (dig > 9) break;
        num = dig + num*10;
    }
    i += ai;
    if (num > UINT32_MAX) return NULL;
    if (ai = 0) return NULL;
    
    *dest = (uint32_t)num;
    return src+i;
}

LPSTR ub10ToStr(LPSTR dest, uint32_t num, int size, BOOL isZPad) {
    #define BS 16
    CHAR tbuf[BS];
    int i=1;
    
    if (size==0 || size >= BS) return NULL;
    for (; i<=size; i++) {
        tbuf[BS-i] = I2A(num%10);
        num /= 10;
        if(num==0) break;
    }
    if(num!=0) return NULL;
    while (isZPad && i<size) {
        tbuf[BS-1-i] = '0';
        i++;
    }
    memcpy(dest, tbuf+BS-i, i);
    return dest+i;
    #undef BS
}


BOOL fnMatchesTemplate(LPCSTR src, LPCSTR templ) {
    int len = lstrlenA(src);
    
    if (len > lstrlenA(templ)) return FALSE;
    
    for (int i=0; i<len; i++) {
        CHAR s=src[i], t=templ[i];
        if (t == '?') continue;
        if (s != t) return FALSE;
    }
    return TRUE;
}

LPSTR fnGetParentA(LPSTR path) {
    int len = lstrlenA(path);
    int i;
    
    for (i=len; i>=0; i--) {
        if (path[i] == '\\' || path[i] == '/') break;
    }
    path[i] = '\0';
    return path+len;
}

LPWSTR fnGetParentW(LPWSTR path) {
    int len = lstrlenW(path);
    int i;
    
    for (i=len; i>=0; i--) {
        if (path[i] == '\\' || path[i] == '/') break;
    }
    path[i] = '\0';
    return path+len;
}

LPSTR writeStrA2A(LPSTR dest, LPCSTR src, int max) {
    int altMax = lstrlenA(src)+1;
    
    max = MIN(max, altMax);
    CopyMemory(dest, src, max);
    dest[max-1] = '\0';
    return dest+max-1;
}

LPSTR writeStrW2A(LPSTR dest, LPCWSTR src, int max) {
    int i;
    int altMax = lstrlenW(src)+1;
        
    max = MIN(max, altMax);
    for (i = 0; i<max; i++) {
        dest[i] = (CHAR)src[i];
    }
    dest[max-1] = '\0';
    return dest+max-1;
}

LPWSTR writeStrA2W(LPWSTR dest, LPCSTR src, int max) {
    int i;
    int altMax = lstrlenA(src)+1;
        
    max = MIN(max, altMax);
    for (i = 0; i<max; i++) {
        dest[i] = (WCHAR)src[i];
    }
    dest[max-1] = L'\0';
    return dest+max-1;
}

/*void concatStrA2A(LPSTR dest, LPSTR src, int max) {
    int l1 = lstrlenA(dest);
    int l2 = lstrlenA(src);
    
    if (max <= 0) return;
    if (l1 >= max) return;
    if (l1+l2 >= max) l2 = max-l1;
    CopyMemory(dest+l1, src, l2);
}*/