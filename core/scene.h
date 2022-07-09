#ifndef __CELESTE_SCENE_H__
#define __CELESTE_SCENE_H__

#include "layer.h"

typedef struct {
    unsigned int       layers_count;
    celeste_layer_t  **layers;
} celeste_scene_t;

celeste_scene_t *celeste_scene_create();
void celeste_scene_destroy(celeste_scene_t  *scene);

void celeste_scene_add_layer(celeste_scene_t *scene, celeste_layer_t *layer);
void celeste_scene_render(celeste_scene_t *scene);

#endif /* __CELESTE_SCENE_H__ */
