#include "gapi.h"

#include <celeste/celeste.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <stb/stb_image.h>
#include <sys/stat.h>

buffer_t* celeste_buffer_create(float* vertices, unsigned int count, unsigned int component_count)
{
    buffer_t* buffer = malloc(sizeof(buffer_t));

    GLuint id;
    glGenBuffers(1, &id);

    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    buffer->id = id;
    buffer->component_count = component_count;
    return buffer;
}

void celeste_buffer_destroy(buffer_t* buffer)
{
    glDeleteBuffers(1, &(buffer->id));
    free(buffer);
}

void celeste_buffer_bind(buffer_t* buffer)
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer->id);
}

void celeste_buffer_unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

index_buffer_t* celeste_index_buffer_create(unsigned int* indices, unsigned int count)
{
    index_buffer_t* index_buffer = malloc(sizeof(index_buffer_t));

    GLuint id;
    glGenBuffers(1, &id);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    index_buffer->id = id;
    index_buffer->count = count;
    return index_buffer;
}

void celeste_index_buffer_destroy(index_buffer_t* index_buffer)
{
    glDeleteBuffers(1, &(index_buffer->id));
    free(index_buffer);
}

void celeste_index_buffer_bind(index_buffer_t* index_buffer)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer->id);
}

void celeste_index_buffer_unbind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

vertex_array_t* celeste_vertex_array_create()
{
    vertex_array_t* vertex_array = malloc(sizeof(vertex_array_t));

    GLuint id;
    glGenVertexArrays(1, &id);

    vertex_array->id = id;
    vertex_array->buffers = NULL;
    vertex_array->buffers_count = 0;
    return vertex_array;
}

void celeste_vertex_array_destroy(vertex_array_t* vertex_array)
{
    for (unsigned int i = 0; i < vertex_array->buffers_count; i++)
        celeste_buffer_destroy(vertex_array->buffers[i]);

    glDeleteVertexArrays(1, &(vertex_array->id));
    if (vertex_array->buffers_count)
        free(vertex_array->buffers);
    free(vertex_array);
}

void celeste_vertex_array_add_buffer(vertex_array_t* vertex_array, buffer_t* buffer)
{
    vertex_array->buffers = realloc(vertex_array->buffers, vertex_array->buffers_count + 1);
    vertex_array->buffers[vertex_array->buffers_count] = buffer;
    vertex_array->buffers_count++;
}

void celeste_vertex_array_link_attribute(vertex_array_t* vertex_array, unsigned int layout, int size, void* offset)
{
    buffer_t* buffer = vertex_array->buffers[vertex_array->buffers_count - 1];
    glBindVertexArray(vertex_array->id);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->id);

    glEnableVertexAttribArray(layout);
    glVertexAttribPointer(layout, size, GL_FLOAT, GL_FALSE, buffer->component_count * sizeof(float), offset);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void celeste_vertex_array_bind(vertex_array_t* vertex_array)
{
    glBindVertexArray(vertex_array->id);
}

void celeste_vertex_array_unbind()
{
    glBindVertexArray(0);
}

celeste_shader_t* celeste_shader_create(char* filepath)
{
    struct stat s;
    FILE *fd;
    char *src;
    celeste_shader_t * shader;

    if (stat(filepath, &s))
    {
        CELESTE_LOG_ERROR_PATH("Failed to stat shader file!", filepath);
        return NULL;
    }

    src = malloc(s.st_size + 1);
    fd = fopen(filepath, "r");

    fread(src, sizeof(char), s.st_size, fd);
    fclose(fd);
    src[s.st_size] = '\0';

    shader = celeste_shader_create_src(src);
    free(src);
    return shader;
}

celeste_shader_t* celeste_shader_create_const_src(const char* constsrc)
{
    celeste_shader_t *shader;
    char *src;

    src = strdup(constsrc);
    shader = celeste_shader_create_src(src);

    free(src);
    return shader;
}

