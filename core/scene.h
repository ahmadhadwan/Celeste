#ifndef __CELESTE_SCENE_H__
#define __CELESTE_SCENE_H__

#include "layer.h"
#include "audio.h"

typedef struct {
    unsigned int   layers_count;
    CLSTlayer    **layers;

    uint32_t       textures_count;
    CLSTtexture  **textures;

    uint32_t       fonts_count;
    CLSTfont     **fonts;

    uint32_t       audio_count;
    CLSTaudio    **audio;
} CLSTscene;

/*
 * Creates a scene.
 *
 * returns a CLSTscene pointer which,
 * must be destroyed by clstSceneDestroy().
 */
CLSTscene *clstScene();

/*
 * Destroys the scene, and frees it's memory.
 */
void clstSceneDestroy(CLSTscene *scene);

/*
 * Adds a layer to the scene.
 */
void clstSceneAddLayer(CLSTscene *scene, CLSTlayer *layer);

/*
 * Adds a texture to the scene.
 */
void clstSceneAddTexture(CLSTscene *scene, CLSTtexture *texture);

/*
 * Gets a texture from a texture name.
 */
CLSTtexture *clstSceneGetTexture(CLSTscene *scene, char *texture_name);

/*
 * Adds a font to the scene.
 */
void clstSceneAddFont(CLSTscene *scene, CLSTfont *font);

/*
 * Gets a font from a font name.
 */
CLSTfont *clstSceneGetFont(CLSTscene *scene, char *font_name);

/*
 * Adds an audio to the scene.
 */
void clstSceneAddAudio(CLSTscene *scene, CLSTaudio *audio);

/*
 * Gets an audio from an audio name.
 */
CLSTaudio *clstSceneGetAudio(CLSTscene *scene, char *audio_name);

/*
 * Renders each sprite in each layer in the scene.
 */
void clstSceneRender(CLSTscene *scene);

#endif /* __CELESTE_SCENE_H__ */
