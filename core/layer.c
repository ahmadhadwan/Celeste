#include <cglm/cglm.h>
#include <glad/glad.h>
#include "layer.h"
#include <stdio.h>
#include <stdlib.h>
#include "sprite.h"
#include "window.h"

static void render_sprites(celeste_layer_t *layer, celeste_sprite_t *sprite);

celeste_layer_t *celeste_layer_create(float left, float right, float bottom, float top, float near_z, float far_z)
{
    mat4 projection;
    glm_ortho(left, right, bottom, top, near_z, far_z, projection);
    return celeste_layer_create_custom(celeste_renderer_default(), celeste_shader_default(), NULL, projection);
}

celeste_layer_t *celeste_layer_create_mat4(mat4 projection)
{
    return celeste_layer_create_custom(celeste_renderer_default(), celeste_shader_default(), NULL, projection);
}

celeste_layer_t *celeste_layer_create_camera(celeste_camera_t *camera, float left, float right, float bottom, float top, float near_z, float far_z)
{
    mat4 projection;
    glm_ortho(left, right, bottom, top, near_z, far_z, projection);
    return celeste_layer_create_custom(celeste_renderer_default(), celeste_shader_default(), camera, projection);
}

celeste_layer_t *celeste_layer_create_camera_mat4(celeste_camera_t *camera, mat4 projection)
{
    return celeste_layer_create_custom(celeste_renderer_default(), celeste_shader_default(), camera, projection);
}

celeste_layer_t *celeste_layer_create_custom(celeste_renderer_t *renderer, celeste_shader_t *shader, celeste_camera_t *camera, mat4 projection)
{
    celeste_layer_t *layer;
    celeste_t *celeste;
    GLuint textures_uni;

    layer = malloc(sizeof(celeste_layer_t));
    layer->renderer = renderer;
    layer->shader = shader;
    layer->sprites = malloc(0);
    layer->sprites_size = 0;

    layer->camera = camera;

    layer->projection.near_z =  (1 + projection[2][3]) / projection[2][2];
    layer->projection.far_z  = -(1 + projection[2][3]) / projection[2][2];
    layer->projection.bottom = -(1 - projection[1][3]) / projection[1][1];
    layer->projection.top    =  (1 - projection[1][3]) / projection[1][1];
    layer->projection.left   = -(1 + projection[0][3]) / projection[0][0];
    layer->projection.right  =  (1 + projection[0][3]) / projection[0][0];

    celeste = celeste_get_instance();
    layer->cursor.x = (float)(celeste->wincursor.x * (layer->projection.right * 2) / celeste->winwidth - layer->projection.right);
    layer->cursor.y = (float)(layer->projection.top - celeste->wincursor.y * (layer->projection.top * 2) / celeste->winheight);

    int textures_array[32] = {
         0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
        10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
        20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
        30, 31
    };

    celeste_shader_activate(shader);
    textures_uni = glGetUniformLocation(shader->id, "textures");
    glUniform1iv(textures_uni, 32, textures_array);
    celeste_shader_uniform_mat4(shader, "projection", projection);

    return layer;
}

void celeste_layer_destroy(celeste_layer_t *layer)
{
    free(layer->sprites);
    free(layer);
}

void celeste_layer_add_sprite(celeste_layer_t *layer, void *sprite)
{
    layer->sprites = realloc(layer->sprites, (layer->sprites_size + 1) * sizeof(void*));
    layer->sprites[layer->sprites_size] = sprite;
    layer->sprites_size++;
}

void celeste_layer_render(celeste_layer_t *layer)
{
    mat4 view;
    layer->renderer->projection_x = layer->projection.right * 2;
    layer->renderer->projection_y = layer->projection.top * 2;

    celeste_shader_activate(layer->shader);
    celeste_renderer_begin(layer->renderer);

    if (layer->camera)
    {
        glm_translate_make(view, layer->camera->position);
        celeste_renderer_push(layer->renderer, view);
    }

    for (unsigned int i = 0; i < layer->sprites_size; i++)
        render_sprites(layer, (celeste_sprite_t*)layer->sprites[i]);

    if (layer->camera)
        celeste_renderer_pop(layer->renderer);
    celeste_renderer_end();
    celeste_renderer_flush(layer->renderer);
}

void render_sprites(celeste_layer_t *layer, celeste_sprite_t *sprite)
{
    celeste_t *celeste;
    switch (sprite->type)
    {
        case SPRITE:
            celeste_renderer_submit_sprite(layer->renderer, (celeste_sprite_t*)sprite);
            break;
        case LABEL:
            celeste_renderer_submit_label(layer->renderer, (celeste_label_t*)sprite);
            break;
        case ANIMATION:
        {
            celeste_animation_t *a = (celeste_animation_t*)sprite;
            double time = celeste_get_time();
            if (time >= a->last_animation_time + a->frame_time) {
                if (a->current_frame < a->frames_count - 1)
                    a->current_frame++;
                else
                    a->current_frame = 0;
                a->last_animation_time = time;
            }
            celeste_renderer_submit_sprite(layer->renderer, a->frames[a->current_frame]);
            break;
        }
        case GROUP:
        {
            celeste_group_t *g = (celeste_group_t*)sprite;
            celeste_renderer_push(layer->renderer, g->translation);
            for (int i = 0; i < g->sprites_count; i++) {
                render_sprites(layer, g->sprites[i]);
            }
            celeste_renderer_pop(layer->renderer);
            break;
        }
        case BUTTON:
        {
            celeste_button_t *b = (celeste_button_t*)sprite;
            celeste_group_t  *g = (celeste_group_t*)b->group;
            celeste_sprite_t *s = g->sprites[0];
            vec3 pos;

            celeste_renderer_push(layer->renderer, g->translation);
            for (int i = 0; i < g->sprites_count; i++) {
                render_sprites(layer, g->sprites[i]);
            }

            celeste = celeste_get_instance();
            layer->cursor.x = (float)(celeste->wincursor.x * (layer->projection.right * 2) / celeste->winwidth - layer->projection.right);
            layer->cursor.y = (float)(layer->projection.top - celeste->wincursor.y * (layer->projection.top * 2) / celeste->winheight);

            glm_mat4_mulv3(layer->renderer->transformation_back[layer->renderer->transformation_back_size - 1], s->position, 1.0f, pos);
            if (layer->cursor.x >= pos[0] && layer->cursor.x <= (pos[0] + s->size[0])
             && layer->cursor.y >= pos[1] && layer->cursor.y <= (pos[1] + s->size[1]))
            {
                if (b->status == CELESTE_BUTTON_STATUS_FOCUSED)
                {
                    if (celeste_mouse_button_clicked(CELESTE_MOUSE_LEFT)) {
                        b->status = CELESTE_BUTTON_STATUS_CLICKED;
                    }
                }
                else { /* First frame of the cursor colliding with the button */
                    if (celeste_mouse_button_clicked(CELESTE_MOUSE_LEFT)) {
                        b->status = CELESTE_BUTTON_STATUS_NONE;
                    }
                    else {
                        b->status = CELESTE_BUTTON_STATUS_FOCUSED;
                    }
                }
            }
            else {
                b->status = CELESTE_BUTTON_STATUS_NONE;
            }

            celeste_renderer_pop(layer->renderer);
            break;
        }
        default:
            CELESTE_LOG_ERROR("Sprite type not implemented!");
    }
}
