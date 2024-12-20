#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void clstListRemove(CLSTlist *list, void *item)
{
    for (int i = 0; i < list->count; i++) {
        if ((list->items)[i] == item)
        {
            if (i + 1 < list->count)
                memmove((list->items + i), (list->items + i + 1), (list->count - i - 1) * sizeof(void *));
            
            list->items = realloc(list->items, (list->count - 1) * sizeof(void *));
            list->count--;
            break;
        }
    }
}