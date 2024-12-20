#ifndef __CELESTE_INTERNAL_H__
#define __CELESTE_INTERNAL_H__

#include "../gapi.h"
#include "../common.h"
#include "../list.h"
#include "../renderer.h"
#include "../scene.h"
#include "window.h"

#ifdef CELESTE_PTHREAD
    #include <pthread.h>
#else
    #ifdef _WIN32
        #include <windows.h>
        #define CELESTE_WINTHREAD
        #define pthread_t HANDLE
    #else
        #error "Define CELESTE_PTHREAD macro!"
    #endif /* _WIN32 */
#endif /* CELESTE_PTHREAD */

typedef struct {
    CLSTwindow    window;
    CLSTrenderer *default_renderer;
    CLSTshader   *default_shader;
    CLSTscene    *scene;

    CLSTlist     *keys;
    CLSTlist     *clicks;

    char         *input_listener;
    uint32_t      input_listener_len;
    uint32_t      input_listener_max_len;
    CLSTlist     *scroll_listeners;

    double        last_physics_update;
    float         world_gravity;

    void         *aumanager;
    void         *aumixer;
    pthread_t     audio_thread;

    void         *loader;
} CLST;

#include "../celeste.h"

#endif /* __CELESTE_INTERNAL_H__ */