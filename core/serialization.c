#include "internal/gapi.h"
#include "internal/celeste.h"

#include "serialization.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *clstSerializeSprite(CLSTsprite *sprite, uint32_t *out_data_size)
{
    void *data;
    uint32_t data_size, name_len, texname_len;

    if (sprite->texture == NULL)
        texname_len = 1;
    else
        texname_len = strlen(sprite->texture->name) + 1;

    name_len = strlen(sprite->name) + 1;
    data_size = (sizeof(vec2) * 6) + sizeof(uint32_t) + name_len + texname_len;
    data = malloc(data_size);
    
    memcpy(data, sprite->name, name_len);
    memcpy(data + name_len, sprite->position, sizeof(vec2));
    memcpy(data + name_len + (sizeof(vec2) * 1), sprite->size, sizeof(vec2));
    memcpy(data + name_len + (sizeof(vec2) * 2), sprite->uv[0], sizeof(vec2));
    memcpy(data + name_len + (sizeof(vec2) * 3), sprite->uv[1], sizeof(vec2));
    memcpy(data + name_len + (sizeof(vec2) * 4), sprite->uv[2], sizeof(vec2));
    memcpy(data + name_len + (sizeof(vec2) * 5), sprite->uv[3], sizeof(vec2));
    memcpy(data + name_len + (sizeof(vec2) * 6), &(sprite->color), sizeof(uint32_t));
    memcpy(data + name_len + (sizeof(vec2) * 6) + sizeof(uint32_t), sprite->texture == NULL ? "" : sprite->texture->name, texname_len);

    *out_data_size = data_size;
    return data;
}

CLSTsprite *clstDeserializeSprite(void *data, uint32_t *data_size)
{    
    CLSTsprite *sprite;
    uint32_t name_len, texname_len, color;
    char *texname, *name;
    vec2 position, size, uv[4];
    CLSTtexture *tex;

    name = data;
    name_len = strlen(name) + 1;

    memcpy(position, (data + name_len), sizeof(vec2));
    memcpy(size    , (data + name_len + sizeof(vec2)), sizeof(vec2));
    memcpy(uv[0]   , (data + name_len + (sizeof(vec2) * 2)), sizeof(vec2));
    memcpy(uv[1]   , (data + name_len + (sizeof(vec2) * 3)), sizeof(vec2));
    memcpy(uv[2]   , (data + name_len + (sizeof(vec2) * 4)), sizeof(vec2));
    memcpy(uv[3]   , (data + name_len + (sizeof(vec2) * 5)), sizeof(vec2));
    memcpy(&color  , (data + name_len + (sizeof(vec2) * 6)), sizeof(uint32_t));
    texname = (char *)(data + name_len + sizeof(uint32_t) + (sizeof(vec2) * 6));
    texname_len = strlen(texname) + 1;
    *data_size = name_len + sizeof(uint32_t) + (sizeof(vec2) * 6) + texname_len;

    tex = texname_len == 1 ? NULL : clstSceneGetTexture(clstInstance()->scene, texname);
    sprite = clstSprite(position, size, tex, name);
    sprite->color = color;
    memcpy(sprite->uv[0], uv[0], sizeof(vec2));
    memcpy(sprite->uv[1], uv[1], sizeof(vec2));
    memcpy(sprite->uv[2], uv[2], sizeof(vec2));
    memcpy(sprite->uv[3], uv[3], sizeof(vec2));
    return sprite;
}

