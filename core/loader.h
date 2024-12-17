#ifndef __CELESTE_LOADER_H__
#define __CELESTE_LOADER_H__

#include <stdint.h>
#include "celeste.h"

#define CLST_FILE_MAGIC_NUMBER  0x04200615
#define CLST_LOADABLE_TYPE_SIZE 1

typedef struct {
    char    *name;
    void    *data;
    uint32_t data_size;
    uint8_t  type;
} CLSTloadable;

typedef enum {
    CLST_TEXTURE_FILE = 1,
    CLST_TEXTURE_BIN  = 2,
} CLSTloadabletype;

typedef struct {
    char          *filepath;
    CLSTloadable **loadables;
    int            loadables_count;
} CLSTloader;

CLSTloader *clstLoader(char *filepath);
void clstLoaderDestroy(CLSTloader *loader);

void clstLoaderSaveData(CLSTloader *loader);
void clstLoaderLoadData(CLSTloader *loader);

CLSTloadable *clstLoadable(char *name, void *data, int data_size, uint8_t type);
void clstLoadableDestroy(CLSTloadable *loadable);

CLSTtexture *clstTextureSave(char *filepath);
CLSTtexture *clstTextureMemSave(char *name, uint8_t *buffer, uint32_t bufsize);

#endif /* __CELESTE_LOADER_H__ */
