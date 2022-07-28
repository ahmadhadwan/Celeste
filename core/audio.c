#include "audio.h"
#include "celeste.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *get_file_extension(const char *filepath);
static void setFlagAndDestroyOnFinish(ga_Handle *in_handle, void *in_context);

celeste_audio_t *celeste_audio_create(const char *filepath)
{
    celeste_audio_t *audio;
    const char *file_ext;

    audio = malloc(sizeof(celeste_audio_t));
    audio->filepath = (const char *)strdup(filepath);

    file_ext = get_file_extension(filepath);
    if (file_ext == NULL)
    {
        CELESTE_LOG_ERROR_PATH("Failed to load audio!", filepath);
        exit(1);
    }

    audio->sound = gau_load_sound_file(filepath, "ogg");
    return audio;
}

void celeste_audio_destroy(celeste_audio_t *audio)
{
    ga_sound_release(audio->sound);
    free((void *)audio->filepath);
    free(audio);
}

celeste_audio_player_t *celeste_audio_player_create(celeste_audio_t *audio, int loop)
{
    celeste_audio_player_t *player;

    player = malloc(sizeof(celeste_audio_player_t));
    player->ended = 0;
    player->handle = gau_create_handle_sound(
            celeste_get_instance()->aumixer, audio->sound,
            &setFlagAndDestroyOnFinish, &player->ended,
            loop ? &player->loop : NULL
    );

    ga_handle_getParamf(player->handle, GA_HANDLE_PARAM_GAIN,  &player->gain);
    ga_handle_getParamf(player->handle, GA_HANDLE_PARAM_PITCH, &player->pitch);
    ga_handle_getParamf(player->handle, GA_HANDLE_PARAM_PAN,   &player->pan);
    return player;
}

void celeste_audio_player_destroy(celeste_audio_player_t *player)
{
    ga_handle_destroy(player->handle);
    free(player);
}

int celeste_audio_player_play(celeste_audio_player_t *player)
{
    ga_handle_play(player->handle);
    return CELESTE_OK;
}

int celeste_audio_player_stop(celeste_audio_player_t *player)
{
    ga_handle_stop(player->handle);
    return CELESTE_OK;
}

int celeste_audio_player_set_gain(celeste_audio_player_t *player, float gain)
{
    if (!ga_handle_setParamf(player->handle, GA_HANDLE_PARAM_GAIN, gain))
        return 1;

    player->gain = gain;
    return CELESTE_OK;
}

int celeste_audio_player_set_pitch(celeste_audio_player_t *player, float pitch)
{
    if (!ga_handle_setParamf(player->handle, GA_HANDLE_PARAM_PITCH, pitch))
        return 1;

    player->pitch = pitch;
    return CELESTE_OK;
}

int celeste_audio_player_set_pan(celeste_audio_player_t *player, float pan)
{
    if (!ga_handle_setParamf(player->handle, GA_HANDLE_PARAM_PAN, pan))
        return 1;

    player->pan = pan;
    return CELESTE_OK;
}

const char *get_file_extension(const char *filepath)
{
    size_t len;

    len = strlen(filepath);
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
    *((gc_int32*)in_context) = 1;
}
