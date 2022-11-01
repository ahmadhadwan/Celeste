#ifndef __CELESTE_INTERNAL_AUDIO_H__
#define __CELESTE_INTERNAL_AUDIO_H__

#ifdef CELESTE_GORILLA_AUDIO
    #include <gorilla/ga.h>
    #include <gorilla/gau.h>
#else
    #error "Define CELESTE_GORILLA_AUDIO macro!"
#endif /* CELESTE_GORILLA_AUDIO */

enum { CELESTE_AUDIO_TYPE_FILE = 0, CELESTE_AUDIO_TYPE_MEMORY };

typedef struct {
    int         type;
#ifdef CELESTE_GORILLA_AUDIO
    union {
        ga_Sound   *sound;
        ga_Memory  *memory;
    };
#endif /* CELESTE_GORILLA_AUDIO */
} CLSTaudio;

typedef struct {
#ifdef CELESTE_GORILLA_AUDIO
    ga_Handle            *handle;
    gau_SampleSourceLoop *loop;
#endif /* CELESTE_GORILLA_AUDIO */
} CLSTaudioplayer;

#endif /* __CELESTE_INTERNAL_AUDIO_H__ */
