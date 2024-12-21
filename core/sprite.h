#ifndef __CELESTE_SPRITE_H__
#define __CELESTE_SPRITE_H__

#include <cglm/vec2.h>
#include <cglm/vec3.h>
#include <cglm/mat4.h>
#include "gapi.h"
#include "list.h"

typedef void (*CLSTdrawfunc)(void *renderer, void *renderable);

typedef enum {
    CELESTE_BUTTON_STATUS_NONE     = 0,
    CELESTE_BUTTON_STATUS_FOCUSED  = 1,
    CELESTE_BUTTON_STATUS_CLICKED  = 2,
    CELESTE_BUTTON_STATUS_RELEASED = 3,
} CLSTbuttonstatus;

typedef enum {
    CELESTE_RENDERABLE_SPRITE = 0,
    CELESTE_RENDERABLE_LABEL  = 1,
    CELESTE_RENDERABLE_ANIMATION = 2,
    CELESTE_RENDERABLE_GROUP = 3,
    CELESTE_RENDERABLE_BUTTON = 4,
} CLSTrenderabletype;

typedef struct {
    CLSTdrawfunc draw;
    char        *name;
} CLSTrenderable;

typedef struct {
    CLSTdrawfunc draw;
    char        *name;
    vec2         position;
    vec2         size;
    vec2         uv[4];
    CLSTtexture *texture;
    uint32_t     color;
} CLSTsprite;

typedef struct {
    CLSTdrawfunc draw;
    char        *name;
    vec2         position;
    vec2         uv[4];
    CLSTfont    *font;
    char        *text;
    uint32_t     color;
} CLSTlabel;

typedef struct {
    CLSTdrawfunc draw;
    char        *name;
    uint32_t     current_frame;
    CLSTlist    *frames;
    double       frame_time;
    double       last_animation_time;
} CLSTanimation;

typedef struct {
    CLSTdrawfunc draw;
    char        *name;
    CLSTlist    *renderables;
    mat4         translation;
} CLSTgroup;

typedef struct {
    CLSTdrawfunc draw;
    char        *name;
    uint32_t     status;
    uint32_t     disabled;
    CLSTsprite  *sprite;
    mat4         translation;
} CLSTbutton;

CLSTsprite *clstSprite(vec2 position, vec2 size, CLSTtexture *texture, char *name);
CLSTsprite *clstSpriteTexAtlaspx(vec2 position, vec2 size, CLSTtexture *texture_atlas, vec2 startpx, vec2 endpx, char *name);
CLSTsprite *clstSpriteTexAtlas(vec2 position, vec2 size, CLSTtexture *texture_atlas, vec2 offset, vec2 texsize, char *name);
CLSTsprite *clstSpriteCol(vec2 position, vec2 size, uint32_t color, char *name);

void clstSpriteFlipX(CLSTsprite *sprite);
void clstSpriteFlipY(CLSTsprite *sprite);

void clstSpriteDestroy(CLSTsprite *sprite);

CLSTlabel *clstLabel(vec2 position, char *text, CLSTfont *font, char *name);
CLSTlabel *clstLabelCol(vec2 position, char *text, CLSTfont *font, uint32_t color, char *name);
void clstLabelSetText(CLSTlabel *label, char *text);
void clstLabelDestroy(CLSTlabel *label);

/*
 * Creates an animation.
 *
 * returns a CLSTanimation pointer,
 * which must be destroyed by clstAnimationDestroy().
 */
CLSTanimation *clstAnimation(CLSTsprite **frames, uint32_t frames_count, double frame_time, char *name);

/*
 * Destroys the animation, and frees its memory.
 */
void clstAnimationDestroy(CLSTanimation *animation);

/*
 * Creates a group.
 *
 * returns a CLSTgroup pointer,
 * which must be destroyed by clstGroupDestroy().
 */
CLSTgroup *clstGroup(vec2 position, char *name);

/*
 * Creates a group.
 *
 * returns a CLSTgroup pointer,
 * which must be destroyed by clstGroupDestroy().
 */
CLSTgroup *clstGroupMat4(mat4 translation, char *name);

/*
 * Destroys the group, and frees its memory.
 */
void clstGroupDestroy(CLSTgroup *group);

/*
 * Changes a group's position.
 */
void clstGroupPosition(CLSTgroup *group, vec2 position);

/*
 * Adds a sprite to the group.
 */
void clstGroupAddRenderable(CLSTgroup *group, void *renderable);

/*
 * Creates a button.
 */
CLSTbutton *clstButton(CLSTsprite *sprite, char *name);

/*
 * Creates a button.
 */
CLSTbutton *clstButtonCustom(uint32_t status, uint32_t disabled, CLSTsprite *sprite, mat4 translation, char *name);

/*
 * Destroys the button, and frees its memory.
 */
void clstButtonDestroy(CLSTbutton *button);

/*
 * Returns the renderable type.
 */
CLSTrenderabletype clstRenderableType(CLSTrenderable *renderable);

/*
 * Destroys the renderable, and frees its memory.
 */
void clstRenderableDestroy(CLSTrenderable *renderable);

/*
 * Checks for a button event.
 */
void clstButtonEv(CLSTbutton *button, float projection_x, float projection_y);

#endif /* __CELESTE_SPRITE_H__ */
