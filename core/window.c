#include <glad/glad.h>
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "window.h"

#define CELESTE_ICON_PATH "./res/icons/celeste_48x48.png"

static const char *default_shader_src = "#type vertex\n#version 330 core\n"
"layout(location=0)in vec4 position;layout(location=1)in vec2 uv;layout(location=2)in int tid;layout(location=3)in vec4 color;"
"out DATA{vec4 position;vec2 uv;int tid;vec4 color;}vs_out;uniform mat4 projection;"
"void main(){gl_Position=projection*position;vs_out.position=position;vs_out.uv=uv;vs_out.tid=tid;vs_out.color=color;}"
"#type fragment\n#version 460 core\n"
"layout(location=0)out vec4 color;in DATA{vec4 position;vec2 uv;flat int tid;vec4 color;}fs_in;uniform sampler2D textures[32];"
"void main(){vec4 texColor;switch(fs_in.tid){case 1:texColor=texture(textures[0],fs_in.uv);break;"
"case 2:texColor=texture(textures[1],fs_in.uv);break;case 3:texColor=texture(textures[2],fs_in.uv);break;"
"case 4:texColor=texture(textures[3],fs_in.uv);break;case 5:texColor=texture(textures[4],fs_in.uv);break;"
"case 6:texColor=texture(textures[5],fs_in.uv);break;case 7:texColor=texture(textures[6],fs_in.uv);break;"
"case 8:texColor=texture(textures[7],fs_in.uv);break;case 9:texColor=texture(textures[8],fs_in.uv);break;"
"case 10:texColor=texture(textures[9],fs_in.uv);break;case 11:texColor=texture(textures[10],fs_in.uv);break;"
"case 12:texColor=texture(textures[11],fs_in.uv);break;case 13:texColor=texture(textures[12],fs_in.uv);break;"
"case 14:texColor=texture(textures[13],fs_in.uv);break;case 15:texColor=texture(textures[14],fs_in.uv);break;"
"case 16:texColor=texture(textures[15],fs_in.uv);break;case 17:texColor=texture(textures[16],fs_in.uv);break;"
"case 18:texColor=texture(textures[17],fs_in.uv);break;case 19:texColor=texture(textures[18],fs_in.uv);break;"
"case 20:texColor=texture(textures[19],fs_in.uv);break;case 21:texColor=texture(textures[20],fs_in.uv);break;"
"case 22:texColor=texture(textures[21],fs_in.uv);break;case 23:texColor=texture(textures[22],fs_in.uv);break;"
"case 24:texColor=texture(textures[23],fs_in.uv);break;case 25:texColor=texture(textures[24],fs_in.uv);break;"
"case 26:texColor=texture(textures[25],fs_in.uv);break;case 27:texColor=texture(textures[26],fs_in.uv);break;"
"case 28:texColor=texture(textures[27],fs_in.uv);break;case 29:texColor=texture(textures[28],fs_in.uv);break;"
"case 30:texColor=texture(textures[29],fs_in.uv);break;case 31:texColor=texture(textures[30],fs_in.uv);break;"
"case 32:texColor=texture(textures[31],fs_in.uv);break;default:color=fs_in.color;return;}color=texColor*fs_in.color;}";

static void window_close_callback(GLFWwindow *glfw_window);
static void window_resize(GLFWwindow *glfw_window, int width, int height);
static void cursor_position_callback(GLFWwindow *glfw_window, double xpos, double ypos);
static void window_focus_callback(GLFWwindow* window, int focused);

int celeste_window_create(celeste_t *celeste, const char *title)
{
    GLFWmonitor *glfw_monitor;
    const GLFWvidmode *glfw_vidmode;
    GLFWwindow *window;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    glfw_monitor = glfwGetPrimaryMonitor();
    glfw_vidmode = glfwGetVideoMode(glfw_monitor);
    celeste->winwidth = glfw_vidmode->width;
    celeste->winheight = glfw_vidmode->height;

    window = glfwCreateWindow(glfw_vidmode->width, glfw_vidmode->height, title, glfw_monitor, NULL);
    if (window == NULL) {
        CELESTE_LOG_ERROR("Failed to create GLFW window!");
        celeste_terminate();
        return 1;
    }

    celeste->window = window;
    celeste_window_set_icon(CELESTE_ICON_PATH);

    glfwMakeContextCurrent(window);
    glfwSetWindowSizeCallback(window, window_resize);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetWindowCloseCallback(window, window_close_callback);
    glfwSetWindowFocusCallback(window, window_focus_callback);
    glfwSwapInterval(0);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        CELESTE_LOG_ERROR("Failed to initialize GLAD!");
        celeste_window_destroy();
        celeste_terminate();
        return 1;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    printf("%s %s\nOpenGL Version %s\n", glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_VERSION));

    celeste->winalive = 1;
    celeste->winfocused = 1;
    celeste->default_renderer = celeste_renderer_create();
    celeste->default_shader = celeste_shader_create_const_src(default_shader_src);
    glfwShowWindow(window);
    return CELESTE_OK;
}