void *clstSerializeLabel(CLSTlabel *label, uint32_t *out_data_size)
{
    void *data;
    uint32_t data_size, name_len, text_len, fontname_len;

    name_len = strlen(label->name) + 1;
    text_len = strlen(label->text) + 1;
    fontname_len = strlen(label->font->name) + 1;
    data_size = (sizeof(vec2) * 5) + sizeof(uint32_t) + name_len + text_len + fontname_len;
    data = malloc(data_size);
    
    memcpy(data, label->name, name_len);
    memcpy(data + name_len, label->position, sizeof(vec2));
    memcpy(data + name_len + (sizeof(vec2) * 1), label->uv[0], sizeof(vec2));
    memcpy(data + name_len + (sizeof(vec2) * 2), label->uv[1], sizeof(vec2));
    memcpy(data + name_len + (sizeof(vec2) * 3), label->uv[2], sizeof(vec2));
    memcpy(data + name_len + (sizeof(vec2) * 4), label->uv[3], sizeof(vec2));
    memcpy(data + name_len + (sizeof(vec2) * 5), &(label->color), sizeof(uint32_t));
    memcpy(data + name_len + (sizeof(vec2) * 5) + sizeof(uint32_t), label->text, text_len);
    memcpy(data + name_len + text_len + (sizeof(vec2) * 5) + sizeof(uint32_t), label->font->name, fontname_len);

    *out_data_size = data_size;
    return data;
}

CLSTlabel *clstDeserializeLabel(void *data, uint32_t *data_size)
{    
    CLSTlabel *label;
    uint32_t name_len, text_len, fontname_len, color;
    char *name, *text, *fontname;
    vec2 position, uv[4];

    name = data;
    name_len = strlen(name) + 1;

    memcpy(position, (data + name_len), sizeof(vec2));
    memcpy(uv[0]   , (data + name_len + (sizeof(vec2) * 1)), sizeof(vec2));
    memcpy(uv[1]   , (data + name_len + (sizeof(vec2) * 2)), sizeof(vec2));
    memcpy(uv[2]   , (data + name_len + (sizeof(vec2) * 3)), sizeof(vec2));
    memcpy(uv[3]   , (data + name_len + (sizeof(vec2) * 4)), sizeof(vec2));
    memcpy(&color  , (data + name_len + (sizeof(vec2) * 5)), sizeof(uint32_t));

    text = (char *)(data + name_len + sizeof(uint32_t) + (sizeof(vec2) * 5));
    text_len = strlen(text) + 1;
    fontname = (char *)(data + name_len + text_len + sizeof(uint32_t) + (sizeof(vec2) * 5));
    fontname_len = strlen(fontname) + 1;
    *data_size = name_len + sizeof(uint32_t) + (sizeof(vec2) * 5) + text_len + fontname_len;

    label = clstLabelCol(position, text, clstSceneGetFont(clstInstance()->scene, fontname), color, name);
    memcpy(label->uv[0], uv[0], sizeof(vec2));
    memcpy(label->uv[1], uv[1], sizeof(vec2));
    memcpy(label->uv[2], uv[2], sizeof(vec2));
    memcpy(label->uv[3], uv[3], sizeof(vec2));
    return label;
}

void *clstSerializeAnimation(CLSTanimation *animation, uint32_t *out_data_size)
{
    void *data;
    uint32_t data_size, offset, name_len;

    name_len = strlen(animation->name) + 1;
    data_size = sizeof(uint32_t) + sizeof(double) + name_len;
    data = malloc(data_size);
    
    memcpy(data, animation->name, name_len);
    memcpy(data + name_len, &(animation->frames->count), sizeof(uint32_t));
    memcpy(data + name_len + sizeof(uint32_t), &(animation->frame_time), sizeof(double));

    offset = data_size;
    for (int i = 0; i < animation->frames->count; i++)
    {
        void *sprite_data;
        uint32_t sprite_size;

        sprite_data = clstSerializeSprite(((CLSTsprite **)animation->frames->items)[i], &sprite_size);
        data_size += sprite_size;
        data = realloc(data, data_size);
        memcpy(data + offset, sprite_data, sprite_size);
        free(sprite_data);

        offset = data_size;
    }

    *out_data_size = data_size;
    return data;
}

CLSTanimation *clstDeserializeAnimation(void *data, uint32_t *data_size)
{
    uint32_t name_len, frames_count, offset;
    double frame_time;
    char *name;

    name = data;
    name_len = strlen(name) + 1;
    memcpy(&frames_count, (data + name_len), sizeof(uint32_t));
    memcpy(&frame_time  , (data + name_len + sizeof(uint32_t)), sizeof(double));

    CLSTsprite *frames[frames_count];
    offset = name_len + sizeof(uint32_t) + sizeof(double);
    for (int i = 0; i < frames_count; i++)
    {
        uint32_t sprite_size;
        frames[i] = clstDeserializeSprite(data + offset, &sprite_size);
        offset += sprite_size;
    }

    *data_size = offset;
    return clstAnimation(frames, frames_count, frame_time, name);
}

