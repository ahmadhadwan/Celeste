#ifndef __CELESTE_INPUT_H__
#define __CELESTE_INPUT_H__

#define CELESTE_CURSOR_ARROW         0x00036001
#define CELESTE_CURSOR_IBEAM         0x00036002
#define CELESTE_CURSOR_CROSSHAIR     0x00036003
#define CELESTE_CURSOR_HAND          0x00036004
#define CELESTE_CURSOR_HRESIZE       0x00036005
#define CELESTE_CURSOR_VRESIZE       0x00036006

#define CELESTE_KEY_SPACE            32
#define CELESTE_KEY_APOSTROPHE       39
#define CELESTE_KEY_COMMA            44
#define CELESTE_KEY_MINUS            45
#define CELESTE_KEY_PERIOD           46
#define CELESTE_KEY_SLASH            47
#define CELESTE_KEY_0                48
#define CELESTE_KEY_1                49
#define CELESTE_KEY_2                50
#define CELESTE_KEY_3                51
#define CELESTE_KEY_4                52
#define CELESTE_KEY_5                53
#define CELESTE_KEY_6                54
#define CELESTE_KEY_7                55
#define CELESTE_KEY_8                56
#define CELESTE_KEY_9                57
#define CELESTE_KEY_SEMICOLON        59
#define CELESTE_KEY_EQUAL            61
#define CELESTE_KEY_A                65
#define CELESTE_KEY_B                66
#define CELESTE_KEY_C                67
#define CELESTE_KEY_D                68
#define CELESTE_KEY_E                69
#define CELESTE_KEY_F                70
#define CELESTE_KEY_G                71
#define CELESTE_KEY_H                72
#define CELESTE_KEY_I                73
#define CELESTE_KEY_J                74
#define CELESTE_KEY_K                75
#define CELESTE_KEY_L                76
#define CELESTE_KEY_M                77
#define CELESTE_KEY_N                78
#define CELESTE_KEY_O                79
#define CELESTE_KEY_P                80
#define CELESTE_KEY_Q                81
#define CELESTE_KEY_R                82
#define CELESTE_KEY_S                83
#define CELESTE_KEY_T                84
#define CELESTE_KEY_U                85
#define CELESTE_KEY_V                86
#define CELESTE_KEY_W                87
#define CELESTE_KEY_X                88
#define CELESTE_KEY_Y                89
#define CELESTE_KEY_Z                90
#define CELESTE_KEY_LEFT_BRACKET     91
#define CELESTE_KEY_BACKSLASH        92
#define CELESTE_KEY_RIGHT_BRACKET    93
#define CELESTE_KEY_GRAVE_ACCENT     96
#define CELESTE_KEY_WORLD_1          161
#define CELESTE_KEY_WORLD_2          162
#define CELESTE_KEY_ESCAPE           256
#define CELESTE_KEY_ENTER            257
#define CELESTE_KEY_TAB              258
#define CELESTE_KEY_BACKSPACE        259
#define CELESTE_KEY_INSERT           260
#define CELESTE_KEY_DELETE           261
#define CELESTE_KEY_RIGHT            262
#define CELESTE_KEY_LEFT             263
#define CELESTE_KEY_DOWN             264
#define CELESTE_KEY_UP               265
#define CELESTE_KEY_PAGE_UP          266
#define CELESTE_KEY_PAGE_DOWN        267
#define CELESTE_KEY_HOME             268
#define CELESTE_KEY_END              269
#define CELESTE_KEY_CAPS_LOCK        280
#define CELESTE_KEY_SCROLL_LOCK      281
#define CELESTE_KEY_NUM_LOCK         282
#define CELESTE_KEY_PRINT_SCREEN     283
#define CELESTE_KEY_PAUSE            284
#define CELESTE_KEY_F1               290
#define CELESTE_KEY_F2               291
#define CELESTE_KEY_F3               292
#define CELESTE_KEY_F4               293
#define CELESTE_KEY_F5               294
#define CELESTE_KEY_F6               295
#define CELESTE_KEY_F7               296
#define CELESTE_KEY_F8               297
#define CELESTE_KEY_F9               298
#define CELESTE_KEY_F10              299
#define CELESTE_KEY_F11              300
#define CELESTE_KEY_F12              301
#define CELESTE_KEY_F13              302
#define CELESTE_KEY_F14              303
#define CELESTE_KEY_F15              304
#define CELESTE_KEY_F16              305
#define CELESTE_KEY_F17              306
#define CELESTE_KEY_F18              307
#define CELESTE_KEY_F19              308
#define CELESTE_KEY_F20              309
#define CELESTE_KEY_F21              310
#define CELESTE_KEY_F22              311
#define CELESTE_KEY_F23              312
#define CELESTE_KEY_F24              313
#define CELESTE_KEY_F25              314
#define CELESTE_KEY_KP_0             320
#define CELESTE_KEY_KP_1             321
#define CELESTE_KEY_KP_2             322
#define CELESTE_KEY_KP_3             323
#define CELESTE_KEY_KP_4             324
#define CELESTE_KEY_KP_5             325
#define CELESTE_KEY_KP_6             326
#define CELESTE_KEY_KP_7             327
#define CELESTE_KEY_KP_8             328
#define CELESTE_KEY_KP_9             329
#define CELESTE_KEY_KP_DECIMAL       330
#define CELESTE_KEY_KP_DIVIDE        331
#define CELESTE_KEY_KP_MULTIPLY      332
#define CELESTE_KEY_KP_SUBTRACT      333
#define CELESTE_KEY_KP_ADD           334
#define CELESTE_KEY_KP_ENTER         335
#define CELESTE_KEY_KP_EQUAL         336
#define CELESTE_KEY_LEFT_SHIFT       340
#define CELESTE_KEY_LEFT_CONTROL     341
#define CELESTE_KEY_LEFT_ALT         342
#define CELESTE_KEY_LEFT_SUPER       343
#define CELESTE_KEY_RIGHT_SHIFT      344
#define CELESTE_KEY_RIGHT_CONTROL    345
#define CELESTE_KEY_RIGHT_ALT        346
#define CELESTE_KEY_RIGHT_SUPER      347
#define CELESTE_KEY_MENU             348

