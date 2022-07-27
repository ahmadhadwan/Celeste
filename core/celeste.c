#include "celeste.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

static celeste_t *celeste_instance = NULL;

#ifdef CELESTE_AUDIO_ASYNC
    #ifdef CELESTE_PTHREAD
        static void *audio_manager_update(void *arg);
    #elif defined(CELESTE_WINTHREAD) /* CELESTE_PTHREAD */
        static DWORD WINAPI audio_manager_update(LPVOID arg);
    #endif /* CELESTE_WINTHREAD */
#endif /* CELESTE_AUDIO_ASYNC */

celeste_t *celeste_init()
{
    celeste_t *celeste;

    CELESTE_ASSERT(celeste_instance == NULL && "Celeste was already initialized!");

    if (!glfwInit()) {
        CELESTE_LOG_ERROR("Failed to initialize GLFW!");
        return NULL;
    }

    celeste = malloc(sizeof(celeste_t));
    celeste->window = NULL;
    celeste->winalive = celeste->winheight = celeste->winwidth = 0;
    celeste->wincursor.x = celeste->wincursor.y = 0.0;

    celeste->keys = malloc(0);
    celeste->keys_count = 0;

    celeste->buttons = malloc(0);
    celeste->buttons_count = 0;

    celeste->input_listener = NULL;
    celeste->input_listener_len = 0;
    celeste->input_listener_max_len = 0;

    celeste->scroll_listeners = malloc(0);
    celeste->scroll_listeners_count = 0;

    gc_initialize(0);
    celeste->aumanager = gau_manager_create();
    celeste->aumixer = gau_manager_mixer(celeste->aumanager);
    celeste_instance = celeste;

#ifdef CELESTE_AUDIO_ASYNC
    #ifdef CELESTE_PTHREAD
        pthread_create(&(celeste->audio_thread), NULL, &audio_manager_update, NULL);
    #elif defined(CELESTE_WINTHREAD) /* CELESTE_PTHREAD */
        celeste->audio_thread = CreateThread(NULL, 0, audio_manager_update, NULL, 0, NULL);
    #endif /* CELESTE_WINTHREAD */
#endif /* CELESTE_AUDIO_ASYNC */

    return celeste;
}

void celeste_terminate()
{
    celeste_t *celeste;
    void *aumanager;

    celeste = celeste_get_instance();
    aumanager = celeste->aumanager;

#ifdef CELESTE_AUDIO_ASYNC
        celeste->aumanager = NULL;
    #ifdef CELESTE_PTHREAD
        pthread_join(celeste->audio_thread, NULL);
    #elif defined(CELESTE_WINTHREAD) /* CELESTE_PTHREAD */
        WaitForSingleObject(celeste->audio_thread, INFINITE);
    #endif /* CELESTE_WINTHREAD */
#endif /* CELESTE_AUDIO_ASYNC */

    gau_manager_destroy(aumanager);
    gc_shutdown();

    celeste_shader_destroy(celeste->default_shader);
    celeste_renderer_destroy(celeste->default_renderer);

    glfwDestroyWindow(celeste->window);
    glfwTerminate();

    free(celeste->scroll_listeners);
    free(celeste->buttons);
    free(celeste->keys);
    free(celeste);
    celeste_instance = NULL;
}

celeste_t *celeste_get_instance()
{
    CELESTE_ASSERT(celeste_instance && "Celeste isn't initialized!");
    return celeste_instance;
}

double celeste_get_time()
{
    return glfwGetTime();
}

void celeste_update(celeste_t *celeste)
{
    glfwSwapBuffers(celeste->window);
    glfwPollEvents();

#ifndef CELESTE_AUDIO_ASYNC
    gau_manager_update(celeste->aumanager);
#endif /* CELESTE_AUDIO_ASYNC */
}

void celeste_wait_event(celeste_t *celeste)
{
    glfwSwapBuffers(celeste->window);
    glfwWaitEvents();
}

#ifdef CELESTE_AUDIO_ASYNC
    #ifdef CELESTE_PTHREAD
        void *audio_manager_update(void *arg)
    #elif defined(CELESTE_WINTHREAD) /* CELESTE_PTHREAD */
        DWORD WINAPI audio_manager_update(LPVOID arg)
    #endif /* CELESTE_WINTHREAD */
    {
        celeste_t *celeste;

        celeste = celeste_get_instance();
        while (celeste->aumanager)
        {
            gau_manager_update(celeste->aumanager);
            gc_thread_sleep(1);
        }
    #ifdef CELESTE_PTHREAD
        return NULL;
    #elif defined(CELESTE_WINTHREAD) /* CELESTE_PTHREAD */
        return (DWORD)0;
    #endif /* CELESTE_WINTHREAD */
    }
#endif /* CELESTE_AUDIO_ASYNC */
