#ifndef __CELESTE_INTERNAL_WINDOW_H__
#define __CELESTE_INTERNAL_WINDOW_H__

#ifdef CELESTE_GLFW
    #include <GLFW/glfw3.h>
#else
    #error "Define CELESTE_GLFW macro!"
#endif /* CELESTE_GLFW */

typedef struct {
    GLFWwindow *glfw_window;
    const char *title;
    uint32_t    alive;
    uint32_t    focused;
    uint32_t    width;
    uint32_t    height;
    struct {
        double x;
        double y;
    } cursor;
    struct {
        uint32_t width;
        uint32_t height;
    } fb;
} CLSTwindow;

#include "../window.h"

void clstWindowStartListening();
void clstWindowStopListening();

#endif /* __CELESTE_INTERNAL_WINDOW_H__ */
