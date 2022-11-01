#ifndef __CELESTE_INTERNAL_WINDOW_H__
#define __CELESTE_INTERNAL_WINDOW_H__

#ifdef CELESTE_GLFW
    #include <GLFW/glfw3.h>
#else
    #error "Define CELESTE_GLFW macro!"
#endif /* CELESTE_GLFW */

void clstWindowStartListening();
void clstWindowStopListening();

#endif /* __CELESTE_INTERNAL_WINDOW_H__ */
