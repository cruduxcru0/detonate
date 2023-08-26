#ifndef io_h_
#define io_h_

#ifndef LIBRETRO

#include <stdint.h>

void audio_mix(void *samples, size_t size);
bool audio_init(float refreshra, float input_srate, float fps, bool fp);
void audio_changeratefps(float refreshra, float input_srate, float fps);
void audio_destroy();

#endif

#endif