void celeste_window_destroy()
{
    glfwDestroyWindow(celeste_get_instance()->window);
}

void celeste_window_clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void celeste_window_set_clear_color(unsigned int color)
{
    glClearColor((color & 0x000000FF)        / 255.0f,
                ((color & 0x0000FF00) >> 8)  / 255.0f,
                ((color & 0x00FF0000) >> 16) / 255.0f,
                ((color & 0xFF000000) >> 24) / 255.0f);
}

unsigned int celeste_window_get_clear_color()
{
    unsigned int color;
    float channels[4] = {};

    glGetFloatv(GL_COLOR_CLEAR_VALUE, channels);
    color = (unsigned int)(channels[0] * 255)
          | (unsigned int)(channels[1] * 255) << 8
          | (unsigned int)(channels[2] * 255) << 16
          | (unsigned int)(channels[3] * 255) << 24;

    return color;
}

void celeste_window_swap_interval(int interval)
{
    glfwSwapInterval(interval);
}

void celeste_window_set_icon(const char *filepath)
{
    struct stat s;
    GLFWimage icon;
    int icon_bpp;

    if (stat(filepath, &s)) {
        CELESTE_LOG_ERROR_PATH("Failed to load icon!", filepath);
        return;
    }

    icon.pixels = stbi_load(filepath, &icon.width, &icon.height, &icon_bpp, STBI_rgb_alpha);
    glfwSetWindowIcon(celeste_get_instance()->window, 1, &icon);
    free(icon.pixels);
}

void celeste_window_set_system_cursor(unsigned int cursor)
{
    glfwSetCursor(celeste_get_instance()->window, glfwCreateStandardCursor(cursor));
}

celeste_cursor_t *celeste_window_cursor_create(const char *filepath)
{
    GLFWimage image;
    int bpp;

    image.pixels = stbi_load(filepath, &(image.width), &(image.height), &bpp, STBI_rgb_alpha);
    return (celeste_cursor_t*)glfwCreateCursor(&image, 0, 0);
}

void celeste_window_cursor_destroy(celeste_cursor_t *cursor)
{
    glfwDestroyCursor((GLFWcursor*)cursor);
}

void celeste_window_set_custom_cursor(celeste_cursor_t *cursor)
{
    glfwSetCursor(celeste_get_instance()->window, (GLFWcursor*)cursor);
}

void celeste_window_screenshot(celeste_t* celeste, const char* filepath)
{
    unsigned char pixels[4 * celeste->winwidth * celeste->winheight];

    glReadPixels(0, 0, celeste->winwidth, celeste->winheight, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    stbi_flip_vertically_on_write(1);
    stbi_write_png(filepath, celeste->winwidth, celeste->winheight, 4, pixels, celeste->winwidth * 4);
}

int celeste_mouse_button_clicked(int button)
{
    return glfwGetMouseButton(celeste_get_instance()->window, button);
}

void celeste_window_set_cursor_mode(int mode)
{
    glfwSetInputMode(celeste_get_instance()->window, GLFW_CURSOR, mode);
}

void window_close_callback(GLFWwindow* glfw_window)
{
    celeste_get_instance()->winalive = 0;
}

void window_resize(GLFWwindow* glfw_window, int width, int height)
{
    celeste_t *celeste;

    glViewport(0, 0, width, height);

    celeste = celeste_get_instance();
    celeste->winwidth = width;
    celeste->winheight = height;
}

void cursor_position_callback(GLFWwindow* glfw_window, double xpos, double ypos)
{
    celeste_t* celeste;

    celeste = celeste_get_instance();
    celeste->wincursor.x = xpos;
    celeste->wincursor.y = ypos;
}

void window_focus_callback(GLFWwindow* window, int focused)
{
    celeste_get_instance()->winfocused = focused;
}
