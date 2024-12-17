#ifndef __CELESTE_H__
#define __CELESTE_H__

#include "common.h"
#include "gapi.h"
#include "input.h"
#include "physics.h"
#include "renderer.h"
#include "scene.h"
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

    CLSTkey      *keys;
    CLSTclick    *clicks;
    int           keys_count;
    int           clicks_count;

    char         *input_listener;
    int           input_listener_len;
    int           input_listener_max_len;
    double     **scroll_listeners;
    int          scroll_listeners_count;

    int          bodies_count;
    CLSTbody   **bodies;
    double       last_physics_update;
    float        world_gravity;

    void        *aumanager;
    void        *aumixer;
    pthread_t    audio_thread;

    void        *loader;
} CLST;

/*
 * Initializes celeste.
 */
CLST *clstInit();

/*
 * Terminates celeste.
 */
void clstTerminate();

/*
 * Returns the current celeste instance.
 */
CLST *clstInstance();

/*
 * Returns the time elapsed from initializing celeste.
 */
double clstTime();

/*
 * Swaps buffers, and handles the window's events.
 */
void clstUpdate(CLST *celeste);

/*
 * Swaps buffers, and waits for a window event.
 */
void clstWaitEv(CLST *celeste);

#endif /* __CELESTE_H__ */
