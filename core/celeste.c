#include "internal/audio.h"
#include "internal/window.h"

#include "celeste.h"
#include <stdio.h>
#include <stdlib.h>

static CLST *celeste_instance = NULL;

#ifdef CELESTE_PTHREAD
    static void *audio_manager_update(void *arg);
#elif defined(CELESTE_WINTHREAD) /* CELESTE_PTHREAD */
    static DWORD WINAPI audio_manager_update(LPVOID arg);
#endif /* CELESTE_WINTHREAD */

void clstPhysicsUpdate(CLST *celeste);

CLST *clstInit()
{
    CLST *celeste;

    CELESTE_ASSERT(celeste_instance == NULL && "Celeste was already initialized!");

#if defined(__linux__) && defined(CELESTE_LINUX_FORCE_NVIDIA)
    setenv("__NV_PRIME_RENDER_OFFLOAD", "1", 0);
    setenv("__GLX_VENDOR_LIBRARY_NAME", "nvidia", 0);
#endif /* __linux__ */

    if (!glfwInit()) {
        CELESTE_LOG_ERROR("Failed to initialize GLFW!");
        return NULL;
    }

    celeste = malloc(sizeof(CLST));
    celeste->window.window = NULL;
    celeste->window.title = NULL;
    celeste->window.alive = celeste->window.focused = celeste->window.height = celeste->window.width = 0;
    celeste->window.cursor.x = celeste->window.cursor.y = 0.0;

    celeste->keys = malloc(0);
    celeste->keys_count = 0;

    celeste->clicks = malloc(0);
    celeste->clicks_count = 0;

    celeste->input_listener = NULL;
    celeste->input_listener_len = 0;
    celeste->input_listener_max_len = 0;

    celeste->scroll_listeners = malloc(0);
    celeste->scroll_listeners_count = 0;

    celeste->bodies_count = 0;
    celeste->bodies = malloc(0);
    celeste->last_physics_update = 0.0;
    celeste->world_gravity = 0.0;

    gc_initialize(0);
    celeste->aumanager = gau_manager_create();
    celeste->aumixer = gau_manager_mixer(celeste->aumanager);
    celeste_instance = celeste;

#ifdef CELESTE_PTHREAD
    pthread_create(&(celeste->audio_thread), NULL, &audio_manager_update, NULL);
#elif defined(CELESTE_WINTHREAD) /* CELESTE_PTHREAD */
    celeste->audio_thread = CreateThread(NULL, 0, audio_manager_update, NULL, 0, NULL);
#endif /* CELESTE_WINTHREAD */

    return celeste;
}

void clstTerminate()
{
    CLST *celeste;
    void *aumanager;

    celeste = clstInstance();
    aumanager = celeste->aumanager;

    celeste->aumanager = NULL;
#ifdef CELESTE_PTHREAD
    pthread_join(celeste->audio_thread, NULL);
#elif defined(CELESTE_WINTHREAD) /* CELESTE_PTHREAD */
    WaitForSingleObject(celeste->audio_thread, INFINITE);
#endif /* CELESTE_WINTHREAD */

    gau_manager_destroy(aumanager);
    gc_shutdown();

    clstShaderDestroy(celeste->default_shader);
    clstRendererDestroy(celeste->default_renderer);

    glfwTerminate();

    free(celeste->bodies);
    free(celeste->scroll_listeners);
    free(celeste->clicks);
    free(celeste->keys);
    free(celeste);
    celeste_instance = NULL;
}

CLST *clstInstance()
{
    CELESTE_ASSERT(celeste_instance && "Celeste isn't initialized!");
    return celeste_instance;
}

double clstTime()
{
    return glfwGetTime();
}

void clstUpdate(CLST *celeste)
{
    glfwSwapBuffers(celeste->window.window);
    glfwPollEvents();

    clstPhysicsUpdate(celeste);
}

void clstWaitEv(CLST *celeste)
{
    glfwSwapBuffers(celeste->window.window);
    glfwWaitEvents();
}

#ifdef CELESTE_PTHREAD
    void *audio_manager_update(void *arg)
#elif defined(CELESTE_WINTHREAD) /* CELESTE_PTHREAD */
    DWORD WINAPI audio_manager_update(LPVOID arg)
#endif /* CELESTE_WINTHREAD */
{
    CLST *celeste;

    celeste = clstInstance();
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