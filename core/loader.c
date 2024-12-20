#include "internal/gapi.h"
#include "internal/celeste.h"

#include "loader.h"
#include "serialization.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static int clstLoadableExists(char *name, void *data, uint32_t data_size, uint8_t type);

static void *memory_dupe(void *src, uint32_t data_size)
{
    void *data;
    data = malloc(data_size);
    memcpy(data, src, data_size);
    return data;
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
    clstLoadable(name, buffer, bufsize, CELESTE_TEXTURE_BIN);
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
    clstLoadable(name, buffer, bufsize, CELESTE_FONT_BIN);
    return clstFontMem(buffer, bufsize, size, name);
}

CLSTaudio *clstAudioSave(const char *filepath, char *name)
{
    clstLoadable(name, (void *)filepath, strlen(filepath) + 1, CELESTE_AUDIO_FILE);
    return clstAudio(filepath, name);
}

CLSTaudio *clstAudioMemSave(uint8_t *buffer, uint32_t bufsize, char *name)
{
    clstLoadable(name, buffer, bufsize, CELESTE_AUDIO_BIN);
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

    text_len = strlen(text) + 1;
    fontname_len = strlen(font->name) + 1;
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
    void *data;
    uint32_t data_size;

    data_size = (sizeof(uint32_t) * 2) + (sizeof(vec2) * 3);
    data = malloc(data_size);

    memcpy(data, &layer, sizeof(uint32_t));
    memcpy(data + (sizeof(uint32_t)), &flags, sizeof(uint32_t));
    memcpy(data + (sizeof(uint32_t) * 2), position, sizeof(vec2));
    memcpy(data + (sizeof(uint32_t) * 2) + sizeof(vec2), size, sizeof(vec2));
    memset(data + (sizeof(uint32_t) * 2) + (sizeof(vec2) * 2), 0, sizeof(vec2));

    clstLoadable(name, data, data_size, CELESTE_BODY);
    free(data);

    return clstBody(layer, flags, position, size, name);
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
        CELESTE_LOG_ERROR("Can't have multiple loaders at one time!");
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
    fp = fopen(loader->filepath, "wb");
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

void clstLoaderLoadData(CLSTloader *loader, CLSTscene *scene)
{
    struct stat s;
    FILE *fp;
    uint8_t *data;
    uint32_t data_offset;
    uint32_t magic_number;

    if (stat(loader->filepath, &s) == -1) {
        CELESTE_LOG("Failed to stat file `%s`!\n", loader->filepath);
        clstTerminate();
        exit(1);
    }

    fp = fopen(loader->filepath, "rb");
    data = malloc(s.st_size);
    fread(data, sizeof(uint8_t), s.st_size, fp);
    fclose(fp);

    magic_number = *((uint32_t *)data);
    if (magic_number != CELESTE_FILE_MAGIC_NUMBER) {
        CELESTE_LOG("File `%s` isn't a valid celeste scene!\n", loader->filepath);
        clstTerminate();
        exit(1);
    }

    data_offset = sizeof(uint32_t);
    while (data_offset < s.st_size)
    {
        CLSTloadabletype type;
        char *name;
        uint32_t data_size;

        type = *(uint8_t *)(data + data_offset);
        data_offset += sizeof(uint8_t);
        name = (char *)(data + data_offset);
        data_offset += strlen((char *)(data + data_offset)) + 1;
        data_size = *((uint32_t *)(data + data_offset));
        data_offset += sizeof(uint32_t);

        switch (type)
        {
            case CELESTE_DELETED:
                data_offset += data_size;
                continue;
            case CELESTE_TEXTURE_FILE:
                CELESTE_LOG("Loading texture `%s` from `%s`!\n", name, data + data_offset);
                clstSceneAddTexture(scene, clstTexture((char *)(data + data_offset), name));
                break;
            case CELESTE_TEXTURE_BIN:
                CELESTE_LOG("Loading texture `%s`!\n", name);
                clstSceneAddTexture(scene, clstTextureMem(data + data_offset, data_size, name));
                break;
            case CELESTE_FONT_FILE:
                CELESTE_LOG("Loading font `%s` from `%s`!\n", name, (data + data_offset + sizeof(float)));
                clstSceneAddFont(scene, clstFont((char *)(data + data_offset + sizeof(float)), *(float*)(data + data_offset), name));
                break;
            case CELESTE_FONT_BIN:
                CELESTE_LOG("Loading font `%s`!\n", name);
                clstSceneAddFont(scene, clstFontMem(data + data_offset + sizeof(float), data_size - sizeof(float), *(float*)(data + data_offset), name));
                break;
            case CELESTE_AUDIO_FILE:
                CELESTE_LOG("Loading audio `%s` from `%s`!\n", name, data + data_offset);
                clstSceneAddAudio(scene, clstAudio((char *)(data + data_offset), name));
                break;
            case CELESTE_AUDIO_BIN:
                CELESTE_LOG("Loading audio `%s`!\n", name);
                clstSceneAddAudio(scene, clstAudioMem(data + data_offset, data_size, name));
                break;
            case CELESTE_SPRITE:
            {
                vec2 pos, size;
                CLSTtexture *tex;
                glm_vec2_copy((float *)(data + data_offset), pos);
                glm_vec2_copy((float *)(data + data_offset + sizeof(vec2)), size);
                tex = clstSceneGetTexture(scene, (char *)(data + data_offset + (sizeof(vec2) * 2)));

                CELESTE_LOG("Loading sprite `%s` at `%2.2f, %2.2f`, size `%2.2f, %2.2f`!\n", name, pos[0], pos[1], size[0], size[1]);
                clstLayerAddRenderable(clstSceneGetLastLayer(scene), clstSprite(pos, size, tex, name));
                break;
            }
            case CELESTE_SPRITE_COLOR:
            {
                vec2 pos, size;
                uint32_t color;
                glm_vec2_copy((float *)(data + data_offset), pos);
                glm_vec2_copy((float *)(data + data_offset + sizeof(vec2)), size);
                color = *(uint32_t *)(data + data_offset + sizeof(vec2) * 2);

                CELESTE_LOG("Loading sprite `%s` at `%2.2f, %2.2f`, size `%2.2f, %2.2f`!\n", name, pos[0], pos[1], size[0], size[1]);
                clstLayerAddRenderable(clstSceneGetLastLayer(scene), clstSpriteCol(pos, size, color, name));
                break;
            }
            case CELESTE_SPRITE_ATLAS:
            {
                vec2 pos, size, offset, texsize;
                CLSTtexture *tex;
                glm_vec2_copy((float *)(data + data_offset), pos);
                glm_vec2_copy((float *)(data + data_offset + sizeof(vec2)), size);
                glm_vec2_copy((float *)(data + data_offset + (sizeof(vec2) * 2)), offset);
                glm_vec2_copy((float *)(data + data_offset + (sizeof(vec2) * 3)), texsize);
                tex = clstSceneGetTexture(scene, (char *)(data + data_offset + sizeof(vec2) * 4));

                CELESTE_LOG("Loading sprite atlas `%s` at `%2.2f, %2.2f`, size `%2.2f, %2.2f`!\n", name, pos[0], pos[1], size[0], size[1]);

                clstLayerAddRenderable(clstSceneGetLastLayer(scene), clstSpriteTexAtlas(pos, size, tex, offset, texsize, name));
                break;
            }
            case CELESTE_ANIMATION:
            {
                uint32_t frames_count, sprite_offset;
                double frame_time;
                frames_count = *(uint32_t *)(data + data_offset);
                frame_time   = *(double *)(data + data_offset + sizeof(uint32_t));

                CELESTE_LOG("Loading animation `%s`, `%u` frames!\n", name, frames_count);

                CLSTsprite *frames[frames_count];
                sprite_offset = sizeof(uint32_t) + sizeof(double);
                for (int i = 0; i < frames_count; i++)
                {
                    uint32_t sprite_size;
                    frames[i] = clstDeserializeSprite(data + data_offset + sprite_offset, &sprite_size);
                    CELESTE_LOG("  -- Loading frame `%s`!\n", frames[i]->name);
                    sprite_offset += sprite_size;
                }

                clstLayerAddRenderable(clstSceneGetLastLayer(scene), clstAnimation(frames, frames_count, frame_time, name));
                break;
            }
            case CELESTE_LABEL:
            {
                vec2 pos;
                char *text, *font_name;
                glm_vec2_copy((float *)(data + data_offset), pos);
                text = (char *)(data + data_offset + sizeof(vec2));
                font_name = (char *)(data + data_offset + sizeof(vec2) + strlen(text) + 1);

                CELESTE_LOG("Loading label `%s` `%s` at `%2.2f, %2.2f`!\n", name, text, pos[0], pos[1]);

                clstLayerAddRenderable(clstSceneGetLastLayer(scene), clstLabel(pos, text, clstSceneGetFont(scene, font_name), name));
                break;
            }
            case CELESTE_GROUP:
            {
                CLSTgroup *group;
                mat4 translation;
                uint32_t renderables_count, renderable_offset;

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
                            renderable = (CLSTrenderable *)clstDeserializeAnimation(data + data_offset + renderable_offset, &renderable_size);
                            break;
                        case CELESTE_RENDERABLE_GROUP:
                            renderable = (CLSTrenderable *)clstDeserializeGroup(data + data_offset + renderable_offset, &renderable_size);
                            break;
                        case CELESTE_RENDERABLE_BUTTON:
                            renderable = (CLSTrenderable *)clstDeserializeButton(data + data_offset + renderable_offset, &renderable_size);
                            break;
                        default:
                            CELESTE_LOG_ERROR("Unimplemented renderable deserialization!\n");
                            clstTerminate();
                            exit(1);
                    }
                    CELESTE_LOG("  -- Loading renderable `%s`!\n", renderable->name);

                    clstGroupAddRenderable(group, renderable);
                    renderable_offset += renderable_size;
                }

                clstLayerAddRenderable(clstSceneGetLastLayer(scene), group);
                break;
            }
            case CELESTE_BODY:
            {
                CLSTbody *body;
                uint32_t layer, flags;
                vec2 position, size, velocity;

                memcpy(&layer  , (data + data_offset), sizeof(uint32_t));
                memcpy(&flags  , (data + data_offset + sizeof(uint32_t)), sizeof(uint32_t));
                memcpy(position, (data + data_offset + (sizeof(uint32_t) * 2)), sizeof(vec2));
                memcpy(size    , (data + data_offset + (sizeof(uint32_t) * 2) + sizeof(vec2)), sizeof(vec2));
                memcpy(velocity, (data + data_offset + (sizeof(uint32_t) * 2) + (sizeof(vec2) * 2)), sizeof(vec2));

                body = clstBody(layer, flags, position, size, name);
                glm_vec2_copy(velocity, body->velocity);
                CELESTE_LOG("Loading body `%s` at `%2.2f, %2.2f`!\n", name, body->position[0], body->position[1]);
                clstSceneAddBody(scene, body);
                break;
            }
            case CELESTE_LAYER:
            {
                vec2 campos;
                float right, top;
                glm_vec2_copy((float *)(data + data_offset), campos);
                right = *((float *)(data + data_offset + sizeof(float) * 2));
                top   = *((float *)(data + data_offset + sizeof(float) * 3));

                CELESTE_LOG("Loading layer `%s` `%2.2f, %2.2f`!\n", name, right, top);
                clstSceneAddLayer(scene, clstLayerCamera(clstCameraOrtho(campos), right, top, name));
                break;
            }
            default:
                CELESTE_LOG_ERROR("Unimplemented load data type!");
                break;
        }
        clstLoadable(name, data + data_offset, data_size, type);
        data_offset += data_size;
    }

    free(data);
}

CLSTloadable *clstLoadable(char *name, void *data, uint32_t data_size, uint8_t type)
{
    CLSTloader *loader;
    CLSTloadable *loadable;

    if (clstLoadableExists(name, data, data_size, type))
        return NULL;

    loadable = malloc(sizeof(CLSTloadable));
    loadable->name = strdup(name);
    loadable->data = memory_dupe(data, data_size);
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

int clstLoadableExists(char *name, void *data, uint32_t data_size, uint8_t type)
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