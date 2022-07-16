#include "celeste.h"
#include <string.h>

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
        if (celeste->keys[i].celeste_key == key)
        {
            memmove(celeste->keys + i, celeste->keys + i + 1, (celeste->keys_count - i) * sizeof(celeste_key_t));
            celeste->keys = realloc(celeste->keys, (celeste->keys_count - 1) * sizeof(celeste_key_t));
            break;
        }
    }

    celeste->keys_count--;
}

int celeste_key(int key)
{
    return glfwGetKey(celeste_get_instance()->window, key);
}
