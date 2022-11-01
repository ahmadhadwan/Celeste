#ifndef __CELESTE_INTERNAL_RENDERER_H__
#define __CELESTE_INTERNAL_RENDERER_H__

#include <cglm/vec2.h>
#include <cglm/vec3.h>
#include <cglm/mat4.h>
#include "gapi.h"

typedef struct {
    vec3         vertex;
    vec2         uv;
    float        tid;
    unsigned int color;
} CLSTvertexdata;

typedef struct {
    unsigned int     vao;
    unsigned int     vbo;
    CLSTindexbuffer *ibo;
    unsigned int     index_count;
    CLSTvertexdata  *buffer;
    unsigned int    *texture_slots;
    unsigned int     texture_slots_size;
    mat4            *transformation_back;
    unsigned int     transformation_back_size;
    float            projection_x;
    float            projection_y;
} CLSTrenderer;

#define RENDERER_MAX_SPRITES  (60000)
#define RENDERER_VERTEX_SIZE  (sizeof(CLSTvertexdata))
#define RENDERER_SPRITE_SIZE  (RENDERER_VERTEX_SIZE * 4)
#define RENDERER_BUFFER_SIZE  (RENDERER_SPRITE_SIZE * RENDERER_MAX_SPRITES)
#define RENDERER_INDICES_SIZE (RENDERER_MAX_SPRITES * 6)
#define RENDERER_MAX_TEXTURES (32)

#define SHADER_VERTEX_INDEX 0
#define SHADER_UV_INDEX		1
#define SHADER_TID_INDEX	2
#define SHADER_COLOR_INDEX	3

#endif /* __CELESTE_INTERNAL_RENDERER_H__ */
