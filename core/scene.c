#include "scene.h"

CLSTscene *clstScene()
{
    CLSTscene *scene = malloc(sizeof(CLSTscene));
    scene->layers_count = 0;
    scene->layers = malloc(0);
    scene->textures_count = 0;
    scene->textures = NULL;
    scene->textures_names = NULL;
    return scene;
}

void clstSceneDestroy(CLSTscene *scene)
{
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

void clstSceneAddTexture(CLSTscene *scene, CLSTtexture *texture, char *name)
{
    scene->textures = realloc(scene->textures, (scene->textures_count + 1) * sizeof(CLSTtexture *));
    scene->textures[scene->textures_count] = texture;
    
    scene->textures_names = realloc(scene->textures_names, (scene->textures_count + 1) * sizeof(char *));
    scene->textures_names[scene->textures_count] = name;
    scene->textures_count++;
}

void clstSceneRender(CLSTscene *scene)
{
    for (int i = 0; i < scene->layers_count; i++)
        clstLayerRender(scene->layers[i]);
}