#define CELESTE_MOUSE_LEFT           0x00000000
#define CELESTE_MOUSE_RIGHT          0x00000001
#define CELESTE_MOUSE_MIDDLE         0x00000002

#define CELESTE_CURSOR_MODE_NORMAL   0x00034001
#define CELESTE_CURSOR_MODE_DISABLED 0x00034002
#define CELESTE_CURSOR_MODE_HIDDEN   0x00034003

typedef void (*CLSTfunction)(void *arg);

typedef struct {
    uint32_t     key;
    CLSTfunction function;
    void        *arg;
} CLSTkey;

typedef struct {
    uint32_t     click;
    CLSTfunction function;
    void         *arg;
} CLSTclick;

/*
 * Adds a key callback.
 */
void clstKeyAddCallback(CLSTkey key);

/*
 * Removes a key callback with the specified key.
 */
void clstKeyRemoveCallback(uint32_t key);

/*
 * Returns 1 if the key was pressed.
 */
uint32_t clstKey(uint32_t key);

/*
 * Adds a mouse click callback.
 */
void clstClickAddCallback(CLSTclick click);

/*
 * Removes a mouse click callback with the specified click.
 */
void clstClickRemoveCallback(uint32_t click);

/*
 * Returns 1 if the mouse button was pressed.
 */
uint32_t clstClick(uint32_t click);

/*
 * Adds a scroll listener.
 */
void clstScrollAddListener(double *listener);

/*
 * Removes the specified scroll listener.
 */
void clstScrollRemoveListener(double *listener);

/*
 * Sets an input listener.
 */
void clstInputSetListener(char *listener, uint32_t max_len);

/*
 * Remove the current input listener.
 */
void clstInputRemoveListener();

#endif /* __CELESTE_INPUT_H__ */
