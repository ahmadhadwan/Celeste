#include "scene.h"

CLSTscene *clstScene()
{
    CLSTscene *scene = malloc(sizeof(CLSTscene));
    scene->layers_count = 0;
    scene->layers = malloc(0);
    return scene;
}

void clstSceneDestroy(CLSTscene *scene)
{
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

void clstSceneRender(CLSTscene *scene)
{
    for (int i = 0; i < scene->layers_count; i++)
        clstLayerRender(scene->layers[i]);
}
