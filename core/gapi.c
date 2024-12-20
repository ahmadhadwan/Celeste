#include "internal/gapi.h"
#include "internal/celeste.h"

#include <celeste/celeste.h>
#include "gapi.h"
#include <glad/glad.h>
#include <stb/stb_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static GLuint opengl_texture_create(uint8_t *pixels, uint32_t width, uint32_t height, GLenum format);

CLSTbuffer *clstBuffer(float *vertices, uint32_t count, uint32_t component_count)
{
    CLSTbuffer *buffer;
    GLuint id;

    glGenBuffers(1, &id);

    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), vertices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    buffer = malloc(sizeof(CLSTbuffer));
    buffer->id = id;
    buffer->component_count = component_count;
    return buffer;
}

void clstBufferDestroy(CLSTbuffer *buffer)
{
    glDeleteBuffers(1, &(buffer->id));
    free(buffer);
}

void clstBufferBind(CLSTbuffer *buffer)
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer->id);
}

void clstBufferUnbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

CLSTindexbuffer *clstIndexBuffer(uint32_t *indices, uint32_t count)
{
    CLSTindexbuffer *index_buffer;
    GLuint id;

    glGenBuffers(1, &id);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), indices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    index_buffer = malloc(sizeof(CLSTindexbuffer));
    index_buffer->id = id;
    index_buffer->count = count;
    return index_buffer;
}

void clstIndexBufferDestroy(CLSTindexbuffer *index_buffer)
{
    glDeleteBuffers(1, &(index_buffer->id));
    free(index_buffer);
}

void clstIndexBufferBind(CLSTindexbuffer *index_buffer)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer->id);
}

void clstIndexBufferUnbind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

CLSTvertexarray *clstVertexArray()
{
    CLSTvertexarray *vertex_array;
    GLuint id;

    glGenVertexArrays(1, &id);

    vertex_array = malloc(sizeof(CLSTvertexarray));
    vertex_array->id = id;
    vertex_array->buffers = NULL;
    vertex_array->buffers_count = 0;
    return vertex_array;
}

void clstVertexArrayDestroy(CLSTvertexarray *vertex_array)
{
    for (unsigned int i = 0; i < vertex_array->buffers_count; i++)
        clstBufferDestroy(vertex_array->buffers[i]);

    glDeleteVertexArrays(1, &(vertex_array->id));
    if (vertex_array->buffers_count)
        free(vertex_array->buffers);
    free(vertex_array);
}

void clstVertexArrayAddBuffer(CLSTvertexarray *vertex_array, CLSTbuffer *buffer)
{
    vertex_array->buffers = realloc(vertex_array->buffers, vertex_array->buffers_count + 1);
    vertex_array->buffers[vertex_array->buffers_count] = buffer;
    vertex_array->buffers_count++;
}

void clstVertexArrayLinkAttribute(CLSTvertexarray *vertex_array, uint32_t layout, int size, void *offset)
{
    CLSTbuffer *buffer;

    buffer = vertex_array->buffers[vertex_array->buffers_count - 1];
    glBindVertexArray(vertex_array->id);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->id);

    glEnableVertexAttribArray(layout);
    glVertexAttribPointer(layout, size, GL_FLOAT, GL_FALSE, buffer->component_count * sizeof(float), offset);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void clstVertexArrayBind(CLSTvertexarray *vertex_array)
{
    glBindVertexArray(vertex_array->id);
}

void clstVertexArrayUnbind()
{
    glBindVertexArray(0);
}

CLSTshader* clstShader(char *filepath)
{
    struct stat s;
    FILE *fd;
    char *src;
    CLSTshader *shader;

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

    shader = clstShaderSrc(src);
    free(src);
    return shader;
}

CLSTshader *clstShaderConstSrc(const char *constsrc)
{
    CLSTshader *shader;
    char *src;

    src = strdup(constsrc);
    shader = clstShaderSrc(src);

    free(src);
    return shader;
}

CLSTshader *clstShaderSrc(char *src)
{
    CLSTshader *shader;
    char *vertex_src, *fragment_src;
    GLuint vertex, fragment, program;
    GLint result;

    vertex_src = fragment_src = NULL;
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

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, (const char* const*)(&vertex_src), NULL);
    glCompileShader(vertex);

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

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
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

    program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);

    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    shader = malloc(sizeof(CLSTshader));
    shader->id = program;
    return shader;
}

CLSTshader *clstShaderDefault()
{
    return clstInstance()->default_shader;
}

void clstShaderDestroy(CLSTshader *shader)
{
    glDeleteProgram(shader->id);
    free(shader);
}

void clstShaderActivate(CLSTshader *shader)
{
    glUseProgram(shader->id);
}

void clstShaderDeactivate()
{
    glUseProgram(0);
}

void clstShaderUniformInt(CLSTshader *shader, char *uniform_name, int value)
{
    glUniform1i(glGetUniformLocation(shader->id, uniform_name), value);
}

void clstShaderUniformIntFloat(CLSTshader *shader, char *uniform_name, float value)
{
    glUniform1f(glGetUniformLocation(shader->id, uniform_name), value);
}

void clstShaderUniformVec2(CLSTshader *shader, char *uniform_name, vec2 value)
{
    glUniform2f(glGetUniformLocation(shader->id, uniform_name), value[0], value[1]);
}

void clstShaderUniformVec3(CLSTshader *shader, char *uniform_name, vec3 value)
{
    glUniform3f(glGetUniformLocation(shader->id, uniform_name), value[0], value[1], value[2]);
}

void clstShaderUniformVec4(CLSTshader *shader, char *uniform_name, vec4 value)
{
    glUniform4f(glGetUniformLocation(shader->id, uniform_name), value[0], value[1], value[2], value[3]);
}

