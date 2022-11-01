#include "internal/gapi.h"
#include "internal/renderer.h"

#include "celeste.h"
#include "renderer.h"
#include "sprite.h"
#include <stdlib.h>
#include <cglm/cglm.h>

static void default_uv(vec2 uv[4])
{
    glm_vec2_copy((float[]){ 0.0f, 0.0f }, uv[0]);
    glm_vec2_copy((float[]){ 0.0f, 1.0f }, uv[1]);
    glm_vec2_copy((float[]){ 1.0f, 1.0f }, uv[2]);
    glm_vec2_copy((float[]){ 1.0f, 0.0f }, uv[3]);
}

static void draw_sprite(CLSTrenderer *renderer, CLSTsprite *sprite)
{
    clstRendererDrawQuad(renderer, sprite->position, sprite->size, sprite->texture, sprite->uv, sprite->color);
}

static void draw_label(CLSTrenderer *renderer, CLSTlabel *label)
{
    clstRendererDrawString(renderer, label->position, label->uv, label->font, label->text, label->color);
}

static void draw_animation(CLSTrenderer *renderer, CLSTanimation *animation)
{
    double time;
    CLSTsprite *sprite;

    time = clstTime();
    if (time >= animation->last_animation_time + animation->frame_time) {
        if (animation->current_frame < animation->frames_count - 1)
            animation->current_frame++;
        else
            animation->current_frame = 0;
        animation->last_animation_time = time;
    }

    sprite = animation->frames[animation->current_frame];
    clstRendererDrawQuad(renderer, sprite->position, sprite->size, sprite->texture, sprite->uv, sprite->color);
}

static void draw_group(CLSTrenderer *renderer, CLSTgroup *group)
{
    CLSTsprite *sprite;

    clstRendererMat4Push(renderer, group->translation);
    for (int i = 0; i < group->sprites_count; i++) {
        sprite = group->sprites[i];
        sprite->draw(renderer, sprite);
    }
    clstRendererMat4Pop(renderer);
}

static void draw_button(CLSTrenderer *renderer, CLSTbutton *button)
{
    CLSTsprite *sprite;

    sprite = button->sprite;
    sprite->draw(renderer, sprite);

    glm_mat4_copy(renderer->transformation_back[renderer->transformation_back_size - 1], button->translation);

    clstButtonEv(button, renderer->projection_x, renderer->projection_y);
}

void clstSprite(vec2 position, vec2 size, CLSTtexture *texture, CLSTsprite *sprite)
{
    sprite->draw = (draw_func)draw_sprite;
    glm_vec2_copy(position, sprite->position);
    glm_vec2_copy(size, sprite->size);
    default_uv(sprite->uv);
    sprite->texture = texture;
    sprite->color = 0xFFFFFFFF;
}

void clstSpriteTexAtlas(vec2 position, vec2 size, CLSTtexture *texture_atlas, vec2 offset, vec2 texsize, CLSTsprite *sprite)
{
    sprite->draw = (draw_func)draw_sprite;
    glm_vec2_copy(position, sprite->position);
    glm_vec2_copy(size, sprite->size);

    glm_vec2_copy((float[]){
            (offset[0]       * texsize[0]) / texture_atlas->width,
            (offset[1]       * texsize[1]) / texture_atlas->height
    }, sprite->uv[0]);

    glm_vec2_copy((float[]){
            (offset[0]       * texsize[0]) / texture_atlas->width,
            ((offset[1] + 3) * texsize[1]) / texture_atlas->height
    }, sprite->uv[1]);

    glm_vec2_copy((float[]){
            ((offset[0] + 3) * texsize[0]) / texture_atlas->width,
            ((offset[1] + 3) * texsize[1]) / texture_atlas->height
    }, sprite->uv[2]);

    glm_vec2_copy((float[]){
            ((offset[0] + 3) * texsize[0]) / texture_atlas->width,
            (offset[1]       * texsize[1]) / texture_atlas->height
    }, sprite->uv[3]);

    sprite->texture = texture_atlas;
    sprite->color = 0xFFFFFFFF;
}

