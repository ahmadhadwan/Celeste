#include "internal/gapi.h"
#include "internal/renderer.h"
#include "internal/layer.h"
#include "internal/window.h"

#include "celeste.h"
#include <cglm/cglm.h>
#include <glad/glad.h>
#include "layer.h"
#include "physics.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sprite.h"
#include "window.h"

CLSTlayer *clstLayer(float right, float top, char *name)
{
    mat4 projection;
    glm_ortho(-right, right, -top, top, 1.0f, -1.0f, projection);
    return clstLayerCustom(clstRendererDefault(), clstShaderDefault(), NULL, projection, name);
}

CLSTlayer *clstLayerMat4(mat4 projection, char *name)
{
    return clstLayerCustom(clstRendererDefault(), clstShaderDefault(), NULL, projection, name);
}

CLSTlayer *clstLayerShader(float right, float top, CLSTshader *shader, char *name)
{
    mat4 projection;
    glm_ortho(-right, right, -top, top, 1.0f, -1.0f, projection);
    return clstLayerCustom(clstRendererDefault(), shader, NULL, projection, name);
}

CLSTlayer *clstLayerCamera(CLSTcamera *camera, float right, float top, char *name)
{
    mat4 projection;
    glm_ortho(-right, right, -top, top, 1.0f, -1.0f, projection);
    return clstLayerCustom(clstRendererDefault(), clstShaderDefault(), camera, projection, name);
}

CLSTlayer *clstLayerCameraMat4(CLSTcamera *camera, mat4 projection, char *name)
{
    return clstLayerCustom(clstRendererDefault(), clstShaderDefault(), camera, projection, name);
}

CLSTlayer *clstLayerCustom(CLSTrenderer *renderer, CLSTshader *shader, CLSTcamera *camera, mat4 projection, char *name)
{
    CLSTlayer *layer;
    GLuint textures_uni;
    int textures_array[32];

    layer = malloc(sizeof(CLSTlayer));
    layer->renderer = renderer;
    layer->shader = shader;
    layer->sprites = clstListCreate();
    layer->camera = camera;
    layer->name = malloc(strlen(name) + 1);
    strcpy(layer->name, name);

    layer->projection.top    =  (1 - projection[1][3]) / projection[1][1];
    layer->projection.right  =  (1 + projection[0][3]) / projection[0][0];

    for (int i = 0; i < sizeof(textures_array) / sizeof(int); i++) {
        textures_array[i] = i;
    }

    clstShaderActivate(shader);
    textures_uni = glGetUniformLocation(shader->id, "textures");
    glUniform1iv(textures_uni, 32, textures_array);
    clstShaderUniformMat4(shader, "projection", projection);

    return layer;
}

void clstLayerDestroy(CLSTlayer *layer)
{
    free(layer->name);
    free(layer->camera);
    clstListDestroy(layer->sprites, (CLSTitemdestroy)clstSpriteDestroy);
    free(layer);
}

void clstLayerAddSprite(CLSTlayer *layer, void *sprite)
{
    clstListAdd(layer->sprites, sprite);
}

void clstLayerRender(CLSTlayer *layer)
{
    mat4 view;
    CLSTsprite *sprite;

    layer->renderer->projection_x = layer->projection.right * 2;
    layer->renderer->projection_y = layer->projection.top * 2;

    clstShaderActivate(layer->shader);
    clstRendererBegin(layer->renderer);

    if (layer->camera)
    {
        vec3 inverted_position = { -layer->camera->position[0], -layer->camera->position[1], 0.0f };
        glm_translate_make(view, inverted_position);
        clstRendererMat4Push(layer->renderer, view);
    }

    for (int i = 0; i < layer->sprites->count; i++) {
        sprite = (CLSTsprite *)layer->sprites->items[i];
        sprite->draw(layer->renderer, sprite);
    }

    if (layer->camera)
        clstRendererMat4Pop(layer->renderer);
    clstRendererEnd();
    clstRendererFlush(layer->renderer);
}
