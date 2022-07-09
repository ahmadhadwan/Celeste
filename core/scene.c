#include "scene.h"

celeste_scene_t *celeste_scene_create()
{
    celeste_scene_t *scene = malloc(sizeof(celeste_scene_t));
    scene->layers_count = 0;
    scene->layers = malloc(0);
    return scene;
}

void celeste_scene_destroy(celeste_scene_t *scene)
{
    for (int i = 0; i < scene->layers_count; i++)
        celeste_layer_destroy(scene->layers[i]);

    free(scene->layers);
    free(scene);
}

void celeste_scene_add_layer(celeste_scene_t *scene, celeste_layer_t *layer)
{
    scene->layers = realloc(scene->layers, (scene->layers_count + 1) * sizeof(celeste_layer_t*));
    scene->layers[scene->layers_count] = layer;
    scene->layers_count++;
}

void celeste_scene_render(celeste_scene_t *scene)
{
    for (int i = 0; i < scene->layers_count; i++)
        celeste_layer_render(scene->layers[i]);
}
