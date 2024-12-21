#include "internal/gapi.h"
#include "internal/renderer.h"
#include "internal/celeste.h"

#include "renderer.h"
#include "sprite.h"
#include "input.h"
#include <stdlib.h>
#include <string.h>
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

    time = clstGetTime();
    if (time >= animation->last_animation_time + animation->frame_time) {
        if (animation->current_frame < animation->frames->count - 1)
            animation->current_frame++;
        else
            animation->current_frame = 0;
        animation->last_animation_time = time;
    }

    sprite = ((CLSTsprite **)animation->frames->items)[animation->current_frame];
    clstRendererDrawQuad(renderer, sprite->position, sprite->size, sprite->texture, sprite->uv, sprite->color);
}

static void draw_group(CLSTrenderer *renderer, CLSTgroup *group)
{
    CLSTsprite **sprites;
    CLSTsprite *sprite;

    sprites = (CLSTsprite **)group->renderables->items;
    clstRendererMat4Push(renderer, group->translation);
    for (int i = 0; i < group->renderables->count; i++) {
        sprite = sprites[i];
        sprite->draw(renderer, sprite);
    }
    clstRendererMat4Pop(renderer);
}

static void draw_button(CLSTrenderer *renderer, CLSTbutton *button)
{
    CLSTsprite *sprite;

    sprite = button->sprite;
    sprite->draw(renderer, sprite);

    if (button->disabled)
        return;

    glm_mat4_copy(renderer->transformation_back[renderer->transformation_back_size - 1], button->translation);
    clstButtonEv(button, renderer->projection_x, renderer->projection_y);
}

CLSTsprite *clstSprite(vec2 position, vec2 size, CLSTtexture *texture, char *name)
{
    CLSTsprite *sprite;

    sprite = malloc(sizeof(CLSTsprite));
    sprite->draw = (CLSTdrawfunc)draw_sprite;
    sprite->name = strdup(name);
    glm_vec2_copy(position, sprite->position);
    glm_vec2_copy(size, sprite->size);
    default_uv(sprite->uv);
    sprite->texture = texture;
    sprite->color = 0xFFFFFFFF;
    return sprite;
}

CLSTsprite *clstSpriteTexAtlaspx(vec2 position, vec2 size, CLSTtexture *texture_atlas, vec2 startpx, vec2 endpx, char *name)
{
    CLSTsprite *sprite;
    float u1, v1, u2, v2;

    sprite = malloc(sizeof(CLSTsprite));
    sprite->draw = (CLSTdrawfunc)draw_sprite;
    sprite->name = strdup(name);
    glm_vec2_copy(position, sprite->position);
    glm_vec2_copy(size, sprite->size);

    u1 = startpx[0] / texture_atlas->width;
    v1 = startpx[1] / texture_atlas->height;
    u2 = endpx[0] / texture_atlas->width;
    v2 = endpx[1] / texture_atlas->height;

    glm_vec2_copy((float[]){u1, v1}, sprite->uv[0]);
    glm_vec2_copy((float[]){u1, v2}, sprite->uv[1]);
    glm_vec2_copy((float[]){u2, v2}, sprite->uv[2]);
    glm_vec2_copy((float[]){u2, v1}, sprite->uv[3]);

    sprite->texture = texture_atlas;
    sprite->color = 0xFFFFFFFF;
    return sprite;
}

CLSTsprite *clstSpriteTexAtlas(vec2 position, vec2 size, CLSTtexture *texture_atlas, vec2 offset, vec2 texsize, char *name)
{
    CLSTsprite *sprite;

    sprite = malloc(sizeof(CLSTsprite));
    sprite->draw = (CLSTdrawfunc)draw_sprite;
    sprite->name = strdup(name);
    glm_vec2_copy(position, sprite->position);
    glm_vec2_copy(size, sprite->size);

    glm_vec2_copy((float[]){
            (offset[0]       * texsize[0]) / texture_atlas->width + 0.0001f,
            (offset[1]       * texsize[1]) / texture_atlas->height + 0.0001f
    }, sprite->uv[0]);

    glm_vec2_copy((float[]){
            (offset[0]       * texsize[0]) / texture_atlas->width + 0.0001f,
            ((offset[1] + 1) * texsize[1]) / texture_atlas->height - 0.0001f
    }, sprite->uv[1]);

    glm_vec2_copy((float[]){
            ((offset[0] + 1) * texsize[0]) / texture_atlas->width - 0.0001f,
            ((offset[1] + 1) * texsize[1]) / texture_atlas->height - 0.0001f
    }, sprite->uv[2]);

    glm_vec2_copy((float[]){
            ((offset[0] + 1) * texsize[0]) / texture_atlas->width - 0.0001f,
            (offset[1]       * texsize[1]) / texture_atlas->height + 0.0001f
    }, sprite->uv[3]);

    sprite->texture = texture_atlas;
    sprite->color = 0xFFFFFFFF;
    return sprite;
}

