#ifndef __CELESTE_INPUT_H__
#define __CELESTE_INPUT_H__

typedef struct {
    int celeste_key;
    void (*function)(void *arg);
    void *arg;
} celeste_key_t;

void celeste_key_add_callback(celeste_key_t key);
void celeste_key_remove_callback(int key);

int celeste_key(int key);

#endif /* __CELESTE_INPUT_H__ */
