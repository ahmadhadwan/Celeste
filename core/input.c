#include "internal/window.h"

#include "celeste.h"
#include <string.h>
#include "window.h"

static void listener_delete();

void clstKeyAddCallback(CLSTkey key)
{
    CLST *celeste;

    celeste = clstInstance();
    celeste->keys = realloc(celeste->keys, (celeste->keys_count + 1) * sizeof(CLSTkey));
    celeste->keys[celeste->keys_count] = key;
    celeste->keys_count++;
}

void clstKeyRemoveCallback(int key)
{
    CLST *celeste;

    celeste = clstInstance();
    for (int i = 0; i < celeste->keys_count; i++)
    {
        if (celeste->keys[i].key == key)
        {
            memmove(celeste->keys + i, celeste->keys + i + 1, (celeste->keys_count - i) * sizeof(CLSTkey));
            celeste->keys = realloc(celeste->keys, (celeste->keys_count - 1) * sizeof(CLSTkey));
            celeste->keys_count--;
            break;
        }
    }
}

int clstKey(int key)
{
    return glfwGetKey(clstInstance()->window.window, key);
}

void clstClickAddCallback(CLSTclick click)
{
    CLST *celeste;

    celeste = clstInstance();
    celeste->clicks = realloc(celeste->clicks, (celeste->clicks_count + 1) * sizeof(CLSTclick));
    celeste->clicks[celeste->clicks_count] = click;
    celeste->clicks_count++;
}

void clstClickRemoveCallback(int click)
{
    CLST *celeste;

    celeste = clstInstance();
    for (int i = 0; i < celeste->clicks_count; i++)
    {
        if (celeste->clicks[i].click == click)
        {
            memmove(celeste->clicks + i, celeste->clicks + i + 1, (celeste->clicks_count - i - 1) * sizeof(CLSTclick));
            celeste->clicks = realloc(celeste->clicks, (celeste->clicks_count - 1) * sizeof(CLSTclick));
            celeste->clicks_count--;
            break;
        }
    }
}

int clstClick(int click)
{
    return glfwGetMouseButton(clstInstance()->window.window, click);
}

void clstScrollAddListener(double *listener)
{
    CLST *celeste;

    celeste = clstInstance();
    celeste->scroll_listeners = realloc(celeste->scroll_listeners , (celeste->scroll_listeners_count + 1) * sizeof(double *));
    celeste->scroll_listeners[celeste->scroll_listeners_count] = listener;
    celeste->scroll_listeners_count++;
}

void clstScrollRemoveListener(double *listener)
{
    CLST *celeste;

    celeste = clstInstance();
    for (int i = 0; i < celeste->scroll_listeners_count; i++)
    {
        if (celeste->scroll_listeners[i] == listener)
        {
            memmove(celeste->scroll_listeners + i, celeste->scroll_listeners + i + 1, (celeste->scroll_listeners_count - i) * sizeof(double *));
            celeste->scroll_listeners = realloc(celeste->scroll_listeners, (celeste->scroll_listeners_count - 1) * sizeof(double *));
            celeste->scroll_listeners_count--;
            break;
        }
    }
}

void clstInputSetListener(char *listener, int max_len)
{
    CLST *celeste;

    memset(listener, 0, max_len);
    celeste = clstInstance();

    if (celeste->input_listener)
        clstInputRemoveListener();

    celeste->input_listener = listener;
    celeste->input_listener_len = 0;
    celeste->input_listener_max_len = max_len;
    clstKeyAddCallback((CLSTkey){CELESTE_KEY_BACKSPACE, listener_delete, NULL});
    clstWindowStartListening();
}

void clstInputRemoveListener()
{
    CLST *celeste;

    celeste = clstInstance();

    if (celeste->input_listener == NULL)
        return;

    clstKeyRemoveCallback(CELESTE_KEY_BACKSPACE);
    clstWindowStopListening();
    celeste->input_listener = NULL;
    celeste->input_listener_len = 0;
    celeste->input_listener_max_len = 0;
}

void listener_delete()
{
    CLST *celeste;

    celeste = clstInstance();

    if (celeste->input_listener_len <= 0)
        return;

    if (clstKey(CELESTE_KEY_LEFT_CONTROL) || clstKey(CELESTE_KEY_RIGHT_CONTROL))
    {
        while (celeste->input_listener_len && celeste->input_listener[celeste->input_listener_len - 1] == ' ') {
            celeste->input_listener_len--;
            celeste->input_listener[celeste->input_listener_len] = 0;
        }

        if (celeste->input_listener_len <= 0)
            return;

        do {
            celeste->input_listener_len--;
            celeste->input_listener[celeste->input_listener_len] = 0;
        } while (celeste->input_listener_len && celeste->input_listener[celeste->input_listener_len - 1] != ' ');
    }
    else
    {
        celeste->input_listener_len--;
        celeste->input_listener[celeste->input_listener_len] = 0;
    }
}
