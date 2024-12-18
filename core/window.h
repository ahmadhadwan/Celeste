#ifndef __CELESTE_WINDOW_H__
#define __CELESTE_WINDOW_H__

#include "common.h"

typedef struct {
    void       *window;
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
void clstWindowClearColor(uint32_t color);

/*
 * Gets the clear color.
 */
uint32_t clstWindowGetClearColor();

/*
 * Sets the window's swap interval.
 * 0 -> Disables Vsync.
 * 1 -> Enables Vsync.
 * n -> Limits the framerate to (monitor refresh rate)/n
 */
void clstWindowSwapInterval(uint32_t interval);

/*
 * Opens the icon from filepath, and
 * sets it as the window's icon.
 */
void clstWindowIcon(const char *filepath);

/*
 * Set the window's system cursor.
 */
void clstWindowSystemCursor(uint32_t cursor);

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
uint8_t *clstWindowBufferRead(uint32_t *width, uint32_t *height);

/*
 * Sets the window's cursor mode.
 *
 * - CELESTE_CURSOR_MODE_NORMAL.
 * - CELESTE_CURSOR_MODE_HIDDEN.
 * - CELESTE_CURSOR_MODE_DISABLED.
 */
void clstWindowCursorMode(uint32_t mode);

#endif /* __CELESTE_WINDOW_H__ */
