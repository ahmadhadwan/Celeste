#include "list.h"
#include <stdlib.h>

CLSTlist *clstListCreate()
{
    return calloc(1, sizeof(CLSTlist));
}

void clstListDestroy(CLSTlist *list, CLSTitemdestroy destroy)
{
    for (int i = 0; i < list->count; i++)
        destroy(list->items[i]);
    free(list->items);
    free(list);
}

void clstListAdd(CLSTlist *list, void *item)
{
    list->items = realloc(list->items, (list->count + 1) * sizeof(void *));
    list->items[list->count] = item;
    list->count++;
}