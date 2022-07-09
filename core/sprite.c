#include "renderer.h"
#include "sprite.h"
#include <stdlib.h>
#include <cglm/cglm.h>

static void default_uv(vec2 uv[4])
{
    glm_vec2((float[]){ 0.0f, 0.0f }, uv[0]);
    glm_vec2((float[]){ 0.0f, 1.0f }, uv[1]);
    glm_vec2((float[]){ 1.0f, 1.0f }, uv[2]);
    glm_vec2((float[]){ 1.0f, 0.0f }, uv[3]);
}

void celeste_sprite_create_tex(vec3 position, vec2 size, celeste_texture_t *texture, celeste_sprite_t *sprite)
{
    sprite->type = SPRITE;
    glm_vec3(position, sprite->position);
    glm_vec2(size, sprite->size);
    default_uv(sprite->uv);
    sprite->tid = texture->id;
    sprite->color = 0xFFFFFFFF;
}

void celeste_sprite_create_tex_from_atlas(vec3 position, vec2 size, celeste_texture_t *texture_atlas, vec2 offset, vec2 texsize, celeste_sprite_t *sprite)
{
    sprite->type = SPRITE;
    glm_vec3(position, sprite->position);
    glm_vec2(size, sprite->size);

    glm_vec2((float[]){
            (offset[0]       * texsize[0]) / texture_atlas->width,
            (offset[1]       * texsize[1]) / texture_atlas->height
    }, sprite->uv[0]);

    glm_vec2((float[]){
            (offset[0]       * texsize[0]) / texture_atlas->width,
            ((offset[1] + 3) * texsize[1]) / texture_atlas->height
    }, sprite->uv[1]);

    glm_vec2((float[]){
            ((offset[0] + 3) * texsize[0]) / texture_atlas->width,
            ((offset[1] + 3) * texsize[1]) / texture_atlas->height
    }, sprite->uv[2]);

    glm_vec2((float[]){
            ((offset[0] + 3) * texsize[0]) / texture_atlas->width,
            (offset[1]       * texsize[1]) / texture_atlas->height
    }, sprite->uv[3]);

    sprite->tid = texture_atlas->id;
    sprite->color = 0xFFFFFFFF;
}

void celeste_sprite_create_col(vec3 position, vec2 size, unsigned int color, celeste_sprite_t *sprite)
{
    sprite->type = SPRITE;
    glm_vec3(position, sprite->position);
    glm_vec2(size, sprite->size);
    default_uv(sprite->uv);
    sprite->tid = 0;
    sprite->color = color;
}

void celeste_label_create(vec3 position, char *text, celeste_font_t *font, celeste_label_t *label)
{
    label->type = LABEL;
    glm_vec3(position, label->position);
    default_uv(label->uv);
    label->font = font;
    label->text = text;
    label->color = 0xFFFFFFFF;
}

void celeste_label_create_col(vec3 position, char *text, celeste_font_t *font, unsigned int color, celeste_label_t *label)
{
    label->type = LABEL;
    glm_vec3(position, label->position);
    default_uv(label->uv);
    label->font = font;
    label->text = text;
    label->color = color;
}

celeste_animation_t *celeste_animation_create(celeste_sprite_t **frames, int frames_count, double frame_time)
{
    celeste_animation_t *animation = malloc(sizeof(celeste_animation_t));
    animation->type = ANIMATION;
    animation->frames_count = frames_count;
    animation->current_frame = 0;
    animation->frames = malloc(frames_count * sizeof(celeste_sprite_t*));
    animation->frame_time = frame_time;
    animation->last_animation_time = 0.0;

    for (int i = 0; i < frames_count; i++)
        animation->frames[i] = frames[i];

    return animation;
}

void celeste_animation_destroy(celeste_animation_t *animation)
{
    free(animation->frames);
    free(animation);
}

celeste_group_t *celeste_group_create(vec3 position)
{
    celeste_group_t *group = malloc(sizeof(celeste_group_t));
    group->type = GROUP;
    group->sprites_count = 0;
    group->sprites = malloc(0);
    glm_translate_make(group->translation, position);
    return group;

}

void celeste_group_destroy(celeste_group_t *group)
{
    free(group->sprites);
    free(group);
}

void celeste_group_add_sprite(celeste_group_t *group, void *sprite)
{
    group->sprites = realloc(group->sprites, (group->sprites_count + 1) * sizeof(celeste_sprite_t*));
    group->sprites[group->sprites_count] = sprite;
    group->sprites_count++;
}

void celeste_button_create(celeste_group_t *group, celeste_button_t *button)
{
    button->type = BUTTON;
    button->status = CELESTE_BUTTON_STATUS_NONE;
    button->group = group;
}
