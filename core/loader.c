#include "internal/gapi.h"
#include "loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <libgen.h>

static int clstLoadableExists(char *name, void *data, int data_size, uint8_t type);

CLSTtexture *clstTextureSave(char *filepath, char *name)
{
    void *data;
    uint32_t data_size;

    data_size = strlen(filepath) + 1;
    data = malloc(data_size);
    memcpy(data, filepath, data_size);
    clstLoadable(name, data, data_size, CELESTE_TEXTURE_FILE);
    free(data);
    return clstTexture(filepath, name);
}

CLSTtexture *clstTextureMemSave(uint8_t *buffer, uint32_t bufsize, char *name)
{
    void *data;

    data = malloc(bufsize);
    memcpy(data, buffer, bufsize);
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

    data = malloc(bufsize);
    memcpy(data, buffer, bufsize);
    clstLoadable(name, buffer, bufsize, CELESTE_FONT_BIN);
    free(data);
    return clstFontMem(buffer, bufsize, size, name);
}

CLSTaudio *clstAudioSave(const char *filepath, char *name)
{
    void *data;
    uint32_t data_size;

    data_size = strlen(filepath) + 1;
    data = malloc(data_size);
    memcpy(data, filepath, data_size);
    clstLoadable(name, data, data_size, CELESTE_AUDIO_FILE);
    free(data);
    return clstAudio(filepath, name);
}

CLSTaudio *clstAudioMemSave(unsigned char *buffer, unsigned int bufsize, char *name)
{
    void *data;

    data = malloc(bufsize);
    memcpy(data, buffer, bufsize);
    clstLoadable(name, buffer, bufsize, CELESTE_AUDIO_BIN);
    free(data);
    return clstAudioMem(buffer, bufsize, name);
}

CLSTsprite *clstSpriteSave(vec2 position, vec2 size, CLSTtexture *texture, char *name)
{
    void *data;
    uint32_t data_size;

    data_size = (sizeof(vec2) * 2) + strlen(texture->name) + 1;
    data = malloc(data_size);
    memcpy(data, position, sizeof(vec2));
    memcpy(data + sizeof(vec2), size, sizeof(vec2));
    memcpy(data + (sizeof(vec2) * 2), texture->name, strlen(texture->name) + 1);
    clstLoadable(name, data, data_size, CELESTE_SPRITE);
    free(data);
    return clstSprite(position, size, texture, name);
}

CLSTsprite *clstSpriteTexAtlasSave(vec2 position, vec2 size, CLSTtexture *texture_atlas, vec2 offset, vec2 texsize, char *name)
{
    void *data;
    uint32_t data_size;

    data_size = (sizeof(vec2) * 4) + strlen(texture_atlas->name) + 1;
    data = malloc(data_size);
    memcpy(data, position, sizeof(vec2));
    memcpy(data + sizeof(vec2), size, sizeof(vec2));
    memcpy(data + (sizeof(vec2) * 2), offset, sizeof(vec2));
    memcpy(data + (sizeof(vec2) * 3), texsize, sizeof(vec2));
    memcpy(data + (sizeof(vec2) * 4), texture_atlas->name, strlen(texture_atlas->name) + 1);
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
        CLSTsprite *sprite;

        sprite = frames[i];
        data_size += (sizeof(vec2) * 6) + sizeof(uint32_t) + strlen(sprite->texture->name) + 1;
        data = realloc(data, data_size);

        memcpy(data + offset, sprite->position, sizeof(vec2));
        memcpy((data + offset) + (sizeof(vec2) * 1), sprite->size, sizeof(vec2));
        memcpy((data + offset) + (sizeof(vec2) * 2), sprite->uv[0], sizeof(vec2));
        memcpy((data + offset) + (sizeof(vec2) * 3), sprite->uv[1], sizeof(vec2));
        memcpy((data + offset) + (sizeof(vec2) * 4), sprite->uv[2], sizeof(vec2));
        memcpy((data + offset) + (sizeof(vec2) * 5), sprite->uv[3], sizeof(vec2));
        memcpy((data + offset) + (sizeof(vec2) * 6), &(sprite->color), sizeof(uint32_t));
        memcpy((data + offset) + (sizeof(vec2) * 6) + sizeof(uint32_t), sprite->texture->name, strlen(sprite->texture->name) + 1);

        offset = data_size;
    }

    clstLoadable(name, data, data_size, CELESTE_ANIMATION);
    free(data);
    return clstAnimation(frames, frames_count, frame_time, name);
}

CLSTlabel *clstLabelSave(vec2 position, char *text, CLSTfont *font, char *name)
{
    void *data;
    uint32_t data_size;

    data_size = sizeof(vec2) + (strlen(text) + 1) + (strlen(font->name) + 1);
    data = malloc(data_size);
    memcpy(data, position, sizeof(vec2));
    memcpy(data + sizeof(vec2), text, strlen(text) + 1);
    memcpy(data + sizeof(vec2) + strlen(text) + 1, font->name, strlen(font->name) + 1);
    clstLoadable(name, data, data_size, CELESTE_LABEL);
    free(data);
    return clstLabel(position, text, font, name);
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

                CLSTsprite *frames[frames_count];
                sprite_offset = sizeof(uint32_t) + sizeof(double);
                for (int i = 0; i < frames_count; i++)
                {
                    CLSTsprite *sprite;

                    sprite = malloc(sizeof(CLSTsprite));
                    sprite->name = malloc(strlen(name) + strlen(" Frame ") + 4);
                    sprintf(sprite->name, "%s Frame %d", name, i + 1);
                    memcpy(sprite->position, (data + data_offset + sprite_offset), sizeof(vec2));
                    memcpy(sprite->size, (data + data_offset + sprite_offset + sizeof(vec2)), sizeof(vec2));
                    memcpy(sprite->uv[0], (data + data_offset + sprite_offset + (sizeof(vec2) * 2)), sizeof(vec2));
                    memcpy(sprite->uv[1], (data + data_offset + sprite_offset + (sizeof(vec2) * 3)), sizeof(vec2));
                    memcpy(sprite->uv[2], (data + data_offset + sprite_offset + (sizeof(vec2) * 4)), sizeof(vec2));
                    memcpy(sprite->uv[3], (data + data_offset + sprite_offset + (sizeof(vec2) * 5)), sizeof(vec2));
                    memcpy(&(sprite->color), (data + data_offset + sprite_offset + (sizeof(vec2) * 6)), sizeof(uint32_t));
                    sprite->texture = clstSceneGetTexture(clst->scene, (char *)(data + data_offset + sprite_offset + sizeof(uint32_t) + (sizeof(vec2) * 6)));

                    frames[i] = sprite;
                    sprite_offset += sizeof(uint32_t) + (sizeof(vec2) * 6) + strlen(sprite->texture->name) + 1;
                }

                CELESTE_LOG("Loading animation `%s`, `%u` frames!\n", name, frames_count);

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