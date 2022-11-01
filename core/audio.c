#include "internal/audio.h"

#include "audio.h"
#include "celeste.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *file_ext(const char *filepath);
static void destroy_on_finish(ga_Handle *in_handle, void *in_context);

CLSTaudio *clstAudio(const char *filepath)
{
    CLSTaudio *audio;
    const char *ext;
    ga_Sound *sound;

    ext = file_ext(filepath);
    if (ext == NULL)
    {
        CELESTE_LOG_ERROR_PATH("Unsupported audio format!", filepath);
        return NULL;
    }

    sound = gau_load_sound_file(filepath, ext);
    if (sound == NULL)
    {
        CELESTE_LOG_ERROR_PATH("Failed to load audio file!", filepath);
        return NULL;
    }

    audio = malloc(sizeof(CLSTaudio));
    audio->type = CELESTE_AUDIO_TYPE_FILE;
    audio->sound = sound;
    return audio;
}

CLSTaudio *clstAudioMem(unsigned char *buffer, unsigned int bufsize)
{
    CLSTaudio *audio;
    ga_Memory *mem;

    mem = ga_memory_create(buffer, bufsize);
    if (mem == NULL)
    {
        CELESTE_LOG_ERROR("Failed to load audio from memory!");
        return NULL;
    }

    audio = malloc(sizeof(CLSTaudio));
    audio->type = CELESTE_AUDIO_TYPE_MEMORY;
    audio->memory = mem;
    return audio;
}

void clstAudioDestroy(CLSTaudio *audio)
{
    if (audio->type == CELESTE_AUDIO_TYPE_MEMORY)
        ga_memory_release(audio->memory);
    else
        ga_sound_release(audio->sound);
    free(audio);
}

CLSTaudioplayer *clstAudioPlayer(CLSTaudio *audio, int loop, int _destroy_on_finish)
{
    CLSTaudioplayer *player;
    ga_Handle *handle;

    player = malloc(sizeof(CLSTaudioplayer));
    if (audio->type == CELESTE_AUDIO_TYPE_MEMORY)
        handle = gau_create_handle_memory(clstInstance()->aumixer, audio->memory, "ogg",
                _destroy_on_finish ? &destroy_on_finish : NULL, player, loop ? &player->loop : NULL
        );
    else
        handle = gau_create_handle_sound(clstInstance()->aumixer, audio->sound,
                _destroy_on_finish ? &destroy_on_finish : NULL, player, loop ? &player->loop : NULL
        );
    if (handle == NULL)
    {
        CELESTE_LOG_ERROR("Failed to create an audio player!");
        free(player);
        return NULL;
    }

    player->handle = handle;
    return player;
}

void clstAudioPlayerDestroy(CLSTaudioplayer *player)
{
    ga_handle_destroy(player->handle);
    free(player);
}

CLSTresult clstAudioPlayerPlay(CLSTaudioplayer *player)
{
    return ga_handle_play(player->handle) == GC_SUCCESS ? CELESTE_OK : !CELESTE_OK;
}

CLSTresult clstAudioPlayerStop(CLSTaudioplayer *player)
{
    return ga_handle_stop(player->handle) == GC_SUCCESS ? CELESTE_OK : !CELESTE_OK;
}

float clstAudioPlayerGetGain(CLSTaudioplayer *player)
{
    float gain;
    ga_handle_getParamf(player->handle, GA_HANDLE_PARAM_GAIN, &gain);
    return gain;
}

CLSTresult clstAudioPlayerSetGain(CLSTaudioplayer *player, float gain)
{
    return ga_handle_setParamf(player->handle, GA_HANDLE_PARAM_GAIN, gain) == GC_SUCCESS ? CELESTE_OK : !CELESTE_OK;
}

float clstAudioPlayerGetPitch(CLSTaudioplayer *player)
{
    float pitch;
    ga_handle_getParamf(player->handle, GA_HANDLE_PARAM_PITCH, &pitch);
    return pitch;
}

CLSTresult clstAudioPlayerSetPitch(CLSTaudioplayer *player, float pitch)
{
    return ga_handle_setParamf(player->handle, GA_HANDLE_PARAM_PITCH, pitch) == GC_SUCCESS ? CELESTE_OK : !CELESTE_OK;
}

float clstAudioPlayerGetPan(CLSTaudioplayer *player)
{
    float pan;
    ga_handle_getParamf(player->handle, GA_HANDLE_PARAM_PAN, &pan);
    return pan;
}

CLSTresult clstAudioPlayerSetPan(CLSTaudioplayer *player, float pan)
{
    return ga_handle_setParamf(player->handle, GA_HANDLE_PARAM_PAN, pan) == GC_SUCCESS ? CELESTE_OK : !CELESTE_OK;
}

const char *file_ext(const char *filepath)
{
    size_t len;

    len = strlen(filepath);
    if (len >= 4) {
        if (strcmp(filepath + len - 4, ".ogg") == 0)
            return "ogg";
        else if (strcmp(filepath + len - 4, ".wav") == 0)
            return "wav";
    }

    return NULL;
}

void destroy_on_finish(ga_Handle *in_handle, void *in_context)
{
    clstAudioPlayerDestroy((CLSTaudioplayer *)in_context);
}
