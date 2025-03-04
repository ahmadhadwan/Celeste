#include <glad/glad.h>
#include "internal/window.h"
#include "internal/celeste.h"

#include "input.h"
#include <stb/stb_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "../res/icons/celeste_48x48.c"

static void window_close_callback    (GLFWwindow *window);
static void window_resize_callback   (GLFWwindow *window, int width, int height);
static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
static void cursor_position_callback (GLFWwindow *window, double xpos, double ypos);
static void window_focus_callback    (GLFWwindow *window, int focused);
static void key_callback             (GLFWwindow *window, int key, int scancode, int action, int mods);
static void mouse_button_callback    (GLFWwindow *window, int button, int action, int mods);
static void character_callback       (GLFWwindow *window, unsigned int codepoint);
static void mouse_scroll_callback    (GLFWwindow* window, double xoffset, double yoffset);

CLSTresult clstWindowCreate(void *clstptr, const char *title)
{
    CLST *clst;
    GLFWmonitor *glfw_monitor;
    const GLFWvidmode *glfw_vidmode;
    GLFWwindow *window;
    GLFWimage icon;
    int icon_bpp;

    if (!glfwInit()) {
        CELESTE_LOG_ERROR("Failed to initialize GLFW!");
        return !CELESTE_OK;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    glfw_monitor = glfwGetPrimaryMonitor();
    glfw_vidmode = glfwGetVideoMode(glfw_monitor);

    clst = (CLST *)clstptr;
    clst->window.width = glfw_vidmode->width;
    clst->window.height = glfw_vidmode->height;

    window = glfwCreateWindow(glfw_vidmode->width, glfw_vidmode->height, title, glfw_monitor, NULL);
    if (window == NULL) {
        CELESTE_LOG_ERROR("Failed to create GLFW window!");
        glfwTerminate();
        return !CELESTE_OK;
    }

    clst->window.glfw_window = window;

    icon.pixels = stbi_load_from_memory(celeste_48x48_png,
            sizeof(celeste_48x48_png) / sizeof(unsigned char),
            &icon.width, &icon.height, &icon_bpp, STBI_rgb_alpha);
    glfwSetWindowIcon(window, 1, &icon);
    free(icon.pixels);

    glfwMakeContextCurrent(window);
    glfwSetWindowCloseCallback(window, window_close_callback);
    glfwSetWindowSizeCallback(window, window_resize_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetWindowFocusCallback(window, window_focus_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, mouse_scroll_callback);
    glfwSwapInterval(0);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        CELESTE_LOG_ERROR("Failed to initialize GLAD!");
        clstWindowDestroy();
        return !CELESTE_OK;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    CELESTE_LOG("OpenGL:\n"
                "  Vendor: %s\n"
                "  Renderer: %s\n"
                "  Version: %d.%d\n",
                glGetString(GL_VENDOR), glGetString(GL_RENDERER),
                GLVersion.major, GLVersion.minor
    );

    clst->window.title = title;
    glfwShowWindow(window);
    return CELESTE_OK;
}

void clstWindowDestroy()
{
    glfwDestroyWindow(clstInstance()->window.glfw_window);
    glfwTerminate();
}

uint32_t clstGetWindowAlive()
{
    return !glfwWindowShouldClose(clstInstance()->window.glfw_window);
}

void clstSetWindowAlive(uint32_t alive)
{
    glfwSetWindowShouldClose(clstInstance()->window.glfw_window, !alive);
}

uint32_t clstGetWindowFocused()
{
    return glfwGetWindowAttrib(clstInstance()->window.glfw_window, GLFW_FOCUSED);
}

void clstWindowFocus()
{
    glfwFocusWindow(clstInstance()->window.glfw_window);
}

void clstWindowClear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

uint32_t clstWindowGetClearColor()
{
    uint32_t color;
    float channels[4];

    glGetFloatv(GL_COLOR_CLEAR_VALUE, channels);
    color = (uint32_t)(channels[0] * 255)
          | (uint32_t)(channels[1] * 255) << 8
          | (uint32_t)(channels[2] * 255) << 16
          | (uint32_t)(channels[3] * 255) << 24;

    return color;
}

void clstWindowSetClearColor(uint32_t color)
{
    glClearColor((color & 0x000000FF)        / 255.0f,
                ((color & 0x0000FF00) >> 8)  / 255.0f,
                ((color & 0x00FF0000) >> 16) / 255.0f,
                ((color & 0xFF000000) >> 24) / 255.0f);
}

void clstWindowSwapInterval(uint32_t interval)
{
    glfwSwapInterval(interval);
}

void clstWindowIcon(const char *filepath)
{
    struct stat s;
    GLFWimage icon;
    int icon_bpp;

    if (stat(filepath, &s)) {
        CELESTE_LOG_ERROR_PATH("Failed to load icon!", filepath);
        return;
    }

    icon.pixels = stbi_load(filepath, &icon.width, &icon.height, &icon_bpp, STBI_rgb_alpha);
    glfwSetWindowIcon(clstInstance()->window.glfw_window, 1, &icon);
    free(icon.pixels);
}

void clstWindowSystemCursor(uint32_t cursor)
{
    glfwSetCursor(clstInstance()->window.glfw_window, glfwCreateStandardCursor(cursor));
}

CLSTcursor *clstCursor(const char *filepath)
{
    GLFWimage image;
    int bpp;

    image.pixels = stbi_load(filepath, &(image.width), &(image.height), &bpp, STBI_rgb_alpha);
    return (CLSTcursor *)glfwCreateCursor(&image, 0, 0);
}

void clstCursorDestroy(CLSTcursor *cursor)
{
    glfwDestroyCursor((GLFWcursor *)cursor);
}

void clstWindowSetCursor(CLSTcursor *cursor)
{
    glfwSetCursor(clstInstance()->window.glfw_window, (GLFWcursor *)cursor);
}

uint8_t *clstWindowBufferRead(uint32_t *width, uint32_t *height)
{
    CLST *clst;
    uint8_t *pixels;

    clst = clstInstance();
    pixels = malloc(4 * clst->window.fb.width * clst->window.fb.height);

    glReadPixels(0, 0, clst->window.fb.width, clst->window.fb.height, GL_RGBA, GL_UNSIGNED_BYTE, (void *)pixels);
    return pixels;
}

void clstWindowCursorMode(uint32_t mode)
{
    glfwSetInputMode(clstInstance()->window.glfw_window, GLFW_CURSOR, mode);
}

void clstWindowStartListening()
{
    glfwSetCharCallback(clstInstance()->window.glfw_window, character_callback);
}

void clstWindowStopListening()
{
    glfwSetCharCallback(clstInstance()->window.glfw_window, NULL);
}

static void window_close_callback(GLFWwindow *window)
{
    clstSetWindowAlive(0);
}

static void window_resize_callback(GLFWwindow *window, int width, int height)
{
    CLST *clst;

    clst = clstInstance();
    clst->window.width = width;
    clst->window.height = height;
}

static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    CLST *clst;

    clst = clstInstance();
    clst->window.fb.width = width;
    clst->window.fb.height = height;
    glViewport(0, 0, width, height);
}

static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos)
{
    CLST* clst;

    clst = clstInstance();
    clst->window.cursor.x = xpos;
    clst->window.cursor.y = ypos;
}

