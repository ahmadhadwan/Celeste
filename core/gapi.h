#ifndef __CELESTE_GAPI_H__
#define __CELESTE_GAPI_H__

#include <cglm/vec2.h>
#include <cglm/vec3.h>
#include <cglm/vec4.h>
#include <cglm/mat4.h>
#include <freetype-gl/freetype-gl.h>

typedef struct {
    unsigned int id;
    unsigned int component_count;
} buffer_t;

typedef struct {
    unsigned int id;
    unsigned int count;
} index_buffer_t;

typedef struct {
    unsigned int id;
    buffer_t** buffers;
    unsigned int buffers_count;
} vertex_array_t;

typedef struct {
    unsigned int id;
} celeste_shader_t;

typedef struct {
    unsigned int id;
    unsigned int width;
    unsigned int height;
} celeste_texture_t;

typedef struct {
    unsigned int size;
    texture_atlas_t* ftatlas;
    texture_font_t* ftfont;
} celeste_font_t;

buffer_t* celeste_buffer_create(float* vertices, unsigned int count, unsigned int component_count);
void celeste_buffer_destroy(buffer_t* buffer);

void celeste_buffer_bind(buffer_t* buffer);
void celeste_buffer_unbind();

index_buffer_t* celeste_index_buffer_create(unsigned int* indices, unsigned int count);
void celeste_index_buffer_destroy(index_buffer_t* index_buffer);

void celeste_index_buffer_bind(index_buffer_t* index_buffer);
void celeste_index_buffer_unbind();

vertex_array_t* celeste_vertex_array_create();
void celeste_vertex_array_destroy(vertex_array_t* vertex_array);

void celeste_vertex_array_add_buffer(vertex_array_t* vertex_array, buffer_t* buffer);
void celeste_vertex_array_link_attribute(vertex_array_t* vertex_array, unsigned int layout, int size, void* offset);

void celeste_vertex_array_bind(vertex_array_t* vertex_array);
void celeste_vertex_array_unbind();

celeste_shader_t* celeste_shader_create(char* filepath);
celeste_shader_t* celeste_shader_create_const_src(const char* src);
celeste_shader_t* celeste_shader_create_src(char* src);
celeste_shader_t *celeste_shader_default();
void celeste_shader_destroy(celeste_shader_t* shader);

void celeste_shader_activate(celeste_shader_t* shader);
void celeste_shader_deactivate();

void celeste_shader_uniform_int(celeste_shader_t* shader, char* uniform_name, int value);
void celeste_shader_uniform_float(celeste_shader_t* shader, char* uniform_name, float value);
void celeste_shader_uniform_vec2(celeste_shader_t* shader, char* uniform_name, vec2 value);
void celeste_shader_uniform_vec3(celeste_shader_t* shader, char* uniform_name, vec3 value);
void celeste_shader_uniform_vec4(celeste_shader_t* shader, char* uniform_name, vec4 value);
void celeste_shader_uniform_mat4(celeste_shader_t* shader, char* uniform_name, mat4 value);

celeste_texture_t* celeste_texture_create(char* filepath);
void celeste_texture_destroy(celeste_texture_t* texture);

void celeste_texture_bind(celeste_texture_t* texture);
void celeste_texture_unbind();

celeste_font_t* celeste_font_create(const char* filepath, unsigned int size);
void celeste_font_destroy(celeste_font_t* font);

#endif /* __CELESTE_GAPI_H__ */
