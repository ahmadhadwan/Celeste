#ifndef __CELESTE_SCENE_H__
#define __CELESTE_SCENE_H__

#include "layer.h"

typedef struct {
    unsigned int   layers_count;
    CLSTlayer    **layers;

    uint32_t       textures_count;
    CLSTtexture  **textures;
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
 * Renders each sprite in each layer in the scene.
 */
void clstSceneRender(CLSTscene *scene);

#endif /* __CELESTE_SCENE_H__ */
