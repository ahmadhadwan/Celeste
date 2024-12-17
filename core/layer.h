#ifndef __CELESTE_LAYER_H__
#define __CELESTE_LAYER_H__

#include "camera.h"
#include <cglm/mat4.h>
#include "gapi.h"
#include "renderer.h"
#include "sprite.h"

#ifndef __CELESTE_INTERNAL_LAYER_H__
    typedef struct CLSTlayer CLSTlayer;
#endif /* __CELESTE_INTERNAL_LAYER_H__ */

/*
 * Creates a layer with a projection ortho matrix
 * created using the passed args.
 *
 * returns CLSTlayer pointer,
 * which must be destroyed by clstLayerDestroy().
 */
CLSTlayer *clstLayer(float right, float top, char *name);

/*
 * Creates a layer with the arg
 * as the projection matrix.
 *
 * returns CLSTlayer pointer,
 * which must be destroyed by clstLayerDestroy().
 */
CLSTlayer *clstLayerMat4(mat4 projection, char *name);

/*
 * Creates a layer with a projection ortho matrix
 * created using the passed args, and a shader.
 *
 * returns CLSTlayer pointer,
 * which must be destroyed by clstLayerDestroy().
 */
CLSTlayer *clstLayerShader(float right, float top, CLSTshader *shader, char *name);

/*
 * Creates a layer with a camera, and projection
 * ortho matrix created using the passed args.
 *
 * returns CLSTlayer pointer,
 * which must be destroyed by clstLayerDestroy().
 */
CLSTlayer *clstLayerCamera(CLSTcamera *camera, float right, float top, char *name);

/*
 * Creates a layer with a camera, and with
 * the arg as the projection matrix.
 *
 * returns CLSTlayer pointer,
 * which must be destroyed by clstLayerDestroy().
 */
CLSTlayer *clstLayerCameraMat4(CLSTcamera *camera, mat4 projection, char *name);

/*
 * Creates a layer with a custom renderer,
 * shader, camera, and a projection matrix.
 *
 * returns CLSTlayer pointer,
 * which must be destroyed by clstLayerDestroy().
 */
CLSTlayer *clstLayerCustom(CLSTrenderer *renderer, CLSTshader *shader, CLSTcamera *camera, mat4 projection, char *name);

/*
 * Destroys the layer, and frees its memory.
 */
void clstLayerDestroy(CLSTlayer *layer);

/*
 * Adds a sprite to the layer.
 */
void clstLayerAddSprite(CLSTlayer *layer, void *sprite);

/*
 * Renders the sprites in the layer.
 */
void clstLayerRender(CLSTlayer *layer);

#endif /* __CELESTE_LAYER_H__ */
