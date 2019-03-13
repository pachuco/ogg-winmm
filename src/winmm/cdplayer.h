#ifndef CDPLAYER_H
#define CDPLAYER_H

#include <windows.h>

typedef enum CDP_ERR {
    CDPE_OK,
    CDPE_DEVICE,
    CDPE_NOMEDIA
} CDP_ERR;

CDP_ERR cdplayer_init();
void cplayer_stop();

#endif