#include "internal/gapi.h"
#include "loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static int clstLoadableExists(char *name, void *data, int data_size, uint8_t type);

static void *memory_dupe(void *src, uint32_t data_size)
{
    void *data;
    data = malloc(data_size);
    memcpy(data, src, data_size);
    return data;
}

static void *clstSerializeSprite(CLSTsprite *sprite, uint32_t *out_data_size)
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

static CLSTsprite *clstDeserializeSprite(void *data, uint32_t *data_size)
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

static void *clstSerializeLabel(CLSTlabel *label, uint32_t *out_data_size)
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

static CLSTlabel *clstDeserializeLabel(void *data, uint32_t *data_size)
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
    *data_size = name_len + sizeof(uint32_t) + (sizeof(vec2) * 6) + text_len + fontname_len;

    label = clstLabelCol(position, text, clstSceneGetFont(clstInstance()->scene, fontname), color, name);
    memcpy(label->uv[0], uv[0], sizeof(vec2));
    memcpy(label->uv[1], uv[1], sizeof(vec2));
    memcpy(label->uv[2], uv[2], sizeof(vec2));
    memcpy(label->uv[3], uv[3], sizeof(vec2));
    return label;
}

static void *clstSerializeButton(CLSTbutton *button, uint32_t *out_data_size)
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

static CLSTbutton *clstDeserializeButton(void *data, uint32_t *data_size)
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

static void *clstSerializeBody(CLSTbody *body, uint32_t *out_data_size)
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

static CLSTbody *clstDeserializeBody(void *data)
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

CLSTtexture *clstTextureSave(char *filepath, char *name)
{
    void *data;
    uint32_t data_size;

    data_size = strlen(filepath) + 1;
    data = memory_dupe(filepath, data_size);
    clstLoadable(name, data, data_size, CELESTE_TEXTURE_FILE);
    free(data);
    return clstTexture(filepath, name);
}

CLSTtexture *clstTextureMemSave(uint8_t *buffer, uint32_t bufsize, char *name)
{
    void *data;
    data = memory_dupe(buffer, bufsize);
    clstLoadable(name, buffer, bufsize, CELESTE_TEXTURE_BIN);
    free(data);
    return clstTextureMem(buffer, bufsize, name);
}

CLSTfont *clstFontSave(char *filepath, float size, char *name)
{
    void *data;
    uint32_t data_size;

    data_size = strlen(filepath) + 1 + sizeof(float);
    data = malloc(data_size);
    memcpy(data, &size, sizeof(float));
    memcpy(data + sizeof(float), filepath, data_size - sizeof(float));
    clstLoadable(name, data, data_size, CELESTE_FONT_FILE);
    free(data);
    return clstFont(filepath, size, name);
}

CLSTfont *clstFontMemSave(uint8_t *buffer, uint32_t bufsize, float size, char *name)
{
    void *data;
    data = memory_dupe(buffer, bufsize);
    clstLoadable(name, buffer, bufsize, CELESTE_FONT_BIN);
    free(data);
    return clstFontMem(buffer, bufsize, size, name);
}

CLSTaudio *clstAudioSave(const char *filepath, char *name)
{
    void *data;
    uint32_t data_size;

    data_size = strlen(filepath) + 1;
    data = memory_dupe((void *)filepath, data_size);
    clstLoadable(name, data, data_size, CELESTE_AUDIO_FILE);
    free(data);
    return clstAudio(filepath, name);
}

CLSTaudio *clstAudioMemSave(unsigned char *buffer, unsigned int bufsize, char *name)
{
    void *data;
    data = memory_dupe(buffer, bufsize);
    clstLoadable(name, buffer, bufsize, CELESTE_AUDIO_BIN);
    free(data);
    return clstAudioMem(buffer, bufsize, name);
}

