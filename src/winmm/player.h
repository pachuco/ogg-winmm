#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>

void plr_stop();
DWORD plr_volumeGet();
void plr_volumeSet(DWORD dwVolume);
int plr_pump();
int plr_length(const char *path);
int plr_play(const char *path);

#endif
