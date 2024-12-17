#ifndef __CELESTE_INTERNAL_LAYER_H__
#define __CELESTE_INTERNAL_LAYER_H__

#include "../camera.h"
#include "gapi.h"
#include "renderer.h"
#include "../sprite.h"
#include "../list.h"

typedef struct {
    CLSTrenderer *renderer;
    CLSTshader   *shader;
    CLSTlist     *sprites;
    CLSTcamera   *camera;
    char         *name;
    struct {
        float right;
        float top;
    } projection;
} CLSTlayer;

#endif /* __CELESTE_INTERNAL_LAYER_H__ */
