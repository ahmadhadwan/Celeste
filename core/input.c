#include "celeste.h"
#include <string.h>
#include "window.h"

static void listener_delete();

void celeste_key_add_callback(celeste_key_t key)
{
    celeste_t *celeste;

    celeste = celeste_get_instance();
    celeste->keys = realloc(celeste->keys, (celeste->keys_count + 1) * sizeof(celeste_key_t));
    celeste->keys[celeste->keys_count] = key;
    celeste->keys_count++;
}

void celeste_key_remove_callback(int key)
{
    celeste_t *celeste;

    celeste = celeste_get_instance();
    for (int i = 0; i < celeste->keys_count; i++)
    {
        if (celeste->keys[i].key == key)
        {
            memmove(celeste->keys + i, celeste->keys + i + 1, (celeste->keys_count - i) * sizeof(celeste_key_t));
            celeste->keys = realloc(celeste->keys, (celeste->keys_count - 1) * sizeof(celeste_key_t));
            celeste->keys_count--;
            break;
        }
    }
}

int celeste_key(int key)
{
    return glfwGetKey(celeste_get_instance()->window, key);
}

void celeste_mouse_button_add_callback(celeste_mouse_button_t button)
{
    celeste_t *celeste;

    celeste = celeste_get_instance();
    celeste->buttons = realloc(celeste->buttons, (celeste->buttons_count + 1) * sizeof(celeste_mouse_button_t));
    celeste->buttons[celeste->buttons_count] = button;
    celeste->buttons_count++;
}

void celeste_mouse_button_remove_callback(int button)
{
    celeste_t *celeste;

    celeste = celeste_get_instance();
    for (int i = 0; i < celeste->buttons_count; i++)
    {
        if (celeste->buttons[i].button == button)
        {
            memmove(celeste->buttons + i, celeste->buttons + i + 1, (celeste->buttons_count - i) * sizeof(celeste_mouse_button_t));
            celeste->buttons = realloc(celeste->buttons, (celeste->buttons_count - 1) * sizeof(celeste_mouse_button_t));
            celeste->buttons_count--;
            break;
        }
    }
}

int celeste_mouse_button(int button)
{
    return glfwGetMouseButton(celeste_get_instance()->window, button);
}

void celeste_scroll_add_listener(double *listener)
{
    celeste_t *celeste;

    celeste = celeste_get_instance();
    celeste->scroll_listeners = realloc(celeste->scroll_listeners , (celeste->scroll_listeners_count + 1) * sizeof(double*));
    celeste->scroll_listeners[celeste->scroll_listeners_count] = listener;
    celeste->scroll_listeners_count++;
}

void celeste_scroll_remove_listener(double *listener)
{
    celeste_t *celeste;

    celeste = celeste_get_instance();
    for (int i = 0; i < celeste->scroll_listeners_count; i++)
    {
        if (celeste->scroll_listeners[i] == listener)
        {
            memmove(celeste->scroll_listeners + i, celeste->scroll_listeners + i + 1, (celeste->scroll_listeners_count - i) * sizeof(double*));
            celeste->scroll_listeners = realloc(celeste->scroll_listeners, (celeste->scroll_listeners_count - 1) * sizeof(double*));
            celeste->scroll_listeners_count--;
            break;
        }
    }
}

void celeste_input_set_listener(char *listener, int max_len)
{
    celeste_t *celeste;

    memset(listener, 0, max_len);
    celeste = celeste_get_instance();
    celeste->input_listener = listener;
    celeste->input_listener_len = 0;
    celeste->input_listener_max_len = max_len;
    celeste_key_add_callback((celeste_key_t){CELESTE_KEY_BACKSPACE, listener_delete, NULL});
    celeste_window_start_listening();
}

void celeste_input_remove_listener()
{
    celeste_t *celeste;

    celeste = celeste_get_instance();
    celeste_key_remove_callback(CELESTE_KEY_BACKSPACE);
    celeste_window_stop_listening();
    celeste->input_listener = NULL;
    celeste->input_listener_len = 0;
    celeste->input_listener_max_len = 0;
}

void listener_delete()
{
    celeste_t *celeste;

    celeste = celeste_get_instance();

    if (!celeste->input_listener_len)
        return;

    celeste->input_listener_len--;
    celeste->input_listener[celeste->input_listener_len] = 0;
}