CLSTsprite *clstSpriteCol(vec2 position, vec2 size, uint32_t color, char *name)
{
    CLSTsprite *sprite;

    sprite = malloc(sizeof(CLSTsprite));
    sprite->draw = (CLSTdrawfunc)draw_sprite;
    sprite->name = strdup(name);
    glm_vec2_copy(position, sprite->position);
    glm_vec2_copy(size, sprite->size);
    default_uv(sprite->uv);
    sprite->texture = NULL;
    sprite->color = color;
    return sprite;
}

void clstSpriteFlipX(CLSTsprite *sprite)
{
    float u1, u2;

    u1 = sprite->uv[0][0];
    u2 = sprite->uv[2][0];

    sprite->uv[0][0] = u2;
    sprite->uv[1][0] = u2;
    sprite->uv[2][0] = u1;
    sprite->uv[3][0] = u1;
}

void clstSpriteFlipY(CLSTsprite *sprite)
{
    float v1, v2;

    v1 = sprite->uv[0][1];
    v2 = sprite->uv[1][1];

    sprite->uv[0][1] = v2;
    sprite->uv[1][1] = v1;
    sprite->uv[2][1] = v1;
    sprite->uv[3][1] = v2;
}

void clstSpriteDestroy(CLSTsprite *sprite)
{
    free(sprite->name);
    free(sprite);
}

CLSTlabel *clstLabel(vec2 position, char *text, CLSTfont *font, char *name)
{
    CLSTlabel *label;

    label = malloc(sizeof(CLSTlabel));
    label->draw = (CLSTdrawfunc)draw_label;
    label->name = strdup(name);
    glm_vec2_copy(position, label->position);
    default_uv(label->uv);
    label->font = font;
    label->text = strdup(text);
    label->color = 0xFFFFFFFF;
    return label;
}

CLSTlabel *clstLabelCol(vec2 position, char *text, CLSTfont *font, uint32_t color, char *name)
{
    CLSTlabel *label;

    label = malloc(sizeof(CLSTlabel));
    label->draw = (CLSTdrawfunc)draw_label;
    label->name = strdup(name);
    glm_vec2_copy(position, label->position);
    default_uv(label->uv);
    label->font = font;
    label->text = strdup(text);
    label->color = color;
    return label;
}

void clstLabelSetText(CLSTlabel *label, char *text)
{
    free(label->text);
    label->text = strdup(text);
}

void clstLabelDestroy(CLSTlabel *label)
{
    free(label->text);
    free(label->name);
    free(label);
}

CLSTanimation *clstAnimation(CLSTsprite **frames, uint32_t frames_count, double frame_time, char *name)
{
    CLSTanimation *animation;

    animation = malloc(sizeof(CLSTanimation));
    animation->draw = (CLSTdrawfunc)draw_animation;
    animation->name = strdup(name);
    animation->current_frame = 0;
    animation->frames = clstListCreate();
    animation->frame_time = frame_time;
    animation->last_animation_time = 0.0;

    for (int i = 0; i < frames_count; i++)
        clstListAdd(animation->frames, frames[i]);

    return animation;
}

void clstAnimationDestroy(CLSTanimation *animation)
{
    clstListDestroy(animation->frames, (CLSTitemdestroy)clstSpriteDestroy);
    free(animation->name);
    free(animation);
}

CLSTgroup *clstGroup(vec2 position, char *name)
{
    CLSTgroup *group = malloc(sizeof(CLSTgroup));
    group->draw = (CLSTdrawfunc)draw_group;
    group->name = strdup(name);
    group->renderables = clstListCreate();
    glm_translate_make(group->translation, (vec3){ position[0], position[1], 0.0f });
    return group;
}

CLSTgroup *clstGroupMat4(mat4 translation, char *name)
{
    CLSTgroup *group = malloc(sizeof(CLSTgroup));
    group->draw = (CLSTdrawfunc)draw_group;
    group->name = strdup(name);
    group->renderables = clstListCreate();
    glm_mat4_copy(translation, group->translation);
    return group;
}

void clstGroupDestroy(CLSTgroup *group)
{
    clstListDestroy(group->renderables, (CLSTitemdestroy)clstRenderableDestroy);
    free(group->name);
    free(group);
}

void clstGroupPosition(CLSTgroup *group, vec2 position)
{
    glm_translate_make(group->translation, (vec3){ position[0], position[1], 0.0f });
}

