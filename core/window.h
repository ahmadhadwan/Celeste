#ifndef __CELESTE_WINDOW_H__
#define __CELESTE_WINDOW_H__

#include "common.h"

#ifndef __CELESTE_INTERNAL_WINDOW_H__
    typedef struct CLSTwindow CLSTwindow;
#endif /* __CELESTE_INTERNAL_WINDOW_H__ */

typedef struct CLSTcursor CLSTcursor;

uint32_t clstGetWindowAlive();
void clstSetWindowAlive(uint32_t alive);

uint32_t clstGetWindowFocused();
void clstWindowFocus();

void clstWindowClear();
uint32_t clstWindowGetClearColor();
void clstWindowSetClearColor(uint32_t color);

void clstWindowSwapInterval(uint32_t interval);

void clstWindowIcon(const char *filepath);
void clstWindowSystemCursor(uint32_t cursor);

CLSTcursor *clstCursor(const char *filepath);
void clstCursorDestroy(CLSTcursor *cursor);
void clstWindowSetCursor(CLSTcursor *cursor);

uint8_t *clstWindowBufferRead(uint32_t *width, uint32_t *height);

void clstWindowCursorMode(uint32_t mode);

#endif /* __CELESTE_WINDOW_H__ */
