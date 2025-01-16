#include "internal/gapi.h"
#include "internal/renderer.h"
#include "internal/celeste.h"

#include <glad/glad.h>
#include "renderer.h"
#include <stdlib.h>
#include <string.h>

static uint32_t submit_texture(CLSTrenderer *renderer, uint32_t tid);

CLSTrenderer *clstRenderer()
{
    CLSTrenderer *renderer;
    GLuint vao, vbo;
    CLSTindexbuffer *ibo;
    GLuint *indices;
    int offset;

    renderer = malloc(sizeof(CLSTrenderer));
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, RENDERER_BUFFER_SIZE, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(SHADER_VERTEX_INDEX);
    glEnableVertexAttribArray(SHADER_UV_INDEX);
    glEnableVertexAttribArray(SHADER_TID_INDEX);
    glEnableVertexAttribArray(SHADER_COLOR_INDEX);
    // glEnableVertexAttribArray(SHADER_NORMAL_INDEX);

    glVertexAttribPointer(SHADER_VERTEX_INDEX, 3, GL_FLOAT, GL_FALSE, RENDERER_VERTEX_SIZE, (const GLvoid*)0);
    glVertexAttribPointer(SHADER_UV_INDEX, 2, GL_FLOAT, GL_FALSE, RENDERER_VERTEX_SIZE, (const GLvoid*)(offsetof(CLSTvertexdata, uv)));
    glVertexAttribPointer(SHADER_TID_INDEX, 1, GL_FLOAT, GL_FALSE, RENDERER_VERTEX_SIZE, (const GLvoid*)(offsetof(CLSTvertexdata, tid)));
    glVertexAttribPointer(SHADER_COLOR_INDEX, 4, GL_UNSIGNED_BYTE, GL_TRUE, RENDERER_VERTEX_SIZE, (const GLvoid*)(offsetof(CLSTvertexdata, color)));
    // glVertexAttribPointer(SHADER_NORMAL_INDEX, 3, GL_FLOAT, GL_FALSE, RENDERER_VERTEX_SIZE, (const GLvoid*)(offsetof(CLSTvertexdata, normal)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    indices = malloc(RENDERER_INDICES_SIZE * sizeof(GLuint));
    offset = 0;
    for (int i = 0; i < RENDERER_INDICES_SIZE; i += 6)
    {
        indices[  i  ] = offset + 0;
        indices[i + 1] = offset + 1;
        indices[i + 2] = offset + 2;

        indices[i + 3] = offset + 2;
        indices[i + 4] = offset + 3;
        indices[i + 5] = offset + 0;

        offset += 4;
    }

    ibo = clstIndexBuffer(indices, RENDERER_INDICES_SIZE);
    free(indices);

    glBindVertexArray(0);

    renderer->vao = vao;
    renderer->vbo = vbo;
    renderer->ibo = ibo;
    renderer->index_count = 0;
    renderer->buffer = NULL;
    renderer->texture_slots = malloc(RENDERER_MAX_TEXTURES * sizeof(unsigned int));
    renderer->texture_slots_size = 0;
    renderer->transformation_back = malloc(sizeof(mat4));
    glm_mat4_identity(*(renderer->transformation_back));
    renderer->transformation_back_size = 1;
    renderer->projection_x = 1.0f;
    renderer->projection_y = 1.0f;
    return renderer;
}

void clstRendererDestroy(CLSTrenderer *renderer)
{
    clstIndexBufferDestroy(renderer->ibo);
    free(renderer->texture_slots);
    free(renderer->transformation_back);
    glDeleteBuffers(1, &renderer->vbo);
    free(renderer);
}

CLSTrenderer *clstRendererDefault()
{
    return clstInstance()->default_renderer;
}

void clstRendererMat4Push(CLSTrenderer *renderer, mat4 transformation)
{
    renderer->transformation_back = realloc(renderer->transformation_back, (renderer->transformation_back_size + 1) * sizeof(mat4));
    glm_mat4_mul(renderer->transformation_back[renderer->transformation_back_size - 1], transformation,
                 renderer->transformation_back[renderer->transformation_back_size]);
    renderer->transformation_back_size++;
}

void clstRendererMat4Pop(CLSTrenderer *renderer)
{
    renderer->transformation_back = realloc(renderer->transformation_back, (renderer->transformation_back_size - 1) * sizeof(mat4));
    renderer->transformation_back_size--;
}

