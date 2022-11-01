#ifndef __CELESTE_WINDOW_H__
#define __CELESTE_WINDOW_H__

#include "common.h"

typedef struct {
    void       *window;
    const char *title;
    int         alive;
    int         focused;
    int         width;
    int         height;
    struct {
        double x;
        double y;
    } cursor;
    struct {
        int width;
        int height;
    } fb;
} CLSTwindow;

typedef struct CLSTcursor CLSTcursor;

/*
 * Creates a window.
 */
CLSTresult clstWindow(const char *title);

/*
 * Destroys the window, and frees its memory.
 */
void clstWindowDestroy();

/*
 * Clear the window.
 */
void clstWindowClear();

/*
 * Sets the clear color.
 */
void clstWindowClearColor(unsigned int color);

/*
 * Gets the clear color.
 */
unsigned int clstWindowGetClearColor();

/*
 * Sets the window's swap interval.
 * 0 -> Disables Vsync.
 * 1 -> Enables Vsync.
 * n -> Limits the framerate to (monitor refresh rate)/n
 */
void clstWindowSwapInterval(int interval);

/*
 * Opens the icon from filepath, and
 * sets it as the window's icon.
 */
void clstWindowIcon(const char *filepath);

/*
 * Set the window's system cursor.
 */
void clstWindowSystemCursor(unsigned int cursor);

/*
 * Opens the cursor from filepath,
 * and creates a cursor.
 *
 * returns a CLSTcursor pointer,
 * which must be destroyed by clstCursorDestroy().
 */
CLSTcursor *clstCursor(const char *filepath);

/*
 * Destroys the cursor, and frees its memory.
 */
void clstCursorDestroy(CLSTcursor *cursor);

/*
 * Sets a custom cursor to the window.
 */
void clstWindowCursor(CLSTcursor *cursor);

/*
 * Reads the window's buffer, returns the pixels in the RGBA32 format,
 * and fills the args with the buffer width and height.
 *
 * Pixels returned must be free'd.
 */
unsigned char *clstWindowBufferRead(unsigned int *width, unsigned int *height);

/*
 * Sets the window's cursor mode.
 *
 * - CELESTE_CURSOR_MODE_NORMAL.
 * - CELESTE_CURSOR_MODE_HIDDEN.
 * - CELESTE_CURSOR_MODE_DISABLED.
 */
void clstWindowCursorMode(int mode);

#endif /* __CELESTE_WINDOW_H__ */
