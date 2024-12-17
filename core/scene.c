#include "internal/gapi.h"
#include "internal/audio.h"
#include "scene.h"
#include <string.h>
#include <stdio.h>

CLSTscene *clstScene()
{
    CLSTscene *scene;
    scene = malloc(sizeof(CLSTscene));
    scene->layers = clstListCreate();
    scene->textures = clstListCreate();
    scene->fonts = clstListCreate();
    scene->audio = clstListCreate();
    return scene;
}

void clstSceneDestroy(CLSTscene *scene)
{
    clstListDestroy(scene->audio,    (CLSTitemdestroy)clstAudioDestroy);
    clstListDestroy(scene->fonts,    (CLSTitemdestroy)clstFontDestroy);
    clstListDestroy(scene->textures, (CLSTitemdestroy)clstTextureDestroy);
    clstListDestroy(scene->layers,   (CLSTitemdestroy)clstLayerDestroy);
    free(scene);
}

void clstSceneAddLayer(CLSTscene *scene, CLSTlayer *layer)
{
    clstListAdd(scene->layers, layer);
}

CLSTlayer *clstSceneGetLayer(CLSTscene *scene)
{
    if (scene->layers->count == 0)
        return NULL;
    return scene->layers->items[0];
}

void clstSceneAddTexture(CLSTscene *scene, CLSTtexture *texture)
{
    clstListAdd(scene->textures, texture);
}

CLSTtexture *clstSceneGetTexture(CLSTscene *scene, char *texture_name)
{
    CLSTtexture **textures;

    textures = (CLSTtexture **)scene->textures->items;
    for (int i = 0; i < scene->textures->count; i++) {
        if (strcmp(textures[i]->name, texture_name) == 0)
            return textures[i];
    }
    return NULL; // TODO: Return error texture
}

void clstSceneAddFont(CLSTscene *scene, CLSTfont *font)
{
    clstListAdd(scene->fonts, font);
}

CLSTfont *clstSceneGetFont(CLSTscene *scene, char *font_name)
{
    CLSTfont **fonts;

    fonts = (CLSTfont **)scene->fonts->items;
    for (int i = 0; i < scene->fonts->count; i++) {
        if (strcmp(fonts[i]->name, font_name) == 0)
            return fonts[i];
    }
    return NULL;
}

void clstSceneAddAudio(CLSTscene *scene, CLSTaudio *audio)
{
    clstListAdd(scene->audio, audio);
}

CLSTaudio *clstSceneGetAudio(CLSTscene *scene, char *audio_name)
{
    CLSTaudio **audio;

    audio = (CLSTaudio **)scene->audio->items;
    for (int i = 0; i < scene->audio->count; i++) {
        if (strcmp(audio[i]->name, audio_name) == 0)
            return audio[i];
    }
    return NULL;
}

void clstSceneRender(CLSTscene *scene)
{
    CLSTlayer **layers;

    layers = (CLSTlayer **)scene->layers->items;
    for (int i = 0; i < scene->layers->count; i++)
        clstLayerRender(layers[i]);
}