CLSTsprite *clstSpriteSave(vec2 position, vec2 size, CLSTtexture *texture, char *name)
{
    void *data;
    uint32_t data_size, texname_len;

    texname_len = strlen(texture->name) + 1;
    data_size = (sizeof(vec2) * 2) + texname_len;
    data = malloc(data_size);

    memcpy(data, position, sizeof(vec2));
    memcpy(data + sizeof(vec2), size, sizeof(vec2));
    memcpy(data + (sizeof(vec2) * 2), texture->name, texname_len);
    clstLoadable(name, data, data_size, CELESTE_SPRITE);
    free(data);
    return clstSprite(position, size, texture, name);
}

CLSTsprite *clstSpriteColSave(vec2 position, vec2 size, uint32_t color, char *name)
{
    void *data;
    uint32_t data_size;

    data_size = (sizeof(vec2) * 2) + sizeof(uint32_t);
    data = malloc(data_size);

    memcpy(data, position, sizeof(vec2));
    memcpy(data + sizeof(vec2), size, sizeof(vec2));
    memcpy(data + (sizeof(vec2) * 2), &color, sizeof(uint32_t));
    clstLoadable(name, data, data_size, CELESTE_SPRITE_COLOR);
    free(data);
    return clstSpriteCol(position, size, color, name);
}

CLSTsprite *clstSpriteTexAtlasSave(vec2 position, vec2 size, CLSTtexture *texture_atlas, vec2 offset, vec2 texsize, char *name)
{
    void *data;
    uint32_t data_size, texname_len;

    texname_len = strlen(texture_atlas->name) + 1;
    data_size = (sizeof(vec2) * 4) + texname_len;
    data = malloc(data_size);

    memcpy(data, position, sizeof(vec2));
    memcpy(data + sizeof(vec2), size, sizeof(vec2));
    memcpy(data + (sizeof(vec2) * 2), offset, sizeof(vec2));
    memcpy(data + (sizeof(vec2) * 3), texsize, sizeof(vec2));
    memcpy(data + (sizeof(vec2) * 4), texture_atlas->name, texname_len);
    clstLoadable(name, data, data_size, CELESTE_SPRITE_ATLAS);
    free(data);
    return clstSpriteTexAtlas(position, size, texture_atlas, offset, texsize, name);
}

CLSTanimation *clstAnimationSave(CLSTsprite **frames, uint32_t frames_count, double frame_time, char *name)
{
    void *data;
    uint32_t data_size, offset;

    data_size = sizeof(uint32_t) + sizeof(double);
    data = malloc(data_size);
    memcpy(data, &frames_count, sizeof(uint32_t));
    memcpy(data + sizeof(uint32_t), &frame_time, sizeof(double));
    offset = data_size;

    for (int i = 0; i < frames_count; i++)
    {
        void *sprite_data;
        uint32_t sprite_size;

        sprite_data = clstSerializeSprite(frames[i], &sprite_size);
        data_size += sprite_size;
        data = realloc(data, data_size);
        memcpy(data + offset, sprite_data, sprite_size);
        free(sprite_data);

        offset = data_size;
    }

    clstLoadable(name, data, data_size, CELESTE_ANIMATION);
    free(data);
    return clstAnimation(frames, frames_count, frame_time, name);
}

CLSTlabel *clstLabelSave(vec2 position, char *text, CLSTfont *font, char *name)
{
    void *data;
    uint32_t data_size, text_len, fontname_len;

    fontname_len = strlen(font->name) + 1;
    text_len = strlen(text) + 1;
    data_size = sizeof(vec2) + text_len + fontname_len;
    data = malloc(data_size);

    memcpy(data, position, sizeof(vec2));
    memcpy(data + sizeof(vec2), text, text_len);
    memcpy(data + sizeof(vec2) + text_len, font->name, fontname_len);
    clstLoadable(name, data, data_size, CELESTE_LABEL);
    free(data);
    return clstLabel(position, text, font, name);
}

CLSTbody *clstBodySave(uint32_t layer, uint32_t flags, vec2 position, vec2 size, char *name)
{
    CLSTbody *body;
    void *data;
    uint32_t data_size;

    body = clstBody(layer, flags, position, size, name);
    data = clstSerializeBody(body, &data_size);
    clstLoadable(name, data, data_size, CELESTE_BODY);
    free(data);
    return body;
}

