#ifndef __CELESTE_INTERNAL_RENDERER_H__
#define __CELESTE_INTERNAL_RENDERER_H__

#include <cglm/vec2.h>
#include <cglm/vec3.h>
#include <cglm/mat4.h>
#include "gapi.h"

typedef struct {
    vec3     vertex;
    vec2     uv;
    float    tid;
    uint32_t color;
} CLSTvertexdata;

typedef struct {
    uint32_t         vao;
    uint32_t         vbo;
    CLSTindexbuffer *ibo;
    uint32_t         index_count;
    CLSTvertexdata  *buffer;
    uint32_t        *texture_slots;
    uint32_t         texture_slots_size;
    mat4            *transformation_back;
    uint32_t         transformation_back_size;
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
