#ifndef __CELESTE_LIST_H__
#define __CELESTE_LIST_H__

#include <stdint.h>

typedef struct {
    uint32_t count;
    void   **items;
} CLSTlist;

CLSTlist *clstListCreate();
void clstListDestroy(CLSTlist *list, void (*clstItemDestroyer)(void *item));

void clstListAdd(CLSTlist *list, void *item);

#endif /* __CELESTE_LIST_H__ */