void clstShaderUniformMat4(CLSTshader *shader, char *uniform_name, mat4 value)
{
    float v[16] = {
        value[0][0], value[0][1], value[0][2], value[0][3],
        value[1][0], value[1][1], value[1][2], value[1][3],
        value[2][0], value[2][1], value[2][2], value[2][3],
        value[3][0], value[3][1], value[3][2], value[3][3],
    };
    glUniformMatrix4fv(glGetUniformLocation(shader->id, uniform_name), 1, GL_FALSE, v);
}

CLSTtexture *clstTexture(char *filepath, char *name)
{
    CLSTtexture *texture;
    uint8_t *pixels;
    uint32_t width, height, bpp;
    struct stat s;

    if (stat(filepath, &s)) {
        CELESTE_LOG_ERROR_PATH("Failed to stat texture!", filepath);
        return NULL;
    }

    stbi_set_flip_vertically_on_load(1);
    pixels = stbi_load(filepath, (int *)&width, (int *)&height, (int *)&bpp, STBI_rgb_alpha);
    if (!pixels)
        return NULL;

    texture = malloc(sizeof(CLSTtexture));
    texture->id = opengl_texture_create(pixels, width, height, GL_RGBA);
    texture->width = width;
    texture->height = height;
    texture->name = malloc(strlen(name) + 1);
    strcpy(texture->name, name);

    stbi_image_free(pixels);
    return texture;
}

CLSTtexture *clstTextureMem(uint8_t *buffer, uint32_t bufsize, char *name)
{
    CLSTtexture *texture;
    uint8_t *pixels;
    uint32_t width, height, bpp;

    stbi_set_flip_vertically_on_load(1);
    pixels = stbi_load_from_memory(buffer, bufsize, (int *)&width, (int *)&height, (int *)&bpp, STBI_rgb_alpha);
    if (!pixels)
        return NULL;

    texture = malloc(sizeof(CLSTtexture));
    texture->id = opengl_texture_create(pixels, width, height, GL_RGBA);
    texture->width = width;
    texture->height = height;
    texture->name = malloc(strlen(name) + 1);
    strcpy(texture->name, name);

    stbi_image_free(pixels);
    return texture;
}

CLSTtexture *clstTextureInline(uint8_t *pixels, uint32_t width, uint32_t height, uint32_t bpp, char *name)
{
    CLSTtexture *texture;

    texture = malloc(sizeof(CLSTtexture));
    texture->id = opengl_texture_create(pixels, width, height, bpp == 32 ? GL_RGBA : GL_RGB);
    texture->width = width;
    texture->height = height;
    texture->name = strdup(name);
    return texture;
}

void clstTextureDestroy(CLSTtexture *texture)
{
    glDeleteTextures(1, &(texture->id));
    free(texture->name);
    free(texture);
}

void clstTextureBind(CLSTtexture *texture)
{
    glBindTexture(GL_TEXTURE_2D, texture->id);
}

void clstTextureUnbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned int clstTextureWidth(CLSTtexture *texture)
{
    return texture->width;
}

unsigned int clstTextureHeight(CLSTtexture *texture)
{
    return texture->height;
}

CLSTfont *clstFont(const char *filepath, float size, char *name)
{
    CLSTfont *font;

    font = malloc(sizeof(CLSTfont));
    font->size = size;
    font->ftatlas = texture_atlas_new(512, 512, 1);
    font->ftfont = texture_font_new_from_file(font->ftatlas, size, filepath);
    font->name = strdup(name);
    return font;
}

CLSTfont *clstFontMem(uint8_t *buffer, uint32_t bufsize, float size, char *name)
{
    CLSTfont *font;

    font = malloc(sizeof(CLSTfont));
    font->size = size;
    font->ftatlas = texture_atlas_new(512, 512, 1);
    font->ftfont = texture_font_new_from_memory(font->ftatlas, size, buffer, bufsize);
    font->name = strdup(name);
    return font;
}

void clstFontDestroy(CLSTfont *font)
{
    texture_atlas_delete(font->ftatlas);
    texture_font_delete(font->ftfont);
    free(font->name);
    free(font);
}

CLSTframebuffer *clstFrameBuffer()
{
    CLSTframebuffer *framebuffer;

    framebuffer = malloc(sizeof(CLSTframebuffer));
    glGenFramebuffers(1, &(framebuffer->id));
    return framebuffer;
}

void clstFrameBufferDestroy(CLSTframebuffer *framebuffer)
{
    glDeleteRenderbuffers(1, &(framebuffer->rbo));
    glDeleteFramebuffers(1, &(framebuffer->id));
    free(framebuffer);
}

void clstFrameBufferBind(CLSTframebuffer *framebuffer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->id);
}

void clstFrameBufferUnbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void clstFrameBufferAttachTexture(CLSTframebuffer *framebuffer, CLSTtexture *texture)
{
    clstFrameBufferBind(framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->id, 0);
    clstFrameBufferUnbind();
}

void clstFrameBufferAttachRenderBuffer(CLSTframebuffer *framebuffer, CLSTrenderbuffer *renderbuffer)
{
    uint32_t rbo;
    
    clstFrameBufferBind(framebuffer);
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo); 
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, clstInstance()->window.width, clstInstance()->window.height);  
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    framebuffer->rbo = rbo;
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    clstFrameBufferUnbind();
}

CLSTresult clstFrameBufferComplete(CLSTframebuffer *framebuffer)
{
    return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE ? CELESTE_OK : !CELESTE_OK;
}

GLuint opengl_texture_create(uint8_t *pixels, uint32_t width, uint32_t height, GLenum format)
{
    GLuint id;

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);

    /*glGenerateMipmap(GL_TEXTURE_2D);*/

    glBindTexture(GL_TEXTURE_2D, 0);
    return id;
}
