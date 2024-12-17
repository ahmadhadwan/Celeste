#include "internal/gapi.h"
#include "scene.h"
#include <string.h>

CLSTscene *clstScene()
{
    CLSTscene *scene = malloc(sizeof(CLSTscene));
    scene->layers_count = 0;
    scene->layers = malloc(0);
    scene->textures_count = 0;
    scene->textures = NULL;
    scene->fonts_count = 0;
    scene->fonts = NULL;
    return scene;
}

void clstSceneDestroy(CLSTscene *scene)
{
    for (int i = 0; i < scene->fonts_count; i++)
        clstFontDestroy(scene->fonts[i]);
    free(scene->fonts);

    for (int i = 0; i < scene->textures_count; i++)
        clstTextureDestroy(scene->textures[i]);
    free(scene->textures);
    
    for (int i = 0; i < scene->layers_count; i++)
        clstLayerDestroy(scene->layers[i]);
    free(scene->layers);

    free(scene);
}

void clstSceneAddLayer(CLSTscene *scene, CLSTlayer *layer)
{
    scene->layers = realloc(scene->layers, (scene->layers_count + 1) * sizeof(CLSTlayer *));
    scene->layers[scene->layers_count] = layer;
    scene->layers_count++;
}

void clstSceneAddTexture(CLSTscene *scene, CLSTtexture *texture)
{
    scene->textures = realloc(scene->textures, (scene->textures_count + 1) * sizeof(CLSTtexture *));
    scene->textures[scene->textures_count] = texture;
    scene->textures_count++;
}

CLSTtexture *clstSceneGetTexture(CLSTscene *scene, char *texture_name)
{
    for (int i = 0; i < scene->textures_count; i++) {
        if (strcmp((scene->textures[i])->name, texture_name) == 0)
            return scene->textures[i];
    }
    return NULL; // TODO: Return error texture
}

void clstSceneAddFont(CLSTscene *scene, CLSTfont *font)
{
    scene->fonts = realloc(scene->fonts, (scene->fonts_count + 1) * sizeof(CLSTtexture *));
    scene->fonts[scene->fonts_count] = font;
    scene->fonts_count++;
}

CLSTfont *clstSceneGetFont(CLSTscene *scene, char *font_name)
{
    for (int i = 0; i < scene->fonts_count; i++) {
        if (strcmp((scene->fonts[i])->name, font_name) == 0)
            return scene->fonts[i];
    }
    return NULL;
}

void clstSceneRender(CLSTscene *scene)
{
    for (int i = 0; i < scene->layers_count; i++)
        clstLayerRender(scene->layers[i]);
}
