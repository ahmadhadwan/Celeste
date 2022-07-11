#ifndef __CELESTE_RENDERER_H__
#define __CELESTE_RENDERER_H__

#include <cglm/vec2.h>
#include <cglm/vec3.h>
#include <cglm/mat4.h>
#include "gapi.h"
#include "sprite.h"

typedef struct {
    vec3         vertex;
    vec2         uv;
    int          tid;
    unsigned int color;
} vertex_data_t;

typedef struct {
    unsigned int    vao;
    unsigned int    vbo;
    index_buffer_t *ibo;
    unsigned int    index_count;
    vertex_data_t  *buffer;
    unsigned int   *texture_slots;
    unsigned int    texture_slots_size;
    mat4           *transformation_back;
    unsigned int    transformation_back_size;
    float           projection_x;
    float           projection_y;
} celeste_renderer_t;

#define RENDERER_MAX_SPRITES  (60000)
#define RENDERER_VERTEX_SIZE  (sizeof(vertex_data_t))
#define RENDERER_SPRITE_SIZE  (RENDERER_VERTEX_SIZE * 4)
#define RENDERER_BUFFER_SIZE  (RENDERER_SPRITE_SIZE * RENDERER_MAX_SPRITES)
#define RENDERER_INDICES_SIZE (RENDERER_MAX_SPRITES * 6)
#define RENDERER_MAX_TEXTURES (32)

#define SHADER_VERTEX_INDEX 0
#define SHADER_UV_INDEX		1
#define SHADER_TID_INDEX	2
#define SHADER_COLOR_INDEX	3

celeste_renderer_t *celeste_renderer_create();
void celeste_renderer_destroy(celeste_renderer_t *renderer);

celeste_renderer_t *celeste_renderer_default();

void celeste_renderer_push(celeste_renderer_t *renderer, mat4 transformation);
void celeste_renderer_pop(celeste_renderer_t *renderer);

void celeste_renderer_begin(celeste_renderer_t *renderer);
void celeste_renderer_submit_sprite(celeste_renderer_t *renderer, celeste_sprite_t *sprite);
void celeste_renderer_submit_label(celeste_renderer_t *renderer, celeste_label_t *label);
void celeste_renderer_end();
void celeste_renderer_flush(celeste_renderer_t *renderer);

#endif /* __CELESTE_RENDERER_H__ */
