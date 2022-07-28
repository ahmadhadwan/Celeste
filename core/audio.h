#ifndef __CELESTE_AUDIO_H__
#define __CELESTE_AUDIO_H__

#ifdef CELESTE_GORILLA_AUDIO
    #include <gorilla/ga.h>
    #include <gorilla/gau.h>
#else
    #error "Define CELESTE_GORILLA_AUDIO macro!"
#endif /* CELESTE_GORILLA_AUDIO */

typedef struct {
    const char *filepath;
#ifdef CELESTE_GORILLA_AUDIO
    ga_Sound   *sound;
#endif /* CELESTE_GORILLA_AUDIO */
} celeste_audio_t;

typedef struct {
    int   ended;
    float gain;
    float pitch;
    float pan;
#ifdef CELESTE_GORILLA_AUDIO
    ga_Handle            *handle;
    gau_SampleSourceLoop *loop;
#endif /* CELESTE_GORILLA_AUDIO */
} celeste_audio_player_t;

celeste_audio_t *celeste_audio_create(const char *filepath);
void celeste_audio_destroy(celeste_audio_t *audio);

celeste_audio_player_t *celeste_audio_player_create(celeste_audio_t *audio, int loop);
void celeste_audio_player_destroy(celeste_audio_player_t *player);

int celeste_audio_player_play(celeste_audio_player_t *player);
int celeste_audio_player_stop(celeste_audio_player_t  *player);

int celeste_audio_player_set_gain(celeste_audio_player_t *player, float gain);
int celeste_audio_player_set_pitch(celeste_audio_player_t *player, float pitch);
/*
 * Set Audio Pan.
 * It's the position of the audio!
 * pan < 0 (left)
 * pan > 0 (right)
 */
int celeste_audio_player_set_pan(celeste_audio_player_t *player, float pan);

#endif /* __CELESTE_AUDIO_H__ */
