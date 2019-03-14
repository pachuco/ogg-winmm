#include <stdio.h>
#include <windows.h>

#define BUFSIZE 1024
static char comBuf[BUFSIZE];
static char errBuf[BUFSIZE];
static char retBuf[BUFSIZE];


static size_t _getline(char* lineptr, size_t n, FILE* stream) {
    int i;
    
    if(!lineptr) return 0;
    fgets(lineptr, n, stream);
    for(i=0; i<n; i++) {
        if (lineptr[i]=='\n') lineptr[i] = '\0';
        if (lineptr[i]=='\0') break;
    }
    
    return i;
}

int main(int argc, char* argv[]) {
    size_t size;
    MCIERROR err;
    
    printf("Welcome to the MCI commandline.\n");
    printf("Type at your heart's content.\n");
    printf("Type \"exit\" to close MCI commandline.\n\n");
    while(TRUE) {
        printf("MCI>");
        size = _getline(comBuf, BUFSIZE, stdin);
        if (!strcmp(comBuf, "exit")) break;
        err = mciSendStringA(comBuf, retBuf, BUFSIZE, NULL);
        mciGetErrorString(err, errBuf, BUFSIZE);
        printf("ret|%s\n", retBuf);
        printf("err|%s\n", errBuf);
    }
    
    return 0;
}