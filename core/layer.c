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

static CLSTrenderable *clstListGetRenderable(CLSTlist *list, char *name);

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
    layer->renderables = clstListCreate();
    layer->camera = camera;
    layer->name = strdup(name);

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
    clstListDestroy(layer->renderables, (CLSTitemdestroy)clstRenderableDestroy);
    free(layer);
}

void clstLayerAddRenderable(CLSTlayer *layer, void *renderable)
{
    clstListAdd(layer->renderables, renderable);
}

CLSTrenderable *clstListGetRenderable(CLSTlist *list, char *name)
{
    CLSTrenderable **renderables, *renderable;
    CLSTrenderabletype type;

    renderables = (CLSTrenderable **)list->items;
    for (int i = 0; i < list->count; i++)
    {
        if (strcmp(renderables[i]->name, name) == 0)
            return renderables[i];

        type = clstRenderableType(renderables[i]);
        if (type == CELESTE_RENDERABLE_GROUP)
        {
            renderable = clstListGetRenderable(((CLSTgroup *)renderables[i])->renderables, name);
            if (renderable != NULL)
                return renderable;
        }
        else if (type == CELESTE_RENDERABLE_BUTTON)
        {
            renderable = (CLSTrenderable *)((CLSTbutton *)renderables[i])->sprite;
            if (strcmp(renderable->name, name) == 0)
                return renderable;
        }
    }
    return NULL;
}

CLSTrenderable *clstLayerGetRenderable(CLSTlayer *layer, char *name)
{
    CLSTrenderable *renderable;

    renderable = clstListGetRenderable(layer->renderables, name);
    if (renderable == NULL) {
        CELESTE_LOG("Renderable `%s` doesn't exist in the layer `%s`!", name, layer->name);
        CELESTE_LOG_ERROR("Renderable doesn't exist!");
    }
    return renderable;
}

void clstLayerRender(CLSTlayer *layer)
{
    mat4 view;
    CLSTrenderable *renderable;

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

    for (int i = 0; i < layer->renderables->count; i++) {
        renderable = (CLSTrenderable *)layer->renderables->items[i];
        renderable->draw(layer->renderer, renderable);
    }

    if (layer->camera)
        clstRendererMat4Pop(layer->renderer);
    clstRendererEnd();
    clstRendererFlush(layer->renderer);
}
