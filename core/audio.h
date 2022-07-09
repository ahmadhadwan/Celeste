#ifndef __CELESTE_AUDIO_H__
#define __CELESTE_AUDIO_H__

#include "celeste.h"

typedef struct {
    const char *filepath;
    int         ended;
    float       gain;
    float       pitch;
    float       pan;
#ifdef CELESTE_GORILLA_AUDIO
    ga_Sound             *sound;
    ga_Handle            *handle;
    gau_SampleSourceLoop *loop;
#else
    #error "Define CELESTE_GORILLA_AUDIO macro!"
#endif /* CELESTE_GORILLA_AUDIO */
} celeste_audio_t;

celeste_audio_t *celeste_audio_create(const char *filepath, int loop);
void celeste_audio_destroy(celeste_audio_t *audio);

void celeste_audio_play(celeste_audio_t *audio);
void celeste_audio_stop(celeste_audio_t *audio);

int celeste_audio_set_gain(celeste_audio_t *audio, float gain);
int celeste_audio_set_pitch(celeste_audio_t *audio, float pitch);
/*
 * Set Audio Pan.
 * It's the position of the audio!
 * pan < 0 (left)
 * pan > 0 (right)
 */
int celeste_audio_set_pan(celeste_audio_t *audio, float pan);

#endif /* __CELESTE_AUDIO_H__ */
