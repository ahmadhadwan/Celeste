#ifndef __CELESTE_INTERNAL_GAPI_H__
#define __CELESTE_INTERNAL_GAPI_H__

#include <freetype-gl/freetype-gl.h>

typedef struct {
    uint32_t id;
    uint32_t component_count;
} CLSTbuffer;

typedef struct {
    uint32_t id;
    uint32_t count;
} CLSTindexbuffer;

typedef struct {
    uint32_t     id;
    uint32_t     buffers_count;
    CLSTbuffer **buffers;
} CLSTvertexarray;

typedef struct {
    uint32_t id;
} CLSTshader;

typedef struct {
    uint32_t id;
    uint32_t width;
    uint32_t height;
    char    *name;
} CLSTtexture;

typedef struct {
    float size;
    texture_atlas_t *ftatlas;
    texture_font_t  *ftfont;
    char            *name;
} CLSTfont;

typedef struct {
    uint32_t id;
    uint32_t rbo;
} CLSTframebuffer;

typedef struct {
    uint32_t id;
} CLSTrenderbuffer;

CLSTbuffer* clstBuffer(float* vertices, uint32_t count, uint32_t component_count);
void clstBufferDestroy(CLSTbuffer* buffer);

void clstBufferBind(CLSTbuffer* buffer);
void clstBufferUnbind();

CLSTindexbuffer* clstIndexBuffer(uint32_t* indices, uint32_t count);
void clstIndexBufferDestroy(CLSTindexbuffer* index_buffer);

void clstIndexBufferBind(CLSTindexbuffer* index_buffer);
void clstIndexBufferUnbind();

CLSTvertexarray* clstVertexArray();
void clstVertexArrayDestroy(CLSTvertexarray* vertex_array);

void clstVertexArrayAddBuffer(CLSTvertexarray* vertex_array, CLSTbuffer* buffer);
void clstVertexArrayLinkAttribute(CLSTvertexarray* vertex_array, uint32_t layout, int size, void* offset);

void clstVertexArrayBind(CLSTvertexarray* vertex_array);
void clstVertexArrayUnbind();

#endif /* __CELESTE_INTERNAL_GAPI_H__ */
