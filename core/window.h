#ifndef __CELESTE_WINDOW_H__
#define __CELESTE_WINDOW_H__

#include "celeste.h"
#include "input.h"

typedef struct celeste_cursor celeste_cursor_t;

int celeste_window_create(celeste_t *celeste, const char *title);
void celeste_window_destroy();

void celeste_window_clear();
void celeste_window_set_clear_color(unsigned int color);
unsigned int celeste_window_get_clear_color();
void celeste_window_set_swap_interval(int interval);

void celeste_window_set_icon(const char *filepath);
void celeste_window_set_system_cursor(unsigned int cursor);
celeste_cursor_t *celeste_window_cursor_create(const char *filepath);
void celeste_window_cursor_destroy(celeste_cursor_t *cursor);
void celeste_window_set_custom_cursor(celeste_cursor_t *cursor);

void celeste_window_screenshot(celeste_t *celeste, const char *filepath);

void celeste_window_set_cursor_mode(int mode);

void celeste_window_start_listening();
void celeste_window_stop_listening();

#endif /* __CELESTE_WINDOW_H__ */