void clstGroupSave(CLSTgroup *group)
{
    void *data;
    uint32_t data_size, offset;

    data_size = sizeof(uint32_t) + sizeof(mat4);
    data = malloc(data_size);
    memcpy(data, &(group->renderables->count), sizeof(uint32_t));
    memcpy(data + sizeof(uint32_t), group->translation, sizeof(mat4));
    offset = data_size;

    for (int i = 0; i < group->renderables->count; i++)
    {
        CLSTrenderable *renderable;
        CLSTrenderabletype type;
        uint32_t renderable_size;
        void *renderable_data;

        renderable = (CLSTrenderable *)group->renderables->items[i];
        type = clstRenderableType(renderable);

        renderable_data = NULL;renderable_size = 0; /* To disable the uninitialized compiler warning */
        switch (type)
        {
            case CELESTE_RENDERABLE_SPRITE:
                renderable_data = clstSerializeSprite((CLSTsprite *)renderable, &renderable_size);
                break;
            case CELESTE_RENDERABLE_LABEL:
                renderable_data = clstSerializeLabel((CLSTlabel *)renderable, &renderable_size);
                break;
            case CELESTE_RENDERABLE_ANIMATION:
                CELESTE_LOG_ERROR("Unimplemented clstAnimationSerialize!");
                break;
            case CELESTE_RENDERABLE_GROUP:
                CELESTE_LOG_ERROR("Unimplemented clstGroupSerialize!");
                break;
            case CELESTE_RENDERABLE_BUTTON:
                renderable_data = clstSerializeButton((CLSTbutton *)renderable, &renderable_size);
                break;
        }

        data_size += renderable_size + sizeof(uint32_t);
        data = realloc(data, data_size);
        memcpy(data + offset, &type, sizeof(uint32_t));
        memcpy(data + offset + sizeof(uint32_t), renderable_data, renderable_size);
        free(renderable_data);
        offset = data_size;
    }

    clstLoadable(group->name, data, data_size, CELESTE_GROUP);
    free(data);
}

CLSTlayer *clstLayerCameraSave(CLSTcamera *camera, float right, float top, char *name)
{
    void *data;
    uint32_t data_size;
    CLSTcameraortho *cam;
    
    cam = camera;
    data_size = sizeof(vec2) + (sizeof(float) * 2);
    data = malloc(data_size);

    memcpy(data, cam->position, sizeof(vec2));
    memcpy(data + sizeof(vec2), &right, sizeof(float));
    memcpy(data + sizeof(vec2) + sizeof(float), &top, sizeof(float));
    clstLoadable(name, data, data_size, CELESTE_LAYER);
    free(data);
    return clstLayerCamera(camera, right, top, name);
}

CLSTloader *clstLoader(char *filepath)
{
    CLST *clst;
    CLSTloader *loader;

    clst = clstInstance();

    if (clst->loader != NULL) {
        // TODO ERROR
        clstTerminate();
        exit(1);
    }

    loader = malloc(sizeof(CLSTloader));
    loader->filepath = filepath;
    loader->loadables = clstListCreate();

    clst->loader = loader;
    return loader;
}

void clstLoaderDestroy(CLSTloader *loader)
{
    clstInstance()->loader = NULL;
    clstListDestroy(loader->loadables, (CLSTitemdestroy)clstLoadableDestroy);
    free(loader);
}

void clstLoaderSaveData(CLSTloader *loader)
{
    FILE *fp;
    const uint32_t start = CELESTE_FILE_MAGIC_NUMBER;

    fp = fopen(loader->filepath, "w");

    fwrite(&start, sizeof(start), 1, fp);

    for (int i = 0; i < loader->loadables->count; i++)
    {
        CLSTloadable *loadable;
        
        loadable = ((CLSTloadable **)loader->loadables->items)[i];
        fwrite(&loadable->type, sizeof(uint8_t), 1, fp);
        fwrite(loadable->name, sizeof(char), strlen(loadable->name) + 1, fp);
        fwrite(&loadable->data_size, sizeof(uint32_t), 1, fp);
        fwrite(loadable->data, sizeof(uint8_t), loadable->data_size, fp);
    }

    fclose(fp);
}

