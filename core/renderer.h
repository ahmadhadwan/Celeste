#ifndef __CELESTE_RENDERER_H__
#define __CELESTE_RENDERER_H__

#include <cglm/vec2.h>
#include <cglm/mat4.h>
#include "gapi.h"
#include "sprite.h"

#ifndef __CELESTE_INTERNAL_RENDERER_H__
    typedef struct CLSTvertexdata CLSTvertexdata ;
    typedef struct CLSTrenderer CLSTrenderer ;
#endif /* __CELESTE_INTERNAL_RENDERER_H__ */

CLSTrenderer *clstRenderer();
void clstRendererDestroy(CLSTrenderer *renderer);

CLSTrenderer *clstRendererDefault();

void clstRendererMat4Push(CLSTrenderer *renderer, mat4 transformation);
void clstRendererMat4Pop(CLSTrenderer *renderer);

void clstRendererBegin(CLSTrenderer *renderer);
void clstRendererDrawQuad(CLSTrenderer *renderer, vec2 position, vec2 size, CLSTtexture *texture, vec2 uv[4], uint32_t color);
void clstRendererDrawString(CLSTrenderer *renderer, vec2 position, vec2 uv[4], CLSTfont *font, char *text, uint32_t color);
void clstRendererEnd();
void clstRendererFlush(CLSTrenderer *renderer);

#endif /* __CELESTE_RENDERER_H__ */