void *clstSerializeButton(CLSTbutton *button, uint32_t *out_data_size)
{
    void *data, *sprite_data;
    uint32_t data_size, name_len, sprite_size;

    name_len = strlen(button->name) + 1;
    sprite_data = clstSerializeSprite(button->sprite, &sprite_size);
    data_size = (sizeof(uint32_t) * 2) + sizeof(mat4) + sprite_size + name_len;
    data = malloc(data_size);
    
    memcpy(data, button->name, name_len);
    memcpy(data + name_len, &(button->status), sizeof(uint32_t));
    memcpy(data + name_len + (sizeof(uint32_t)), &(button->disabled), sizeof(uint32_t));
    memcpy(data + name_len + (sizeof(uint32_t) * 2), button->translation, sizeof(mat4));
    memcpy(data + name_len + (sizeof(uint32_t) * 2) + sizeof(mat4), sprite_data, sprite_size);

    free(sprite_data);
    *out_data_size = data_size;
    return data;
}

CLSTbutton *clstDeserializeButton(void *data, uint32_t *data_size)
{    
    CLSTsprite *sprite;
    char *name;
    uint32_t name_len, sprite_size, status, disabled;
    mat4 translation;

    name = data;
    name_len = strlen(name) + 1;

    memcpy(&status  , (data + name_len), sizeof(uint32_t));
    memcpy(&disabled, (data + name_len + sizeof(uint32_t)), sizeof(uint32_t));
    memcpy(translation, (data + name_len + (sizeof(uint32_t) * 2)), sizeof(mat4));
    sprite = clstDeserializeSprite((data + name_len + (sizeof(uint32_t) * 2) + sizeof(mat4)), &sprite_size);
    *data_size = name_len + (sizeof(uint32_t) * 2) + sizeof(mat4) + sprite_size;
    
    return clstButtonCustom(status, disabled, sprite, translation, name);
}

void *clstSerializeGroup(CLSTgroup *group, uint32_t *out_data_size)
{
    void *data;
    uint32_t data_size, name_len, offset;

    name_len = strlen(group->name) + 1;
    data_size = name_len + sizeof(uint32_t) + sizeof(mat4);
    data = malloc(data_size);
    memcpy(data, group->name, name_len);
    memcpy(data + name_len, &(group->renderables->count), sizeof(uint32_t));
    memcpy(data + name_len + sizeof(uint32_t), group->translation, sizeof(mat4));
    offset = data_size;

    for (int i = 0; i < group->renderables->count; i++)
    {
        CLSTrenderable *renderable;
        CLSTrenderabletype type;
        uint32_t renderable_size;
        void *renderable_data;

        renderable = (CLSTrenderable *)group->renderables->items[i];
        type = clstRenderableType(renderable);

        switch (type)
        {
            case CELESTE_RENDERABLE_SPRITE:
                renderable_data = clstSerializeSprite((CLSTsprite *)renderable, &renderable_size);
                break;
            case CELESTE_RENDERABLE_LABEL:
                renderable_data = clstSerializeLabel((CLSTlabel *)renderable, &renderable_size);
                break;
            case CELESTE_RENDERABLE_ANIMATION:
                renderable_data = clstSerializeAnimation((CLSTanimation *)renderable, &renderable_size);
                break;
            case CELESTE_RENDERABLE_GROUP:
                renderable_data = clstSerializeGroup((CLSTgroup *)renderable, &renderable_size);
                break;
            case CELESTE_RENDERABLE_BUTTON:
                renderable_data = clstSerializeButton((CLSTbutton *)renderable, &renderable_size);
                break;
            default:
                CELESTE_LOG_ERROR("Unimplemented renderable serialization!\n");
                clstTerminate();
                exit(1);
        }

        data_size += renderable_size + sizeof(uint32_t);
        data = realloc(data, data_size);
        memcpy(data + offset, &type, sizeof(uint32_t));
        memcpy(data + offset + sizeof(uint32_t), renderable_data, renderable_size);
        free(renderable_data);
        offset = data_size;
    }
    *out_data_size = data_size;
    return data;
}

