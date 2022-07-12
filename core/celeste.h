#ifndef __CELESTE_H__
#define __CELESTE_H__

#include "common.h"
#include "gapi.h"
#include "renderer.h"
#include "scene.h"

#ifdef CELESTE_GLFW
    #include <GLFW/glfw3.h>
#else
    #error "Define CELESTE_GLFW macro!"
#endif /* CELESTE_GLFW */

#ifdef CELESTE_GORILLA_AUDIO
    #include <gorilla/ga.h>
    #include <gorilla/gau.h>
#else
    #error "Define CELESTE_GORILLA_AUDIO macro!"
#endif /* CELESTE_GORILLA_AUDIO */

#ifdef CELESTE_AUDIO_ASYNC
    #ifdef CELESTE_PTHREAD
        #include <pthread.h>
    #else
        #ifdef _WIN64
            #include <windows.h>
            #define CELESTE_WINTHREAD
        #else
            #error "Define CELESTE_PTHREAD macro!"
        #endif /* _WIN64 */
    #endif /* CELESTE_PTHREAD */
#endif /* CELESTE_AUDIO_ASYNC */

typedef struct {
#ifdef CELESTE_GLFW
    GLFWwindow *window;
#endif /* CELESTE_GLFW */
    char *wintitle;
    int   winalive;
    int   winfocused;
    int   winwidth;
    int   winheight;
    struct {
        double x;
        double y;
    } wincursor;

    celeste_renderer_t *default_renderer;
    celeste_shader_t   *default_shader;
    celeste_scene_t    *scene;

#ifdef CELESTE_GORILLA_AUDIO
    gau_Manager *aumanager;
    ga_Mixer    *aumixer;
#endif /* CELESTE_GORILLA_AUDIO */
#ifdef CELESTE_AUDIO_ASYNC
    #ifdef CELESTE_PTHREAD
        pthread_t audio_thread;
    #elif defined(CELESTE_WINTHREAD) /* CELESTE_PTHREAD  */
        HANDLE audio_thread;
    #endif /* CELESTE_WINTHREAD */
#endif /* CELESTE_AUDIO_ASYNC */
} celeste_t;

celeste_t *celeste_init();
void celeste_terminate();

celeste_t *celeste_get_instance();
double celeste_get_time();

void celeste_update(celeste_t *celeste);
void celeste_wait_event(celeste_t *celeste);

#endif /* __CELESTE_H__ */
