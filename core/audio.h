#ifndef __CELESTE_AUDIO_H__
#define __CELESTE_AUDIO_H__

#include "common.h"

#ifndef __CELESTE_INTERNAL_AUDIO_H__
    typedef struct CLSTaudio CLSTaudio;
    typedef struct CLSTaudioplayer CLSTaudioplayer;
#endif /* __CELESTE_INTERNAL_AUDIO_H__ */

/*
 * Opens the audio file from the filepath,
 * and creates a CLSTaudio.
 * Supports WAV and OGG audio formats.
 *
 * On success returns a CLSTaudio pointer,
 * which must be destroyed by clstAudioDestroy().
 *
 * On error it returns NULL.
 */
CLSTaudio *clstAudio(const char *filepath, char *name);

/*
 * Creates a CLSTaudio from memory.
 * Supports OGG audio format.
 *
 * On success returns a CLSTaudio pointer,
 * which must be destroyed by clstAudioDestroy().
 *
 * On error it returns NULL.
 */
CLSTaudio *clstAudioMem(uint8_t *buffer, uint32_t bufsize, char *name);

/*
 * Destroys a CLSTaudio and frees its memory.
 *
 * Shouldn't be destroyed before an audio player
 * using this audio is destroyed.
 */
void clstAudioDestroy(CLSTaudio *audio);

/*
 * Creates an audio player for an audio.
 *
 * loop arg, and destroy_on_finish arg are booleans.
 *
 * On success returns a CLSTaudioplayer pointer,
 * which must be destroyed by clstAudioPlayerDestroy().
 *
 * On error it returns NULL.
 */
CLSTaudioplayer *clstAudioPlayer(CLSTaudio *audio, uint8_t loop, uint8_t destroy_on_finish);

/*
 * Destroys a CLSTaudioplayer and frees its memory.
 */
void clstAudioPlayerDestroy(CLSTaudioplayer *player);

/*
 * Play the audio player.
 *
 * On success returns CELESTE_OK.
 */
CLSTresult clstAudioPlayerPlay(CLSTaudioplayer *player);

/*
 * Stop the audio player.
 *
 * On success returns CELESTE_OK.
 */
CLSTresult  clstAudioPlayerStop(CLSTaudioplayer  *player);

/*
 * Returns the gain of the audio player.
 */
float clstAudioPlayerGetGain(CLSTaudioplayer *player);

/*
 * Sets the audio player's gain.
 *
 * On success returns CELESTE_OK.
 */
CLSTresult clstAudioPlayerSetGain(CLSTaudioplayer *player, float gain);

/*
 * Returns the pitch of the audio player.
 */
float clstAudioPlayerGetPitch(CLSTaudioplayer *player);

/*
 * Sets the audio player's pitch.
 *
 * On success returns CELESTE_OK.
 */
CLSTresult clstAudioPlayerSetPitch(CLSTaudioplayer *player, float pitch);

/*
 * Returns the pan of the audio player.
 */
float clstAudioPlayerGetPan(CLSTaudioplayer *player);

/*
 * Sets the audio player's pan.
 * pan < 0 (audio source is from the left)
 * pan > 0 (audio source is from the right)
 *
 * On success returns CELESTE_OK.
 */
CLSTresult clstAudioPlayerSetPan(CLSTaudioplayer *player, float pan);

#endif /* __CELESTE_AUDIO_H__ */
