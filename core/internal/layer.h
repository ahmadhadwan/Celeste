#ifndef __CELESTE_INTERNAL_LAYER_H__
#define __CELESTE_INTERNAL_LAYER_H__

#include "../camera.h"
#include "gapi.h"
#include "renderer.h"
#include "../sprite.h"

typedef struct {
    CLSTrenderer *renderer;
    CLSTshader   *shader;
    void        **sprites;
    unsigned int  sprites_size;
    CLSTcamera   *camera;
    struct {
        float right;
        float top;
    } projection;
} CLSTlayer;

#endif /* __CELESTE_INTERNAL_LAYER_H__ */
