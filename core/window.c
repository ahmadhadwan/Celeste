#include <glad/glad.h>
#include "internal/window.h"

#include "celeste.h"
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "window.h"

#include "../res/icons/celeste_48x48.c"
#include "../res/shaders/default.c"

static void window_close_callback    (GLFWwindow *window);
static void window_resize_callback   (GLFWwindow *window, int width, int height);
static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
static void cursor_position_callback (GLFWwindow *window, double xpos, double ypos);
static void window_focus_callback    (GLFWwindow *window, int focused);
static void key_callback             (GLFWwindow *window, int key, int scancode, int action, int mods);
static void mouse_button_callback    (GLFWwindow *window, int button, int action, int mods);
static void character_callback       (GLFWwindow *window, unsigned int codepoint);
static void mouse_scroll_callback    (GLFWwindow* window, double xoffset, double yoffset);

CLSTresult clstWindow(const char *title)
{
    CLST *celeste;
    GLFWmonitor *glfw_monitor;
    const GLFWvidmode *glfw_vidmode;
    GLFWwindow *window;
    GLFWimage icon;
    int icon_bpp;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    glfw_monitor = glfwGetPrimaryMonitor();
    glfw_vidmode = glfwGetVideoMode(glfw_monitor);

    celeste = clstInstance();
    celeste->window.width = glfw_vidmode->width;
    celeste->window.height = glfw_vidmode->height;

    window = glfwCreateWindow(glfw_vidmode->width, glfw_vidmode->height, title, glfw_monitor, NULL);
    if (window == NULL) {
        CELESTE_LOG_ERROR("Failed to create GLFW window!");
        clstTerminate();
        return 1;
    }

    celeste->window.window = window;

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
        clstTerminate();
        return 1;
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

    celeste->window.title = title;
    celeste->window.alive = 1;
    celeste->window.focused = 1;
    celeste->default_renderer = clstRenderer();
    celeste->default_shader = clstShaderConstSrc((const char *)default_shader_src);
    glfwShowWindow(window);
    return CELESTE_OK;
}

void clstWindowDestroy()
{
    glfwDestroyWindow(clstInstance()->window.window);
}

void clstWindowClear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void clstWindowClearColor(unsigned int color)
{
    glClearColor((color & 0x000000FF)        / 255.0f,
                ((color & 0x0000FF00) >> 8)  / 255.0f,
                ((color & 0x00FF0000) >> 16) / 255.0f,
                ((color & 0xFF000000) >> 24) / 255.0f);
}

unsigned int clstWindowGetClearColor()
{
    unsigned int color;
    float channels[4];

    glGetFloatv(GL_COLOR_CLEAR_VALUE, channels);
    color = (unsigned int)(channels[0] * 255)
          | (unsigned int)(channels[1] * 255) << 8
          | (unsigned int)(channels[2] * 255) << 16
          | (unsigned int)(channels[3] * 255) << 24;

    return color;
}

void clstWindowSwapInterval(int interval)
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
    glfwSetWindowIcon(clstInstance()->window.window, 1, &icon);
    free(icon.pixels);
}

void clstWindowSystemCursor(unsigned int cursor)
{
    glfwSetCursor(clstInstance()->window.window, glfwCreateStandardCursor(cursor));
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

void clstWindowCursor(CLSTcursor *cursor)
{
    glfwSetCursor(clstInstance()->window.window, (GLFWcursor *)cursor);
}

unsigned char *clstWindowBufferRead(unsigned int *width, unsigned int *height)
{
    CLST *celeste;
    unsigned char *pixels;

    celeste = clstInstance();
    pixels = malloc(4 * celeste->window.fb.width * celeste->window.fb.height);

    glReadPixels(0, 0, celeste->window.fb.width, celeste->window.fb.height, GL_RGBA, GL_UNSIGNED_BYTE, (void *)pixels);
    return pixels;
}

void clstWindowCursorMode(int mode)
{
    glfwSetInputMode(clstInstance()->window.window, GLFW_CURSOR, mode);
}

void clstWindowStartListening()
{
    glfwSetCharCallback(clstInstance()->window.window, character_callback);
}

void clstWindowStopListening()
{
    glfwSetCharCallback(clstInstance()->window.window, NULL);
}

void window_close_callback(GLFWwindow *window)
{
    clstInstance()->window.alive = 0;
}

void window_resize_callback(GLFWwindow *window, int width, int height)
{
    CLST *celeste;

    celeste = clstInstance();
    celeste->window.width = width;
    celeste->window.height = height;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    CLST *celeste;

    celeste = clstInstance();
    celeste->window.fb.width = width;
    celeste->window.fb.height = height;
    glViewport(0, 0, width, height);
}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos)
{
    CLST* celeste;

    celeste = clstInstance();
    celeste->window.cursor.x = xpos;
    celeste->window.cursor.y = ypos;
}

void window_focus_callback(GLFWwindow *window, int focused)
{
    clstInstance()->window.focused = focused;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    CLST* celeste;

    celeste = clstInstance();
    for (int i = 0; i < celeste->keys_count; i++)
    {
        if (celeste->keys[i].key == key && action != GLFW_RELEASE)
        {
            celeste->keys[i].function(celeste->keys[i].arg);
            break;
        }
    }
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    CLST* celeste;

    celeste = clstInstance();
    for (int i = 0; i < celeste->clicks_count; i++)
    {
        if (celeste->clicks[i].click == button && action != GLFW_RELEASE)
        {
            celeste->clicks[i].function(celeste->clicks[i].arg);
            break;
        }
    }
}

void character_callback(GLFWwindow *window, unsigned int codepoint)
{
    CLST *celeste;

    celeste = clstInstance();
    if (celeste->input_listener_len < celeste->input_listener_max_len - 1)
    {
        celeste->input_listener[celeste->input_listener_len] = (char)codepoint;
        celeste->input_listener_len++;
    }
}

void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    CLST* celeste;

    celeste = clstInstance();
    for (int i = 0; i < celeste->scroll_listeners_count; i++) {
        *(celeste->scroll_listeners[i]) += yoffset;
    }
}
