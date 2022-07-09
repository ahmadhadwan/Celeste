#include "audio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *get_file_extension(const char *filepath);
static void setFlagAndDestroyOnFinish(ga_Handle *in_handle, void *in_context);

celeste_audio_t *celeste_audio_create(const char *filepath, int loop)
{
    celeste_audio_t *audio = malloc(sizeof(celeste_audio_t));
    audio->filepath = filepath;
    audio->ended = 0;
    audio->loop = 0;

    if (get_file_extension(filepath))
        audio->sound = gau_load_sound_file(filepath, "ogg");
    else
        CELESTE_LOG_ERROR_PATH("Failed to load audio!", filepath);

    audio->handle = gau_create_handle_sound(celeste_get_instance()->aumixer, audio->sound, &setFlagAndDestroyOnFinish, &audio->ended, loop ? &audio->loop : 0);
    ga_handle_getParamf(audio->handle, GA_HANDLE_PARAM_GAIN, &audio->gain);
    ga_handle_getParamf(audio->handle, GA_HANDLE_PARAM_PITCH, &audio->pitch);
    ga_handle_getParamf(audio->handle, GA_HANDLE_PARAM_PAN, &audio->pan);
    return audio;
}

void celeste_audio_destroy(celeste_audio_t *audio)
{
    ga_sound_release(audio->sound);
    free(audio);
}

void celeste_audio_play(celeste_audio_t *audio)
{
    ga_handle_play(audio->handle);
}

void celeste_audio_stop(celeste_audio_t *audio)
{
    ga_handle_stop(audio->handle);
}

int celeste_audio_set_gain(celeste_audio_t *audio, float gain)
{
    if (ga_handle_setParamf(audio->handle, GA_HANDLE_PARAM_GAIN, gain))
    {
        audio->gain = gain;
        return CELESTE_OK;
    }
    return 1;
}

int celeste_audio_set_pitch(celeste_audio_t *audio, float pitch)
{
    if (ga_handle_setParamf(audio->handle, GA_HANDLE_PARAM_PITCH, pitch))
    {
        audio->pitch = pitch;
        return CELESTE_OK;
    }
    return 1;
}

int celeste_audio_set_pan(celeste_audio_t *audio, float pan)
{
    if (ga_handle_setParamf(audio->handle, GA_HANDLE_PARAM_PAN, pan))
    {
        audio->pan = pan;
        return CELESTE_OK;
    }
    return 1;
}

/* --- Static Helper Functions --- */
const char *get_file_extension(const char *filepath)
{
    size_t len = strlen(filepath);
    if (len >= 4) {
        if (strcmp(filepath + len - 4, ".ogg"))
            return "ogg";
        else if (strcmp(filepath + len - 4, ".wav"))
            return "wav";
    }

    return NULL;
}

void setFlagAndDestroyOnFinish(ga_Handle *in_handle, void *in_context)
{
    gc_int32* flag = (gc_int32*)(in_context);
    *flag = 1;
    ga_handle_destroy(in_handle);
}