void clstRendererBegin(CLSTrenderer *renderer)
{
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
    renderer->buffer = (CLSTvertexdata *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
}

void clstRendererDrawQuad(CLSTrenderer *renderer, vec2 position, vec2 size, CLSTtexture *texture, vec2 uv[4], uint32_t color)
{
    const float position_x = position[0];
    const float position_y = position[1];

    const float size_x = size[0];
    const float size_y = size[1];

    vec4 *transformation = renderer->transformation_back[renderer->transformation_back_size - 1];

    int ts = 0;
    if (texture)
        ts = submit_texture(renderer, texture->id);

    glm_mat4_mulv3(transformation, (float[]){ position_x, position_y, 0.0f }, 1.0f, renderer->buffer->vertex);
    glm_vec2_copy(uv[0], renderer->buffer->uv);
    renderer->buffer->tid = ts;
    renderer->buffer->color = color;
    renderer->buffer++;

    glm_mat4_mulv3(transformation, (float[]){ position_x, position_y + size_y, 0.0f }, 1.0f, renderer->buffer->vertex);
    glm_vec2_copy(uv[1], renderer->buffer->uv);
    renderer->buffer->tid = ts;
    renderer->buffer->color = color;
    renderer->buffer++;

    glm_mat4_mulv3(transformation, (float[]){ position_x + size_x, position_y + size_y, 0.0f }, 1.0f, renderer->buffer->vertex);
    glm_vec2_copy(uv[2], renderer->buffer->uv);
    renderer->buffer->tid = ts;
    renderer->buffer->color = color;
    renderer->buffer++;

    glm_mat4_mulv3(transformation, (float[]){ position_x + size_x, position_y, 0.0f }, 1.0f, renderer->buffer->vertex);
    glm_vec2_copy(uv[3], renderer->buffer->uv);
    renderer->buffer->tid = ts;
    renderer->buffer->color = color;
    renderer->buffer++;

    renderer->index_count += 6;
}

void clstRendererDrawString(CLSTrenderer *renderer, vec2 position, vec2 uv[4], CLSTfont *font, char *text, uint32_t color)
{
    const int screen_width = 1920;
    const int screen_height = 1080;
    const float scaleX = screen_width / renderer->projection_x;
    const float scaleY = screen_height / renderer->projection_y;

    vec4 *transformation = renderer->transformation_back[renderer->transformation_back_size - 1];

    int ts;
    float x;
    texture_font_t* ftFont;
    unsigned long text_len;

    ts = submit_texture(renderer, font->ftatlas->id);
    x = position[0];
    ftFont = font->ftfont;
    text_len = strlen(text);

    for (int i = 0; i < text_len; i++)
    {
        char c = text[i];
        texture_glyph_t* glyph = texture_font_get_glyph(ftFont, c);
        if (glyph != NULL)
        {
            if (i > 0)
            {
                float kerning = texture_glyph_get_kerning(glyph, text[i - 1]);
                x += kerning / scaleX;
            }

            float x0 = x + glyph->offset_x / scaleX;
            float y0 = position[1] + glyph->offset_y / scaleY;
            float x1 = x0 + glyph->width / scaleX;
            float y1 = y0 - glyph->height / scaleY;

            float u0 = glyph->s0;
            float v0 = glyph->t0;
            float u1 = glyph->s1;
            float v1 = glyph->t1;

            glm_mat4_mulv3(transformation, (float[]){ x0, y0, 0.0f }, 1.0f, renderer->buffer->vertex);
            glm_vec2_copy((float[]){ u0, v0 }, renderer->buffer->uv);
            renderer->buffer->tid = ts;
            renderer->buffer->color = color;
            renderer->buffer++;

            glm_mat4_mulv3(transformation, (float[]){ x0, y1, 0.0f }, 1.0f, renderer->buffer->vertex);
            glm_vec2_copy((float[]){ u0, v1 }, renderer->buffer->uv);
            renderer->buffer->tid = ts;
            renderer->buffer->color = color;
            renderer->buffer++;

            glm_mat4_mulv3(transformation, (float[]){ x1, y1, 0.0f }, 1.0f, renderer->buffer->vertex);
            glm_vec2_copy((float[]){ u1, v1 }, renderer->buffer->uv);
            renderer->buffer->tid = ts;
            renderer->buffer->color = color;
            renderer->buffer++;

            glm_mat4_mulv3(transformation, (float[]){ x1, y0, 0.0f }, 1.0f, renderer->buffer->vertex);
            glm_vec2_copy((float[]){ u1, v0 }, renderer->buffer->uv);
            renderer->buffer->tid = ts;
            renderer->buffer->color = color;
            renderer->buffer++;

            renderer->index_count += 6;

            x += glyph->advance_x / scaleX;
        }
    }
}

void clstRendererEnd()
{
    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void clstRendererFlush(CLSTrenderer *renderer)
{
    for (int i = 0; i < renderer->texture_slots_size; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, renderer->texture_slots[i]);
    }

    glBindVertexArray(renderer->vao);
    clstIndexBufferBind(renderer->ibo);

    glDrawElements(GL_TRIANGLES, renderer->index_count, GL_UNSIGNED_INT, NULL);

    clstIndexBufferUnbind();
    glBindVertexArray(0);

    renderer->index_count = 0;
    renderer->texture_slots_size = 0;
}

static uint32_t submit_texture(CLSTrenderer *renderer, uint32_t tid)
{
    int ts;

    ts = 0;
    for (int i = 0; i < renderer->texture_slots_size; i++)
    {
        if (renderer->texture_slots[i] == tid)
        {
            ts = i + 1;
            break;
        }
    }

    if (!ts)
    {
        if (renderer->texture_slots_size >= RENDERER_MAX_TEXTURES)
        {
            clstRendererEnd();
            clstRendererFlush(renderer);
            clstRendererBegin(renderer);
        }
        renderer->texture_slots[renderer->texture_slots_size] = tid;
        renderer->texture_slots_size++;
        ts = renderer->texture_slots_size;
    }
    return ts;
}