void clstGroupAddRenderable(CLSTgroup *group, void *renderable)
{
    clstListAdd(group->renderables, renderable);
}

CLSTbutton *clstButton(CLSTsprite *sprite, char *name)
{
    CLSTbutton *button;

    button = malloc(sizeof(CLSTbutton));
    button->draw = (CLSTdrawfunc)draw_button;
    button->name = strdup(name);
    button->status = CELESTE_BUTTON_STATUS_NONE;
    button->disabled = 0;
    button->sprite = sprite;
    glm_mat4_zero(button->translation);
    return button;
}

CLSTbutton *clstButtonCustom(uint32_t status, uint32_t disabled, CLSTsprite *sprite, mat4 translation, char *name)
{
    CLSTbutton *button;

    button = malloc(sizeof(CLSTbutton));
    button->draw = (CLSTdrawfunc)draw_button;
    button->name = strdup(name);
    button->status = status;
    button->disabled = disabled;
    button->sprite = sprite;
    glm_mat4_copy(translation, button->translation);
    return button;
}

void clstButtonDestroy(CLSTbutton *button)
{
    clstSpriteDestroy(button->sprite);
    free(button->name);
    free(button);
}

void clstRenderableDestroy(CLSTrenderable *renderable)
{
    switch (clstRenderableType(renderable))
    {
        case CELESTE_RENDERABLE_SPRITE:
            clstSpriteDestroy((CLSTsprite *)renderable);
            break;
        case CELESTE_RENDERABLE_LABEL:
            clstLabelDestroy((CLSTlabel *)renderable);
            break;
        case CELESTE_RENDERABLE_ANIMATION:
        clstAnimationDestroy((CLSTanimation *)renderable);
            break;
        case CELESTE_RENDERABLE_GROUP:
            clstGroupDestroy((CLSTgroup *)renderable);
            break;
        case CELESTE_RENDERABLE_BUTTON:
            clstButtonDestroy((CLSTbutton *)renderable);
            break;
    }
}

CLSTrenderabletype clstRenderableType(CLSTrenderable *renderable)
{
    if (renderable->draw == (CLSTdrawfunc)draw_sprite)
        return CELESTE_RENDERABLE_SPRITE;
    if (renderable->draw == (CLSTdrawfunc)draw_label)
        return CELESTE_RENDERABLE_LABEL;
    if (renderable->draw == (CLSTdrawfunc)draw_animation)
        return CELESTE_RENDERABLE_ANIMATION;
    if (renderable->draw == (CLSTdrawfunc)draw_group)
        return CELESTE_RENDERABLE_GROUP;
    if (renderable->draw == (CLSTdrawfunc)draw_button)
        return CELESTE_RENDERABLE_BUTTON;
    
    CELESTE_LOG_ERROR("Unimplemented renderable type!");
    return -1;
}

void clstButtonEv(CLSTbutton *button, float projection_x, float projection_y)
{
    CLST *clst;
    vec2 cursor;
    vec3 pos;

    clst = clstInstance();
    cursor[0] = (float)(clst->window.cursor.x * projection_x / clst->window.width - (projection_x * 0.5f));
    cursor[1] = (float)((projection_y * 0.5f) - clst->window.cursor.y * projection_y / clst->window.height);

    glm_mat4_mulv3(button->translation, (vec3){ button->sprite->position[0], button->sprite->position[1], 0.0f }, 1.0f, pos);
    if (!clstCollisionRectanglePoint(pos, button->sprite->size, (vec2){ cursor[0], cursor[1] }))
    {
        button->status = CELESTE_BUTTON_STATUS_NONE;
        return;
    }

    if (button->status == CELESTE_BUTTON_STATUS_NONE)
    {
        if (clstClick(CELESTE_MOUSE_LEFT)) {
            button->status = CELESTE_BUTTON_STATUS_NONE;
        }
        else {
            button->status = CELESTE_BUTTON_STATUS_FOCUSED;
        }
    }
    else if (button->status == CELESTE_BUTTON_STATUS_FOCUSED)
    {
        if (clstClick(CELESTE_MOUSE_LEFT)) {
            button->status = CELESTE_BUTTON_STATUS_CLICKED;
        }
    }
    else if (button->status == CELESTE_BUTTON_STATUS_CLICKED)
    {
        if (clstClick(CELESTE_MOUSE_LEFT)) {
            button->status = CELESTE_BUTTON_STATUS_CLICKED;
        }
        else {
            button->status = CELESTE_BUTTON_STATUS_RELEASED;
        }
    }
    else {
        button->status = CELESTE_BUTTON_STATUS_NONE;
    }
}
