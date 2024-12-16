#ifndef __CELESTE_LOADER_H__
#define __CELESTE_LOADER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef struct {
    char *name;
    void *data;
} CLSTloadable;

typedef struct {
    char          *filepath;
    CLSTloadable **loadables;
    int            loadables_count;
} CLSTloader;

CLSTloader *clstLoader(char *filepath);
void clstLoaderDestroy(CLSTloader *loader);

void clstLoaderLoadData(CLSTloader *loader);

CLSTloadable *clstLoadable(CLSTloader *loader, char *name, void *value);
void clstLoadableDestroy(CLSTloadable *loadable);

#ifdef CELESTE_LOADER_IMPLEMENTATION

CLSTloader *clstLoader(char *filepath)
{
    CLSTloader *loader;
    loader = malloc(sizeof(CLSTloader));
    loader->filepath = filepath;
    loader->loadables = malloc(0);
    loader->loadables_count = 0;
    return loader;
}

void clstLoaderDestroy(CLSTloader *loader)
{
    for (int i = 0; i < loader->loadables_count; i++)
        clstLoadableDestroy(loader->loadables[i]);
    free(loader->loadables);
    free(loader);
}

void clstLoaderLoadData(CLSTloader *loader)
{
    struct stat s;
    FILE *fp;
    char *data;

    fp = fopen(loader->filepath, "r");
    data = malloc(s.st_size + 1);
    fread(data, sizeof(char), s.st_size, fp);
    data[s.st_size] = 0;

    for (int i = 0; i < loader->loadables_count; i++)
    {
        CLSTloadable *loadable = loader->loadables[i];
        char *name = strstr(data, loadable->name);
        if (name) {
            name += strlen(loadable->name) + 1;
            ((float*)loadable->data)[0] = atof(name);
            name = strstr(name, " ") + 1;
            ((float*)loadable->data)[1] = atof(name);
        }
    }

    free(data);
    fclose(fp);
}

CLSTloadable *clstLoadable(CLSTloader *loader, char *name, void *value)
{
    CLSTloadable *loadable;
    loadable = malloc(sizeof(CLSTloadable));
    loadable->name = name;
    loadable->data = value;

    loader->loadables = realloc(loader->loadables, (loader->loadables_count + 1) * sizeof(CLSTloadable *));
    loader->loadables[loader->loadables_count] = loadable;
    loader->loadables_count++;
    return loadable;
}

void clstLoadableDestroy(CLSTloadable *loadable)
{
    free(loadable);
}

#endif /* CELESTE_LOADER_IMPLEMENTATION */

#endif /* __CELESTE_LOADER_H__ */
