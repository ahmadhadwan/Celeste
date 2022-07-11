#ifndef __CELESTE_LAYER_H__
#define __CELESTE_LAYER_H__

#include <cglm/mat4.h>
#include "gapi.h"
#include "renderer.h"
#include "sprite.h"


typedef struct {
    celeste_renderer_t *renderer;
    celeste_shader_t   *shader;
    void              **sprites;
    unsigned int        sprites_size;
    struct {
        float left;
        float right;
        float bottom;
        float top;
#ifdef _WIN64
        /* near is a compiler macro on windows or something */
        float _near;
        float _far;
#else
        float near;
        float far;
#endif
    } projection;
    struct {
        double x;
        double y;
    } cursor;
} celeste_layer_t;

#ifdef _WIN64
celeste_layer_t *celeste_layer_create(float left, float right, float bottom, float top, float _near, float _far);
#else
celeste_layer_t *celeste_layer_create(float left, float right, float bottom, float top, float near, float far);
#endif
celeste_layer_t *celeste_layer_create_mat4(mat4 projection);
celeste_layer_t *celeste_layer_create_custom(celeste_renderer_t *renderer, celeste_shader_t *shader, mat4 projection);
void celeste_layer_destroy(celeste_layer_t *layer);

void celeste_layer_add_sprite(celeste_layer_t *layer, void *sprite);

void celeste_layer_render(celeste_layer_t *layer);

#endif /* __CELESTE_LAYER_H__ */