static void window_focus_callback(GLFWwindow *window, int focused)
{
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    CLST* clst;

    clst = clstInstance();
    for (int i = 0; i < clst->keys->count; i++)
    {
        CLSTkey *_key;

        _key = ((CLSTkey **)clst->keys->items)[i];
        if (_key->key == key && action != GLFW_RELEASE)
        {
            _key->function(_key->arg);
            break;
        }
    }
}

static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    CLST* clst;

    clst = clstInstance();
    for (int i = 0; i < clst->clicks->count; i++)
    {
        CLSTclick *click;

        click = ((CLSTclick **)clst->clicks->items)[i];
        if (click->click == button && action != GLFW_RELEASE)
        {
            click->function(click->arg);
            break;
        }
    }
}

static void character_callback(GLFWwindow *window, unsigned int codepoint)
{
    CLST *clst;

    clst = clstInstance();
    if (clst->input_listener_len < clst->input_listener_max_len - 1)
    {
        clst->input_listener[clst->input_listener_len] = (char)codepoint;
        clst->input_listener_len++;
    }
}

static void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    CLST* clst;
    double *listener;

    clst = clstInstance();
    for (int i = 0; i < clst->scroll_listeners->count; i++) {
        listener = ((double **)clst->scroll_listeners->items)[i];
        *listener += yoffset;
    }
}