celeste_shader_t* celeste_shader_create_src(char* src)
{
    char* vertex_src = NULL;
    char* fragment_src = NULL;

    for (int i = 0; src[i]; i++) {
        if (src[i] == '#') {
            int start_i = i;
            while (src[i] != ' ') i++;
            if (strncmp(src + start_i, "#type", 5) == 0) src[start_i] = '\0';
            else continue;
            while (src[i] == ' ') i++;
            start_i = i;
            while (src[i] != '\n') i++;
            i++;
            if (strncmp(src + start_i, "vertex", 6) == 0) {
                vertex_src = src + i;
            } else if (strncmp(src + start_i, "fragment", 8) == 0) {
                fragment_src = src + i;
            } else {
                CELESTE_LOG_ERROR("Shader type unknown!");
                exit(1);
            }
        }
    }

    if (!(vertex_src && fragment_src)) {
        CELESTE_LOG_ERROR("Vertex and Fragment shaders were not found!");
        return NULL;
    }

    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, (const char* const*)(&vertex_src), NULL);
    glCompileShader(vertex);

    GLint result;
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        GLint length;
        glGetShaderiv(vertex, GL_INFO_LOG_LENGTH, &length);
        char error[length + 1];
        glGetShaderInfoLog(vertex, length, &length, error);
        CELESTE_LOG_ERROR_PATH("Failed to compile vertex shader!", error);
        glDeleteShader(vertex);
        return NULL;
    }

    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, (const char* const*)(&fragment_src), NULL);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        GLint length;
        glGetShaderiv(fragment, GL_INFO_LOG_LENGTH, &length);
        char error[length + 1];
        glGetShaderInfoLog(fragment, length, &length, error);
        CELESTE_LOG_ERROR_PATH("Failed to compile fragment shader!", error);
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return NULL;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);

    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    celeste_shader_t* shader = malloc(sizeof(celeste_shader_t));
    shader->id = program;
    return shader;
}

celeste_shader_t *celeste_shader_default()
{
    return celeste_get_instance()->default_shader;
}

void celeste_shader_destroy(celeste_shader_t* shader)
{
    glDeleteProgram(shader->id);
    free(shader);
}

void celeste_shader_activate(celeste_shader_t* shader)
{
    glUseProgram(shader->id);
}

void celeste_shader_deactivate()
{
    glUseProgram(0);
}

void celeste_shader_uniform_int(celeste_shader_t* shader, char* uniform_name, int value)
{
    glUniform1i(glGetUniformLocation(shader->id, uniform_name), value);
}

void celeste_shader_uniform_float(celeste_shader_t* shader, char* uniform_name, float value)
{
    glUniform1f(glGetUniformLocation(shader->id, uniform_name), value);
}

void celeste_shader_uniform_vec2(celeste_shader_t* shader, char* uniform_name, vec2 value)
{
    glUniform2f(glGetUniformLocation(shader->id, uniform_name), value[0], value[1]);
}

void celeste_shader_uniform_vec3(celeste_shader_t* shader, char* uniform_name, vec3 value)
{
    glUniform3f(glGetUniformLocation(shader->id, uniform_name), value[0], value[1], value[2]);
}

void celeste_shader_uniform_vec4(celeste_shader_t* shader, char* uniform_name, vec4 value)
{
    glUniform4f(glGetUniformLocation(shader->id, uniform_name), value[0], value[1], value[2], value[3]);
}

void celeste_shader_uniform_mat4(celeste_shader_t* shader, char* uniform_name, mat4 value)
{
    float v[16] = {
        value[0][0], value[0][1], value[0][2], value[0][3],
        value[1][0], value[1][1], value[1][2], value[1][3],
        value[2][0], value[2][1], value[2][2], value[2][3],
        value[3][0], value[3][1], value[3][2], value[3][3],
    };
    glUniformMatrix4fv(glGetUniformLocation(shader->id, uniform_name), 1, GL_FALSE, v);
}

celeste_texture_t* celeste_texture_create(char* filepath)
{
    celeste_texture_t* texture = malloc(sizeof(celeste_texture_t));
    unsigned char* pixels;
    unsigned int width;
    unsigned int height;
    unsigned int bpp;

    struct stat s;

    if (stat(filepath, &s)) {
        CELESTE_LOG_ERROR_PATH("Failed to stat texture!", filepath);
        return NULL;
    }

    stbi_set_flip_vertically_on_load(1);
    pixels = stbi_load(filepath, (int*)&width, (int*)&height, (int*)&bpp, STBI_rgb_alpha);
    if (!pixels)
        return NULL;

    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT/*GL_CLAMP_TO_EDGE*/);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT/*GL_CLAMP_TO_EDGE*/);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(pixels);

    texture->id = id;
    texture->width = width;
    texture->height = height;
    return texture;
}

void celeste_texture_destroy(celeste_texture_t* texture)
{
    glDeleteTextures(1, &(texture->id));
    free(texture);
}

void celeste_texture_bind(celeste_texture_t* texture)
{
    glBindTexture(GL_TEXTURE_2D, texture->id);
}

void celeste_texture_unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

celeste_font_t* celeste_font_create(const char* filepath, unsigned int size)
{
    celeste_font_t* font = malloc(sizeof(celeste_font_t));
    font->size = size;
    font->ftatlas = texture_atlas_new(512, 512, 1);
    font->ftfont = texture_font_new_from_file(font->ftatlas, size, filepath);
    return font;
}

void celeste_font_destroy(celeste_font_t* font)
{
    texture_atlas_delete(font->ftatlas);
    texture_font_delete(font->ftfont);
    free(font);
}