void clstLoaderLoadData(CLSTloader *loader)
{
    CLST *clst;
    struct stat s;
    FILE *fp;
    uint8_t *data;
    int data_offset;
    uint32_t magic_number;

    if (stat(loader->filepath, &s) == -1) {
        // TODO ERROR
        exit(1);
    }

    fp = fopen(loader->filepath, "r");
    data = malloc(s.st_size);
    fread(data, sizeof(uint8_t), s.st_size, fp);

    magic_number = *((uint32_t *)data);
    if (magic_number != CELESTE_FILE_MAGIC_NUMBER) {
        // TODO ERROR
        exit(1);
    }

    data_offset = sizeof(uint32_t);
    clst = clstInstance();

    while (data_offset < s.st_size)
    {
        CLSTloadabletype type;
        int namelen;
        char *name;
        uint32_t data_size;

        type = *((uint8_t *)(data + data_offset));
        data_offset += sizeof(uint8_t);

        namelen = strlen((const char *)(data + data_offset)) + 1;
        name = (char *)(data + data_offset);
        data_offset += namelen;

        data_size = *((uint32_t *)(data + data_offset));
        data_offset += sizeof(uint32_t);
        switch (type)
        {
            case CELESTE_DELETED:
                data_offset += data_size;
                continue;
            case CELESTE_TEXTURE_FILE:
                CELESTE_LOG("Loading texture `%s` from `%s`!\n", name, (data + data_offset));
                clstSceneAddTexture(clst->scene, clstTexture((char *)(data + data_offset), name));
                break;
            case CELESTE_TEXTURE_BIN:
                CELESTE_LOG("Loading texture `%s`!\n", name);
                clstSceneAddTexture(clst->scene, clstTextureMem((data + data_offset), data_size, name));
                break;
            case CELESTE_FONT_FILE:
                CELESTE_LOG("Loading font `%s` from `%s`!\n", name, (data + data_offset + sizeof(float)));
                clstSceneAddFont(clst->scene, clstFont((char *)(data + data_offset + sizeof(float)), *((float*)(data + data_offset)), name));
                break;
            case CELESTE_FONT_BIN:
                CELESTE_LOG("Loading font `%s`!\n", name);
                clstSceneAddFont(clst->scene, clstFontMem(data + data_offset + sizeof(float), data_size - sizeof(float), *((float*)(data + data_offset)), name));
                break;
            case CELESTE_AUDIO_FILE:
                CELESTE_LOG("Loading audio `%s` from `%s`!\n", name, (data + data_offset));
                clstSceneAddAudio(clst->scene, clstAudio((char *)(data + data_offset), name));
                break;
            case CELESTE_AUDIO_BIN:
                CELESTE_LOG("Loading audio `%s`!\n", name);
                clstSceneAddAudio(clst->scene, clstAudioMem((data + data_offset), data_size, name));
                break;
            case CELESTE_SPRITE:
            {
                vec2 pos, size;
                pos[0]  = *((float *)(data + data_offset + sizeof(float) * 0));
                pos[1]  = *((float *)(data + data_offset + sizeof(float) * 1));
                size[0] = *((float *)(data + data_offset + sizeof(float) * 2));
                size[1] = *((float *)(data + data_offset + sizeof(float) * 3));

                CELESTE_LOG("Loading sprite `%s` at `%2.2f, %2.2f`, size `%2.2f, %2.2f`!\n", name, pos[0], pos[1], size[0], size[1]);

                clstLayerAddRenderable(
                    clstSceneGetLastLayer(clst->scene),
                    clstSprite(pos, size, clstSceneGetTexture(clst->scene, (char *)(data + data_offset + sizeof(vec2) * 2)), name)
                );
                break;
            }
            case CELESTE_SPRITE_COLOR:
            {
                vec2 pos, size;
                uint32_t color;

                pos[0]  = *((float *)(data + data_offset + sizeof(float) * 0));
                pos[1]  = *((float *)(data + data_offset + sizeof(float) * 1));
                size[0] = *((float *)(data + data_offset + sizeof(float) * 2));
                size[1] = *((float *)(data + data_offset + sizeof(float) * 3));
                color = *(uint32_t *)(data + data_offset + sizeof(vec2) * 2);

                CELESTE_LOG("Loading sprite `%s` at `%2.2f, %2.2f`, size `%2.2f, %2.2f`!\n", name, pos[0], pos[1], size[0], size[1]);

                clstLayerAddRenderable(
                    clstSceneGetLastLayer(clst->scene),
                    clstSpriteCol(pos, size, color, name)
                );
                break;
            }
            case CELESTE_SPRITE_ATLAS:
            {
                vec2 pos, size, offset, texsize;
                pos[0]     = *((float *)(data + data_offset + sizeof(float) * 0));
                pos[1]     = *((float *)(data + data_offset + sizeof(float) * 1));
                size[0]    = *((float *)(data + data_offset + sizeof(float) * 2));
                size[1]    = *((float *)(data + data_offset + sizeof(float) * 3));
                offset[0]  = *((float*)(data + data_offset + sizeof(float) * 4));
                offset[1]  = *((float*)(data + data_offset + sizeof(float) * 5));
                texsize[0] = *((float*)(data + data_offset + sizeof(float) * 6));
                texsize[1] = *((float*)(data + data_offset + sizeof(float) * 7));

                CELESTE_LOG("Loading sprite atlas `%s` at `%2.2f, %2.2f`, size `%2.2f, %2.2f`!\n", name, pos[0], pos[1], size[0], size[1]);

                clstLayerAddRenderable(
                    clstSceneGetLastLayer(clst->scene),
                    clstSpriteTexAtlas(pos, size, clstSceneGetTexture(clst->scene, (char *)(data + data_offset + sizeof(vec2) * 4)), offset, texsize, name)
                );
                break;
            }
            case CELESTE_ANIMATION:
            {
                uint32_t frames_count;
                uint32_t sprite_offset;
                double frame_time;

                frames_count = *(uint32_t *)(data + data_offset);
                frame_time   = *(double *)(data + data_offset + sizeof(uint32_t));

                CELESTE_LOG("Loading animation `%s`, `%u` frames!\n", name, frames_count);

                CLSTsprite *frames[frames_count];
                sprite_offset = sizeof(uint32_t) + sizeof(double);
                for (int i = 0; i < frames_count; i++)
                {
                    CLSTsprite *sprite;
                    uint32_t sprite_size;

                    sprite = clstDeserializeSprite(data + data_offset + sprite_offset, &sprite_size);
                    CELESTE_LOG("  -- Loading frame `%s`!\n", sprite->name);

                    frames[i] = sprite;
                    sprite_offset += sprite_size;
                }

                clstLayerAddRenderable(
                    clstSceneGetLastLayer(clst->scene),
                    clstAnimation(frames, frames_count, frame_time, name)
                );
                break;
            }
            case CELESTE_LABEL:
            {
                vec2 pos;
                char *text, *font_name;
                pos[0]  = *((float *)(data + data_offset + sizeof(float) * 0));
                pos[1]  = *((float *)(data + data_offset + sizeof(float) * 1));

                text = (char *)(data + data_offset + sizeof(vec2));
                font_name = (char *)(data + data_offset + sizeof(vec2) + strlen(text) + 1);
                CELESTE_LOG("Loading label `%s` `%s` at `%2.2f, %2.2f`!\n", name, text, pos[0], pos[1]);

                clstLayerAddRenderable(
                    clstSceneGetLastLayer(clst->scene),
                    clstLabel(pos, text, clstSceneGetFont(clst->scene, font_name), name)
                );
                break;
            }
            case CELESTE_GROUP:
            {
                CLSTgroup *group;
                mat4 translation;
                uint32_t renderable_offset, renderables_count;

                renderables_count = *(uint32_t *)(data + data_offset);
                memcpy(translation, data + data_offset + sizeof(uint32_t), sizeof(mat4));
                group = clstGroupMat4(translation, name);

                CELESTE_LOG("Loading group `%s`, `%u` renderables!\n", name, renderables_count);

                renderable_offset = sizeof(uint32_t) + sizeof(mat4);
                for (int i = 0; i < renderables_count; i++)
                {
                    CLSTrenderable *renderable;
                    uint32_t renderable_size, renderable_type;

                    renderable = NULL; renderable_size = 0; /* To disable the compiler warning */
                    renderable_type = *((uint32_t *)(data + data_offset + renderable_offset));
                    renderable_offset += sizeof(uint32_t);
                    switch (renderable_type)
                    {
                        case CELESTE_RENDERABLE_SPRITE:
                            renderable = (CLSTrenderable *)clstDeserializeSprite(data + data_offset + renderable_offset, &renderable_size);
                            break;
                        case CELESTE_RENDERABLE_LABEL:
                            renderable = (CLSTrenderable *)clstDeserializeLabel(data + data_offset + renderable_offset, &renderable_size);
                            break;
                        case CELESTE_RENDERABLE_ANIMATION:
                            CELESTE_LOG_ERROR("Unimplemented clstAnimationSerialize!");
                            break;
                        case CELESTE_RENDERABLE_GROUP:
                            CELESTE_LOG_ERROR("Unimplemented clstGroupSerialize!");
                            break;
                        case CELESTE_RENDERABLE_BUTTON:
                            renderable = (CLSTrenderable *)clstDeserializeButton(data + data_offset + renderable_offset, &renderable_size);
                            break;
                    }
                    CELESTE_LOG("  -- Loading renderable `%s`!\n", renderable->name);

                    clstGroupAddRenderable(group, renderable);
                    renderable_offset += renderable_size;
                }

                clstLayerAddRenderable(clstSceneGetLastLayer(clst->scene), group);
                break;
            }
            case CELESTE_BODY:
            {
                CLSTbody *body;
                body = clstDeserializeBody(data + data_offset);
                CELESTE_LOG("Loading body `%s` at `%2.2f, %2.2f`!\n", name, body->position[0], body->position[1]);
                clstSceneAddBody(clst->scene, body);
                break;
            }
            case CELESTE_LAYER:
            {
                vec2 campos;
                float right, top;
                campos[0] = *((float *)(data + data_offset + sizeof(float) * 0));
                campos[1] = *((float *)(data + data_offset + sizeof(float) * 1));
                right     = *((float *)(data + data_offset + sizeof(float) * 2));
                top       = *((float *)(data + data_offset + sizeof(float) * 3));

                CELESTE_LOG("Loading layer `%s` `%2.2f, %2.2f`!\n", name, right, top);
                clstSceneAddLayer(clst->scene, clstLayerCamera(clstCameraOrtho(campos), right, top, name));
                break;
            }
            default:
                break;
        }
        clstLoadable(name, data + data_offset, data_size, type);
        data_offset += data_size;
    }

    free(data);
    fclose(fp);
}

