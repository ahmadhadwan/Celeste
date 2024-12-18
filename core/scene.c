#include "internal/gapi.h"
#include "internal/audio.h"
#include "internal/layer.h"
#include "scene.h"
#include <string.h>
#include <stdio.h>

CLSTscene *clstScene()
{
    CLSTscene *scene;
    scene = malloc(sizeof(CLSTscene));
    scene->layers   = clstListCreate();
    scene->textures = clstListCreate();
    scene->fonts    = clstListCreate();
    scene->audio    = clstListCreate();
    scene->bodies   = clstListCreate();
    return scene;
}

void clstSceneDestroy(CLSTscene *scene)
{
    clstListDestroy(scene->bodies,   (CLSTitemdestroy)clstBodyDestroy);
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

CLSTlayer *clstSceneGetLayer(CLSTscene *scene, char *layer_name)
{
    CLSTlayer **layers;

    layers = (CLSTlayer **)scene->layers->items;
    for (int i = 0; i < scene->layers->count; i++) {
        if (strcmp(layers[i]->name, layer_name) == 0)
            return layers[i];
    }
    return NULL;
}

CLSTlayer *clstSceneGetLastLayer(CLSTscene *scene)
{
    if (scene->layers->count == 0) {
        CELESTE_ASSERT(scene->layers->count != 0 && "No layer available to fetch from the scene!");
        return NULL;
    }
    
    return ((CLSTlayer **)scene->layers->items)[scene->layers->count - 1];
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

void clstSceneAddBody(CLSTscene *scene, CLSTbody *body)
{
    clstListAdd(scene->bodies, body);
}

void clstSceneRender(CLSTscene *scene)
{
    CLSTlayer **layers;

    layers = (CLSTlayer **)scene->layers->items;
    for (int i = 0; i < scene->layers->count; i++)
        clstLayerRender(layers[i]);
}