CLSTgroup *clstDeserializeGroup(void *data, uint32_t *data_size)
{
    CLSTgroup *group;
    mat4 translation;
    char *name;
    uint32_t name_len, renderables_count, group_size;

    name = (char *)data;
    name_len = strlen(name) + 1;
    renderables_count = *(uint32_t *)(data + name_len);
    memcpy(translation, data + name_len + sizeof(uint32_t), sizeof(mat4));
    group = clstGroupMat4(translation, name);

    group_size = name_len + sizeof(uint32_t) + sizeof(mat4);
    for (int i = 0; i < renderables_count; i++)
    {
        CLSTrenderable *renderable;
        uint32_t renderable_size, renderable_type;

        renderable_size = 0; /* Compiler warning. */
        renderable_type = *((uint32_t *)(data + group_size));
        group_size += sizeof(uint32_t);
        switch (renderable_type)
        {
            case CELESTE_RENDERABLE_SPRITE:
                renderable = (CLSTrenderable *)clstDeserializeSprite(data + group_size, &renderable_size);
                break;
            case CELESTE_RENDERABLE_LABEL:
                renderable = (CLSTrenderable *)clstDeserializeLabel(data + group_size, &renderable_size);
                break;
            case CELESTE_RENDERABLE_ANIMATION:
                renderable = (CLSTrenderable *)clstDeserializeAnimation(data + group_size, &renderable_size);
                break;
            case CELESTE_RENDERABLE_GROUP:
                renderable = (CLSTrenderable *)clstDeserializeGroup(data + group_size, &renderable_size);
                break;
            case CELESTE_RENDERABLE_BUTTON:
                renderable = (CLSTrenderable *)clstDeserializeButton(data + group_size, &renderable_size);
                break;
            default:
                CELESTE_LOG_ERROR("Unimplemented renderable deserialization!\n");
                clstTerminate();
                exit(1);
        }

        clstGroupAddRenderable(group, renderable);
        group_size += renderable_size;
    }

    *data_size = group_size;
    return group;
}

void *clstSerializeBody(CLSTbody *body, uint32_t *out_data_size)
{
    void *data;
    uint32_t data_size, name_len;

    name_len = strlen(body->name) + 1;
    data_size = (sizeof(uint32_t) * 2) + (sizeof(vec2) * 3) + name_len;
    data = malloc(data_size);

    memcpy(data, body->name, name_len);
    memcpy(data + name_len, &(body->layer), sizeof(uint32_t));
    memcpy(data + name_len + (sizeof(uint32_t)), &(body->flags), sizeof(uint32_t));
    memcpy(data + name_len + (sizeof(uint32_t) * 2), body->position, sizeof(vec2));
    memcpy(data + name_len + (sizeof(uint32_t) * 2) + sizeof(vec2), body->size, sizeof(vec2));
    memcpy(data + name_len + (sizeof(uint32_t) * 2) + (sizeof(vec2) * 2), body->velocity, sizeof(vec2));

    *out_data_size = data_size;
    return data;
}

CLSTbody *clstDeserializeBody(void *data)
{
    CLSTbody *body;
    char *name;
    uint32_t name_len, layer, flags;
    vec2 position, size, velocity;

    name = data;
    name_len = strlen(name) + 1;

    memcpy(&layer  , (data + name_len), sizeof(uint32_t));
    memcpy(&flags  , (data + name_len + sizeof(uint32_t)), sizeof(uint32_t));
    memcpy(position, (data + name_len + (sizeof(uint32_t) * 2)), sizeof(vec2));
    memcpy(size    , (data + name_len + (sizeof(uint32_t) * 2) + sizeof(vec2)), sizeof(vec2));
    memcpy(velocity, (data + name_len + (sizeof(uint32_t) * 2) + (sizeof(vec2) * 2)), sizeof(vec2));

    body = clstBody(layer, flags, position, size, name);
    glm_vec2_copy(velocity, body->velocity);
    return body;
}