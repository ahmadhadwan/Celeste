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

CLSTlayer *clstLayer(float right, float top, char *name);

CLSTlayer *clstLayerMat4(mat4 projection, char *name);

CLSTlayer *clstLayerShader(float right, float top, CLSTshader *shader, char *name);

CLSTlayer *clstLayerCamera(CLSTcamera *camera, float right, float top, char *name);

CLSTlayer *clstLayerCameraMat4(CLSTcamera *camera, mat4 projection, char *name);

CLSTlayer *clstLayerCustom(CLSTrenderer *renderer, CLSTshader *shader, CLSTcamera *camera, mat4 projection, char *name);
void clstLayerDestroy(CLSTlayer *layer);

void clstLayerAddRenderable(CLSTlayer *layer, void *renderable);
void clstLayerRemoveRenderable(CLSTlayer *layer, char *name);

void *clstLayerGetRenderable(CLSTlayer *layer, char *name);
void clstLayerRender(CLSTlayer *layer);

#endif /* __CELESTE_LAYER_H__ */
