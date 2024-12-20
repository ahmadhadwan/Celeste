#include "internal/celeste.h"
#include "internal/window.h"

#include "input.h"
#include "celeste.h"
#include <string.h>

static void listener_delete();

void clstKeyAddCallback(CLSTkey key)
{
    CLST *clst;
    CLSTkey *keyptr;

    clst = clstInstance();
    keyptr = malloc(sizeof(CLSTkey));
    memcpy(keyptr, &key, sizeof(CLSTkey));
    clstListAdd(clst->keys, keyptr);
}

void clstKeyRemoveCallback(uint32_t key)
{
    CLST *clst;
    CLSTlist *keylist;
    CLSTkey *_key;

    clst = clstInstance();
    keylist = (CLSTlist *)clst->keys;
    for (int i = 0; i < keylist->count; i++)
    {
        _key = ((CLSTkey **)keylist->items)[i];
        if (_key->key == key)
        {
            clstListRemove(keylist, _key);
            free(_key);
            break;
        }
    }
}

uint32_t clstKey(uint32_t key)
{
    return glfwGetKey(clstInstance()->window.glfw_window, key);
}

void clstClickAddCallback(CLSTclick click)
{
    CLST *clst;
    CLSTclick *clickptr;

    clst = clstInstance();
    clickptr = malloc(sizeof(CLSTclick));
    memcpy(clickptr, &click, sizeof(CLSTclick));
    clstListAdd(clst->clicks, clickptr);
}

void clstClickRemoveCallback(uint32_t click)
{
    CLST *clst;
    CLSTlist *clicklist;
    CLSTclick *_click;

    clst = clstInstance();
    clicklist = (CLSTlist *)clst->clicks;
    for (int i = 0; i < clicklist->count; i++)
    {
        _click = ((CLSTclick **)clicklist->items)[i];
        if (_click->click == click)
        {
            clstListRemove(clicklist, _click);
            free(_click);
            break;
        }
    }
}

uint32_t clstClick(uint32_t click)
{
    return glfwGetMouseButton(clstInstance()->window.glfw_window, click);
}

void clstScrollAddListener(double *listener)
{
    CLST *clst;

    clst = clstInstance();
    clstListAdd(clst->scroll_listeners, listener);
}

void clstScrollRemoveListener(double *listener)
{
    CLST *clst;

    clst = clstInstance();
    clstListRemove(clst->scroll_listeners, listener);
}

void clstInputSetListener(char *listener, uint32_t max_len)
{
    CLST *clst;

    memset(listener, 0, max_len);
    clst = clstInstance();

    if (clst->input_listener)
        clstInputRemoveListener();

    clst->input_listener = listener;
    clst->input_listener_len = 0;
    clst->input_listener_max_len = max_len;
    clstKeyAddCallback((CLSTkey){CELESTE_KEY_BACKSPACE, listener_delete, NULL});
    clstWindowStartListening();
}

void clstInputRemoveListener()
{
    CLST *clst;

    clst = clstInstance();

    if (clst->input_listener == NULL)
        return;

    clstKeyRemoveCallback(CELESTE_KEY_BACKSPACE);
    clstWindowStopListening();
    clst->input_listener = NULL;
    clst->input_listener_len = 0;
    clst->input_listener_max_len = 0;
}

static void listener_delete()
{
    CLST *clst;

    clst = clstInstance();

    if (clst->input_listener_len == 0)
        return;

    if (clstKey(CELESTE_KEY_LEFT_CONTROL) || clstKey(CELESTE_KEY_RIGHT_CONTROL))
    {
        while (clst->input_listener_len && clst->input_listener[clst->input_listener_len - 1] == ' ') {
            clst->input_listener_len--;
            clst->input_listener[clst->input_listener_len] = 0;
        }

        if (clst->input_listener_len <= 0)
            return;

        do {
            clst->input_listener_len--;
            clst->input_listener[clst->input_listener_len] = 0;
        } while (clst->input_listener_len && clst->input_listener[clst->input_listener_len - 1] != ' ');
    }
    else
    {
        clst->input_listener_len--;
        clst->input_listener[clst->input_listener_len] = 0;
    }
}
