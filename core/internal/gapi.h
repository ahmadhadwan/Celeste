#ifndef __CELESTE_INTERNAL_GAPI_H__
#define __CELESTE_INTERNAL_GAPI_H__

#include <freetype-gl/freetype-gl.h>

typedef struct {
    unsigned int id;
    unsigned int component_count;
} CLSTbuffer;

typedef struct {
    unsigned int id;
    unsigned int count;
} CLSTindexbuffer;

typedef struct {
    unsigned int id;
    unsigned int buffers_count;
    CLSTbuffer **buffers;
} CLSTvertexarray;

typedef struct {
    unsigned int id;
} CLSTshader;

typedef struct {
    unsigned int id;
    unsigned int width;
    unsigned int height;
    char        *name;
} CLSTtexture;

typedef struct {
    float size;
    texture_atlas_t *ftatlas;
    texture_font_t  *ftfont;
} CLSTfont;

typedef struct {
    unsigned int id;
    unsigned int rbo;
} CLSTframebuffer;

typedef struct {
    unsigned int id;
} CLSTrenderbuffer;

CLSTbuffer* clstBuffer(float* vertices, unsigned int count, unsigned int component_count);
void clstBufferDestroy(CLSTbuffer* buffer);

void clstBufferBind(CLSTbuffer* buffer);
void clstBufferUnbind();

CLSTindexbuffer* clstIndexBuffer(unsigned int* indices, unsigned int count);
void clstIndexBufferDestroy(CLSTindexbuffer* index_buffer);

void clstIndexBufferBind(CLSTindexbuffer* index_buffer);
void clstIndexBufferUnbind();

CLSTvertexarray* clstVertexArray();
void clstVertexArrayDestroy(CLSTvertexarray* vertex_array);

void clstVertexArrayAddBuffer(CLSTvertexarray* vertex_array, CLSTbuffer* buffer);
void clstVertexArrayLinkAttribute(CLSTvertexarray* vertex_array, unsigned int layout, int size, void* offset);

void clstVertexArrayBind(CLSTvertexarray* vertex_array);
void clstVertexArrayUnbind();

#endif /* __CELESTE_INTERNAL_GAPI_H__ */
