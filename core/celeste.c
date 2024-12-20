#include "internal/audio.h"
#include "internal/window.h"
#include "internal/celeste.h"

#include <stdio.h>
#include <stdlib.h>

static CLST *celeste_instance = NULL;

#ifdef CELESTE_PTHREAD
    static void *audio_manager_update(void *arg);
#elif defined(CELESTE_WINTHREAD) /* CELESTE_PTHREAD */
    static DWORD WINAPI audio_manager_update(LPVOID arg);
#endif /* CELESTE_WINTHREAD */

void clstPhysicsUpdate(CLST *clst);

CLST *clstInit()
{
    CLST *clst;

    if (celeste_instance != NULL) {
        CELESTE_LOG_ERROR("Celeste was already initialized!");
        return NULL;
    }

#if defined(__linux__) && defined(CELESTE_LINUX_FORCE_NVIDIA)
    setenv("__NV_PRIME_RENDER_OFFLOAD", "1", 0);
    setenv("__GLX_VENDOR_LIBRARY_NAME", "nvidia", 0);
#endif /* __linux__ */

    if (!glfwInit()) {
        CELESTE_LOG_ERROR("Failed to initialize GLFW!");
        return NULL;
    }

    if (gc_initialize(0) != GC_SUCCESS) {
        CELESTE_LOG_ERROR("Failed to initialize Gorilla Audio!");
        return NULL;    
    }

    clst = calloc(1, sizeof(CLST));
    clst->keys = clstListCreate();
    clst->clicks = clstListCreate();
    clst->scroll_listeners = clstListCreate();
    clst->aumanager = gau_manager_create();
    clst->aumixer = gau_manager_mixer(clst->aumanager);

#ifdef CELESTE_PTHREAD
    pthread_create(&(clst->audio_thread), NULL, &audio_manager_update, NULL);
#elif defined(CELESTE_WINTHREAD) /* CELESTE_PTHREAD */
    clst->audio_thread = CreateThread(NULL, 0, audio_manager_update, NULL, 0, NULL);
#endif /* CELESTE_WINTHREAD */

    celeste_instance = clst;
    return clst;
}

void clstTerminate()
{
    CLST *clst;
    void *aumanager;

    clst = clstInstance();
    aumanager = clst->aumanager;

    clst->aumanager = NULL;
#ifdef CELESTE_PTHREAD
    pthread_join(clst->audio_thread, NULL);
#elif defined(CELESTE_WINTHREAD) /* CELESTE_PTHREAD */
    WaitForSingleObject(clst->audio_thread, INFINITE);
#endif /* CELESTE_WINTHREAD */

    gau_manager_destroy(aumanager);
    gc_shutdown();

    clstShaderDestroy(clst->default_shader);
    clstRendererDestroy(clst->default_renderer);

    glfwTerminate();

    clstListDestroy(clst->scroll_listeners, (CLSTitemdestroy)free);
    clstListDestroy(clst->clicks, (CLSTitemdestroy)free);
    clstListDestroy(clst->keys, (CLSTitemdestroy)free);
    free(clst);
    celeste_instance = NULL;
}

CLST *clstInstance()
{
    return celeste_instance;
}

CLSTscene *clstGetScene()
{
    return clstInstance()->scene;
}

void clstSetScene(CLSTscene *scene)
{
    clstInstance()->scene = scene;
}

float clstGetWorldGravity()
{
    return clstInstance()->world_gravity;
}

void clstSetWorldGravity(float gravity)
{
    clstInstance()->world_gravity = gravity;
}

void clstResetPhysics()
{
    clstInstance()->last_physics_update = clstGetTime();
}

double clstGetTime()
{
    return glfwGetTime();
}

void clstUpdate(CLST *clst)
{
    glfwSwapBuffers(clst->window.glfw_window);
    glfwPollEvents();

    clstPhysicsUpdate(clst);
}

void clstWaitEv(CLST *clst)
{
    glfwSwapBuffers(clst->window.glfw_window);
    glfwWaitEvents();
}

#ifdef CELESTE_PTHREAD
void *audio_manager_update(void *arg)
#elif defined(CELESTE_WINTHREAD) /* CELESTE_PTHREAD */
DWORD WINAPI audio_manager_update(LPVOID arg)
#endif /* CELESTE_WINTHREAD */
{
    CLST *clst;

    clst = clstInstance();
    while (clst->aumanager)
    {
        gau_manager_update(clst->aumanager);
        gc_thread_sleep(1);
    }
#ifdef CELESTE_PTHREAD
    return NULL;
#elif defined(CELESTE_WINTHREAD) /* CELESTE_PTHREAD */
    return (DWORD)0;
#endif /* CELESTE_WINTHREAD */
}