CLSTloadable *clstLoadable(char *name, void *data, int data_size, uint8_t type)
{
    CLSTloader *loader;
    CLSTloadable *loadable;

    if (clstLoadableExists(name, data, data_size, type))
        return NULL;

    loadable = malloc(sizeof(CLSTloadable));
    loadable->name = strdup(name);
    loadable->data = malloc(data_size);
    memcpy(loadable->data, data, data_size);
    loadable->data_size = data_size;
    loadable->type = type;

    loader = clstInstance()->loader;
    clstListAdd(loader->loadables, loadable);
    return loadable;
}

void clstLoadableDestroy(CLSTloadable *loadable)
{
    free(loadable->name);
    free(loadable->data);
    free(loadable);
}

int clstLoadableExists(char *name, void *data, int data_size, uint8_t type)
{
    CLSTloader *loader;
    
    loader = clstInstance()->loader;
    for (int i = 0; i < loader->loadables->count; i++) {
        CLSTloadable *loadable;
        
        loadable = ((CLSTloadable **)loader->loadables->items)[i];
        if (loadable->type == type)
            if (loadable->data_size == data_size)
                if (strcmp(loadable->name, name) == 0)
                    if (memcmp(loadable->data, data, data_size) == 0)
                        return 1;
    }
    return 0;
}