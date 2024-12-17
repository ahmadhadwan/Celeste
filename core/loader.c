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

CLSTsprite *clstSpriteSave(vec2 position, vec2 size, CLSTtexture *texture)
{
    void *data;
    uint32_t data_size;

    data_size = (sizeof(vec2) * 2) + strlen(texture->name) + 1;
    data = malloc(data_size);
    memcpy(data, position, sizeof(vec2));
    memcpy(data + sizeof(vec2), size, sizeof(vec2));
    memcpy(data + (sizeof(vec2) * 2), texture->name, strlen(texture->name) + 1);
    clstLoadable("Sp", data, data_size, CELESTE_SPRITE);
    free(data);
    return clstSprite(position, size, texture);
}

CLSTlayer *clstLayerCameraSave(CLSTcamera *camera, float right, float top)
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
    clstLoadable("Layer", data, data_size, CELESTE_LAYER);
    free(data);
    return clstLayerCamera(camera, right, top);
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
    loader->loadables = NULL;
    loader->loadables_count = 0;

    clst->loader = loader;
    return loader;
}

void clstLoaderDestroy(CLSTloader *loader)
{
    for (int i = 0; i < loader->loadables_count; i++)
        clstLoadableDestroy(loader->loadables[i]);
    free(loader->loadables);
    free(loader);
}

void clstLoaderSaveData(CLSTloader *loader)
{
    FILE *fp;
    uint32_t start = CELESTE_FILE_MAGIC_NUMBER;

    fp = fopen(loader->filepath, "w");

    fwrite(&start, sizeof(start), 1, fp);

    for (int i = 0; i < loader->loadables_count; i++)
    {
        CLSTloadable *loadable = loader->loadables[i];
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
                // The Loadable is ignored, and isn't written into the next save.
                break;
            case CELESTE_TEXTURE_FILE:
                CELESTE_LOG("Loading texture `%s` from `%s`!\n", name, (data + data_offset));
                clstSceneAddTexture(clst->scene, clstTexture((char *)(data + data_offset), name));
                clstLoadable(name, data + data_offset, data_size, CELESTE_TEXTURE_FILE);
                break;
            case CELESTE_TEXTURE_BIN:
                CELESTE_LOG("Loading texture `%s`!\n", name);
                clstSceneAddTexture(clst->scene, clstTextureMem((data + data_offset), data_size, name));
                clstLoadable(name, data + data_offset, data_size, CELESTE_TEXTURE_BIN);
                break;
            case CELESTE_FONT_FILE:
                CELESTE_LOG("Loading font `%s` from `%s`!\n", name, (data + data_offset + sizeof(float)));
                clstSceneAddFont(clst->scene, clstFont((char *)(data + data_offset + sizeof(float)), *((float*)(data + data_offset)), name));
                clstLoadable(name, data + data_offset, data_size, CELESTE_FONT_FILE);
                break;
            case CELESTE_FONT_BIN:
                CELESTE_LOG("Loading font `%s`!\n", name);
                clstSceneAddFont(clst->scene, clstFontMem(data + data_offset + sizeof(float), data_size - sizeof(float), *((float*)(data + data_offset)), name));
                clstLoadable(name, data + data_offset, data_size, CELESTE_FONT_BIN);
                break;
            case CELESTE_AUDIO_FILE:
                CELESTE_LOG("Loading audio `%s` from `%s`!\n", name, (data + data_offset));
                clstSceneAddAudio(clst->scene, clstAudio((char *)(data + data_offset), name));
                clstLoadable(name, data + data_offset, data_size, CELESTE_AUDIO_FILE);
                break;
            case CELESTE_AUDIO_BIN:
                CELESTE_LOG("Loading audio `%s`!\n", name);
                clstSceneAddAudio(clst->scene, clstAudioMem((data + data_offset), data_size, name));
                clstLoadable(name, data + data_offset, data_size, CELESTE_AUDIO_BIN);
                break;
            case CELESTE_SPRITE:
                vec2 pos, size;
                pos[0]  = *((float*)(data + data_offset + sizeof(float) * 0));
                pos[1]  = *((float*)(data + data_offset + sizeof(float) * 1));
                size[0] = *((float*)(data + data_offset + sizeof(float) * 2));
                size[1] = *((float*)(data + data_offset + sizeof(float) * 3));

                CELESTE_LOG("Loading sprite `%s` at `%2.2f, %2.2f`, size `%2.2f, %2.2f`!\n", name, pos[0], pos[1], size[0], size[1]);

                clstLayerAddSprite(
                    clstSceneGetLayer(clst->scene),
                    clstSprite(pos, size, clstSceneGetTexture(clst->scene, (char *)(data + data_offset + sizeof(float) * 4)))
                );
                clstLoadable(name, data + data_offset, data_size, CELESTE_SPRITE);
                break;
            case CELESTE_LAYER:
                vec2 campos;
                float right, top;
                campos[0] = *((float*)(data + data_offset + sizeof(float) * 0));
                campos[1] = *((float*)(data + data_offset + sizeof(float) * 1));
                right     = *((float*)(data + data_offset + sizeof(float) * 2));
                top       = *((float*)(data + data_offset + sizeof(float) * 3));

                CELESTE_LOG("Loading layer `%s` `%2.2f, %2.2f`!\n", name, right, top);

                clstSceneAddLayer(clst->scene, clstLayerCamera(clstCameraOrtho(campos), right, top));
                clstLoadable(name, data + data_offset, data_size, CELESTE_LAYER);
                break;
            default:
                break;
        }
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
    loadable->name = malloc(strlen(name) + 1);
    strcpy(loadable->name, name);
    loadable->data = malloc(data_size);
    memcpy(loadable->data, data, data_size);
    loadable->data_size = data_size;
    loadable->type = type;

    loader = clstInstance()->loader;
    loader->loadables = realloc(loader->loadables, (loader->loadables_count + 1) * sizeof(CLSTloadable *));
    loader->loadables[loader->loadables_count] = loadable;
    loader->loadables_count++;
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
    for (int i = 0; i < loader->loadables_count; i++) {
        CLSTloadable *loadable;
        
        loadable = loader->loadables[i];
        if (loadable->type == type)
            if (loadable->data_size == data_size)
                if (strcmp(loadable->name, name) == 0)
                    if (memcmp(loadable->data, data, data_size) == 0)
                        return 1;
    }
    return 0;
}