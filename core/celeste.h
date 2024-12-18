#ifndef __CELESTE_H__
#define __CELESTE_H__

#include "common.h"
#include "gapi.h"
#include "input.h"
#include "physics.h"
#include "renderer.h"
#include "scene.h"
#include "window.h"
#include "list.h"

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

/*
 * Initializes celeste.
 */
CLST *clstInit();

/*
 * Terminates celeste.
 */
void clstTerminate();

/*
 * Sets the current scene.
 */
void clstSetScene(CLSTscene *scene);

/*
 * Gets the current scene.
 */
CLSTscene *clstGetScene();

/*
 * Sets the world gravity.
 * This affects the physical bodies CLSTbody.
 */
void clstSetWorldGravity(float gravity);

/*
 * Gets the world gravity.
 */
float clstGetWorldGravity();

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
void clstUpdate(CLST *clst);

/*
 * Swaps buffers, and waits for a window event.
 */
void clstWaitEv(CLST *clst);

#endif /* __CELESTE_H__ */