void clstSpriteCol(vec2 position, vec2 size, unsigned int color, CLSTsprite *sprite)
{
    sprite->draw = (draw_func)draw_sprite;
    glm_vec2_copy(position, sprite->position);
    glm_vec2_copy(size, sprite->size);
    default_uv(sprite->uv);
    sprite->texture = NULL;
    sprite->color = color;
}

void clstLabel(vec2 position, char *text, CLSTfont *font, CLSTlabel *label)
{
    label->draw = (draw_func)draw_label;
    glm_vec2_copy(position, label->position);
    default_uv(label->uv);
    label->font = font;
    label->text = text;
    label->color = 0xFFFFFFFF;
}

void clstLabelCol(vec2 position, char *text, CLSTfont *font, unsigned int color, CLSTlabel *label)
{
    label->draw = (draw_func)draw_label;
    glm_vec2_copy(position, label->position);
    default_uv(label->uv);
    label->font = font;
    label->text = text;
    label->color = color;
}

CLSTanimation *clstAnimation(CLSTsprite **frames, int frames_count, double frame_time)
{
    CLSTanimation *animation;

    animation = malloc(sizeof(CLSTanimation));
    animation->draw = (draw_func)draw_animation;
    animation->frames_count = frames_count;
    animation->current_frame = 0;
    animation->frames = malloc(frames_count * sizeof(CLSTsprite *));
    animation->frame_time = frame_time;
    animation->last_animation_time = 0.0;

    for (int i = 0; i < frames_count; i++)
        animation->frames[i] = frames[i];

    return animation;
}

void clstAnimationDestroy(CLSTanimation *animation)
{
    free(animation->frames);
    free(animation);
}

CLSTgroup *clstGroup(vec2 position)
{
    CLSTgroup *group = malloc(sizeof(CLSTgroup));
    group->draw = (draw_func)draw_group;
    group->sprites_count = 0;
    group->sprites = malloc(0);
    glm_translate_make(group->translation, (vec3){ position[0], position[1], 0.0f });
    return group;
}

void clstGroupDestroy(CLSTgroup *group)
{
    free(group->sprites);
    free(group);
}

void clstGroupAddSprite(CLSTgroup *group, void *sprite)
{
    group->sprites = realloc(group->sprites, (group->sprites_count + 1) * sizeof(CLSTsprite *));
    group->sprites[group->sprites_count] = sprite;
    group->sprites_count++;
}

void clstButton(CLSTsprite *sprite, CLSTbutton *button)
{
    button->draw = (draw_func)draw_button;
    button->status = BUTTON_STATUS_NONE;
    button->sprite = sprite;
    glm_mat4_zero(button->translation);
}

void clstButtonEv(CLSTbutton *button, float projection_x, float projection_y)
{
    CLST *celeste;
    vec2 cursor;
    vec3 pos;

    celeste = clstInstance();
    cursor[0] = (float)(celeste->window.cursor.x * projection_x / celeste->window.width - (projection_x * 0.5f));
    cursor[1] = (float)((projection_y * 0.5f) - celeste->window.cursor.y * projection_y / celeste->window.height);

    glm_mat4_mulv3(button->translation, (vec3){ button->sprite->position[0], button->sprite->position[1], 0.0f }, 1.0f, pos);
    if (clstCollisionRectanglePoint((vec2 *)&pos, &(button->sprite->size), (vec2){ cursor[0], cursor[1] }))
    {
        if (button->status == BUTTON_STATUS_FOCUSED)
        {
            if (clstClick(CELESTE_MOUSE_LEFT)) {
                button->status = BUTTON_STATUS_CLICKED;
            }
        }
        else { /* First frame of the cursor colliding with the button */
            if (clstClick(CELESTE_MOUSE_LEFT)) {
                button->status = BUTTON_STATUS_NONE;
            }
            else {
                button->status = BUTTON_STATUS_FOCUSED;
            }
        }
    }
    else {
        button->status = BUTTON_STATUS_NONE;
    }
}
