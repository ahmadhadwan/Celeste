#include "celeste.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

static celeste_t *celeste_instance = NULL;

#ifdef CELESTE_AUDIO_ASYNC
    static void *audio_manager_update(void *arg);
#endif /* CELESTE_AUDIO_ASYNC */

celeste_t *celeste_init()
{
    celeste_t *celeste;

    if (celeste_instance) {
        CELESTE_LOG_ERROR("Celeste was already initialized!");
        return NULL;
    }

    if (!glfwInit()) {
        CELESTE_LOG_ERROR("Failed to initialize GLFW!");
        return NULL;
    }

    celeste = malloc(sizeof(celeste_t));
    celeste->window = NULL;
    celeste->winalive = celeste->winheight = celeste->winwidth = 0;
    celeste->wincursor.x = celeste->wincursor.y = 0.0;

    gc_initialize(0);
    celeste->aumanager = gau_manager_create();
    celeste->aumixer = gau_manager_mixer(celeste->aumanager);

#ifdef CELESTE_AUDIO_ASYNC
    pthread_create(&(celeste->audio_thread), NULL, &audio_manager_update, NULL);
#endif /* CELESTE_AUDIO_ASYNC */

    celeste_instance = celeste;
    return celeste;
}

void celeste_terminate()
{
    celeste_t *celeste;
    void *aumanager;

    celeste = celeste_get_instance();
    aumanager = celeste->aumanager;

#ifdef CELESTE_AUDIO_ASYNC
    #ifdef CELESTE_PTHREAD
        celeste->aumanager = NULL;
        pthread_join(celeste->audio_thread, NULL);
    #endif /* CELESTE_PTHREAD */
#endif /* CELESTE_AUDIO_ASYNC */

    gau_manager_destroy(aumanager);
    gc_shutdown();

    celeste_shader_destroy(celeste->default_shader);
    celeste_renderer_destroy(celeste->default_renderer);
    glfwDestroyWindow(celeste->window);
    glfwTerminate();
    free(celeste);
    celeste_instance = NULL;
}

celeste_t *celeste_get_instance()
{
    if (celeste_instance)
        return celeste_instance;

    CELESTE_LOG_ERROR("Celeste isn't initialized! aborting!");
    exit(1);
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
    void *audio_manager_update(void *arg)
    {
        celeste_t *celeste;

        celeste = celeste_get_instance();
        while (celeste->aumanager)
        {
            gau_manager_update(celeste->aumanager);
            gc_thread_sleep(1);
        }
        return NULL;
    }
#endif /* CELESTE_AUDIO_ASYNC */
