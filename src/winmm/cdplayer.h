#ifndef CDPLAYER_H
#define CDPLAYER_H

#include <windows.h>

typedef enum CDP_ERR {
    CDPE_OK,
    CDPE_DEVICE,
    CDPE_RANGE
} CDP_ERR;

void cdplayer_init();

#endif