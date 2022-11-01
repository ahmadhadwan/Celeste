#ifndef __CELESTE_SCENE_H__
#define __CELESTE_SCENE_H__

#include "layer.h"

typedef struct {
    unsigned int   layers_count;
    CLSTlayer    **layers;
} CLSTscene;

/*
 * Creates a scene.
 *
 * returns a CLSTscene pointer which,
 * must be destroyed by celeste_scene_destroy().
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
 * Renders each sprite in each layer in the scene.
 */
void clstSceneRender(CLSTscene *scene);

#endif /* __CELESTE_SCENE_H__ */
