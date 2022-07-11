#include <glad/glad.h>
#include "renderer.h"
#include <stdlib.h>
#include <string.h>
#include "window.h"

celeste_renderer_t* celeste_renderer_create()
{
    celeste_renderer_t* renderer = malloc(sizeof(celeste_renderer_t));

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, RENDERER_BUFFER_SIZE, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(SHADER_VERTEX_INDEX);
    glEnableVertexAttribArray(SHADER_UV_INDEX);
    glEnableVertexAttribArray(SHADER_TID_INDEX);
    glEnableVertexAttribArray(SHADER_COLOR_INDEX);

    glVertexAttribPointer(SHADER_VERTEX_INDEX, 3, GL_FLOAT, GL_FALSE, RENDERER_VERTEX_SIZE, (const GLvoid*)0);
    glVertexAttribPointer(SHADER_UV_INDEX, 2, GL_FLOAT, GL_FALSE, RENDERER_VERTEX_SIZE, (const GLvoid*)(offsetof(vertex_data_t, uv)));
    glVertexAttribIPointer(SHADER_TID_INDEX, 1, GL_INT, RENDERER_VERTEX_SIZE, (const GLvoid*)(offsetof(vertex_data_t, tid)));
    glVertexAttribPointer(SHADER_COLOR_INDEX, 4, GL_UNSIGNED_BYTE, GL_TRUE, RENDERER_VERTEX_SIZE, (const GLvoid*)(offsetof(vertex_data_t, color)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint indices[RENDERER_INDICES_SIZE];

    int offset = 0;
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

    index_buffer_t* ibo = celeste_index_buffer_create(indices, RENDERER_INDICES_SIZE);

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

void celeste_renderer_destroy(celeste_renderer_t* renderer)
{
    celeste_index_buffer_destroy(renderer->ibo);
    free(renderer->texture_slots);
    free(renderer->transformation_back);
    glDeleteBuffers(1, &renderer->vbo);
    free(renderer);
}

celeste_renderer_t *celeste_renderer_default()
{
    return celeste_get_instance()->default_renderer;
}

void celeste_renderer_push(celeste_renderer_t* renderer, mat4 transformation)
{
    renderer->transformation_back = realloc(renderer->transformation_back, (renderer->transformation_back_size + 1) * sizeof(mat4));
    glm_mat4_mul(renderer->transformation_back[renderer->transformation_back_size - 1], transformation,
                 renderer->transformation_back[renderer->transformation_back_size]);
    renderer->transformation_back_size++;
}

void celeste_renderer_pop(celeste_renderer_t* renderer)
{
    renderer->transformation_back = realloc(renderer->transformation_back, (renderer->transformation_back_size - 1) * sizeof(mat4));
    renderer->transformation_back_size--;
}

void celeste_renderer_begin(celeste_renderer_t* renderer)
{
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
    renderer->buffer = (vertex_data_t*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
}

void celeste_renderer_submit_sprite(celeste_renderer_t* renderer, celeste_sprite_t* sprite)
{
    float position_x = sprite->position[0];
    float position_y = sprite->position[1];
    float position_z = sprite->position[2];

    float size_x = sprite->size[0];
    float size_y = sprite->size[1];
    unsigned int color = sprite->color;
    GLuint tid = sprite->tid;

    int ts = 0;
    if (tid > 0)
    {
        unsigned char found = 0;
        for (int i = 0; i < renderer->texture_slots_size; i++)
        {
            if (renderer->texture_slots[i] == tid)
            {
                ts = i + 1;
                found = 1;
                break;
            }
        }

        if (!found)
        {
            if (renderer->texture_slots_size >= RENDERER_MAX_TEXTURES)
            {
                celeste_renderer_end();
                celeste_renderer_flush(renderer);
                celeste_renderer_begin(renderer);
            }
            renderer->texture_slots[renderer->texture_slots_size] = tid;
            renderer->texture_slots_size++;
            ts = renderer->texture_slots_size;
        }
    }

    glm_vec3((float[]){ position_x, position_y, position_z }, renderer->buffer->vertex);
    glm_mat4_mulv3(renderer->transformation_back[renderer->transformation_back_size - 1], renderer->buffer->vertex, 1.0f, renderer->buffer->vertex);
    glm_vec2(sprite->uv[0], renderer->buffer->uv);
    renderer->buffer->tid = ts;
    renderer->buffer->color = color;
    renderer->buffer++;

    glm_vec3((float[]){ position_x, position_y + size_y, position_z }, renderer->buffer->vertex);
    glm_mat4_mulv3(renderer->transformation_back[renderer->transformation_back_size - 1], renderer->buffer->vertex, 1.0f, renderer->buffer->vertex);
    glm_vec2(sprite->uv[1], renderer->buffer->uv);
    renderer->buffer->tid = ts;
    renderer->buffer->color = color;
    renderer->buffer++;

    glm_vec3((float[]){ position_x + size_x, position_y + size_y, position_z }, renderer->buffer->vertex);
    glm_mat4_mulv3(renderer->transformation_back[renderer->transformation_back_size - 1], renderer->buffer->vertex, 1.0f, renderer->buffer->vertex);
    glm_vec2(sprite->uv[2], renderer->buffer->uv);
    renderer->buffer->tid = ts;
    renderer->buffer->color = color;
    renderer->buffer++;

    glm_vec3((float[]){ position_x + size_x, position_y, position_z }, renderer->buffer->vertex);
    glm_mat4_mulv3(renderer->transformation_back[renderer->transformation_back_size - 1], renderer->buffer->vertex, 1.0f, renderer->buffer->vertex);
    glm_vec2(sprite->uv[3], renderer->buffer->uv);
    renderer->buffer->tid = ts;
    renderer->buffer->color = color;
    renderer->buffer++;

    renderer->index_count += 6;
}

void celeste_renderer_submit_label(celeste_renderer_t* renderer, celeste_label_t *label)
{
    vec3 position;
    glm_vec3(label->position, position);
    celeste_font_t *font = label->font;
    char *text = label->text;
    celeste_t* celeste = celeste_get_instance();
    int screen_width = celeste->winwidth;
    int screen_height = celeste->winheight;
    int color = label->color;

    int ts = 0;
    unsigned char found = 0;
    for (int i = 0; i < renderer->texture_slots_size; i++)
    {
        if (renderer->texture_slots[i] == font->ftatlas->id)
        {
            ts = i + 1;
            found = 1;
            break;
        }
    }

    if (!found)
    {
        if (renderer->texture_slots_size >= RENDERER_MAX_TEXTURES)
        {
            celeste_renderer_end();
            celeste_renderer_flush(renderer);
            celeste_renderer_begin(renderer);
        }
        renderer->texture_slots[renderer->texture_slots_size] = font->ftatlas->id;
        renderer->texture_slots_size++;
        ts = renderer->texture_slots_size;
    }

    float scaleX = screen_width / renderer->projection_x;
    float scaleY = screen_height / renderer->projection_y;

    float x = position[0];

    texture_font_t* ftFont = font->ftfont;
    int text_len = strlen(text);

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

            glm_vec3((float[]){ x0, y0, 0 }, renderer->buffer->vertex);
            glm_mat4_mulv3(renderer->transformation_back[renderer->transformation_back_size - 1], renderer->buffer->vertex, 1.0f, renderer->buffer->vertex);
            glm_vec2((float[]){ u0, v0 }, renderer->buffer->uv);
            renderer->buffer->tid = ts;
            renderer->buffer->color = color;
            renderer->buffer++;

            glm_vec3((float[]){ x0, y1, 0 }, renderer->buffer->vertex);
            glm_mat4_mulv3(renderer->transformation_back[renderer->transformation_back_size - 1], renderer->buffer->vertex, 1.0f, renderer->buffer->vertex);
            glm_vec2((float[]){ u0, v1 }, renderer->buffer->uv);
            renderer->buffer->tid = ts;
            renderer->buffer->color = color;
            renderer->buffer++;

            glm_vec3((float[]){ x1, y1, 0 }, renderer->buffer->vertex);
            glm_mat4_mulv3(renderer->transformation_back[renderer->transformation_back_size - 1], renderer->buffer->vertex, 1.0f, renderer->buffer->vertex);
            glm_vec2((float[]){ u1, v1 }, renderer->buffer->uv);
            renderer->buffer->tid = ts;
            renderer->buffer->color = color;
            renderer->buffer++;

            glm_vec3((float[]){ x1, y0, 0 }, renderer->buffer->vertex);
            glm_mat4_mulv3(renderer->transformation_back[renderer->transformation_back_size - 1], renderer->buffer->vertex, 1.0f, renderer->buffer->vertex);
            glm_vec2((float[]){ u1, v0 }, renderer->buffer->uv);
            renderer->buffer->tid = ts;
            renderer->buffer->color = color;
            renderer->buffer++;

            renderer->index_count += 6;

            x += glyph->advance_x / scaleX;
        }
    }
}

void celeste_renderer_end()
{
    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void celeste_renderer_flush(celeste_renderer_t* renderer)
{
    for (int i = 0; i < renderer->texture_slots_size; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, renderer->texture_slots[i]);
    }

    glBindVertexArray(renderer->vao);
    celeste_index_buffer_bind(renderer->ibo);

    glDrawElements(GL_TRIANGLES, renderer->index_count, GL_UNSIGNED_INT, NULL);

    celeste_index_buffer_unbind();
    glBindVertexArray(0);

    renderer->index_count = 0;
    renderer->texture_slots_size = 0;
}
