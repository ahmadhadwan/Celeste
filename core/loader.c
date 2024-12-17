#include "loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <libgen.h>

CLSTtexture *clstTextureSave(char *filepath, char *name)
{
    void *data;
    uint32_t data_size;

    data_size = strlen(filepath) + 1;
    data = malloc(data_size);
    memcpy(data, filepath, data_size);
    clstLoadable(name, data, data_size, CLST_TEXTURE_FILE);
    free(data);
    return clstTexture(filepath, name);
}

CLSTtexture *clstTextureMemSave(uint8_t *buffer, uint32_t bufsize, char *name)
{
    void *data;

    data = malloc(bufsize);
    memcpy(data, buffer, bufsize);
    clstLoadable(name, buffer, bufsize, CLST_TEXTURE_BIN);
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
    clstLoadable(name, data, data_size, CLST_FONT_FILE);
    free(data);
    return clstFont(filepath, size, name);
}

CLSTfont *clstFontMemSave(uint8_t *buffer, uint32_t bufsize, float size, char *name)
{
    void *data;

    data = malloc(bufsize);
    memcpy(data, buffer, bufsize);
    clstLoadable(name, buffer, bufsize, CLST_TEXTURE_BIN);
    free(data);
    return clstFontMem(buffer, bufsize, size, name);
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
    uint32_t start = CLST_FILE_MAGIC_NUMBER;

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
    if (magic_number != CLST_FILE_MAGIC_NUMBER) {
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
            case CLST_TEXTURE_FILE:
                CELESTE_LOG("Loading texture `%s` from `%s`!\n", name, (data + data_offset));
                clstSceneAddTexture(clst->scene, clstTexture((char *)(data + data_offset), name));
                clstLoadable(name, data + data_offset, data_size, CLST_TEXTURE_FILE);
                break;
            case CLST_TEXTURE_BIN:
                CELESTE_LOG("Loading texture `%s`!\n", name);
                clstSceneAddTexture(clst->scene, clstTextureMem((data + data_offset), data_size, name));
                clstLoadable(name, data + data_offset, data_size, CLST_TEXTURE_BIN);
                break;
            case CLST_FONT_FILE:
                CELESTE_LOG("Loading font `%s` from `%s`!\n", name, (data + data_offset + sizeof(float)));
                clstSceneAddFont(clst->scene, clstFont((char *)(data + data_offset + sizeof(float)), *((float*)(data + data_offset)), name));
                clstLoadable(name, data + data_offset, data_size, CLST_FONT_FILE);
                break;
            case CLST_FONT_BIN:
                CELESTE_LOG("Loading font `%s`!\n", name);
                clstSceneAddFont(clst->scene, clstFontMem(data + data_offset + sizeof(float), data_size - sizeof(float), *((float*)(data + data_offset)), name));
                clstLoadable(name, data + data_offset, data_size, CLST_FONT_BIN);
                break;
            default:
                // ERROR
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