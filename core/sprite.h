#ifndef __CELESTE_SPRITE_H__
#define __CELESTE_SPRITE_H__

#include <cglm/vec2.h>
#include <cglm/vec3.h>
#include <cglm/mat4.h>
#include "gapi.h"
#include "list.h"

typedef void (*CLSTdrawfunc)(void *renderer, void *renderable);

typedef struct {
    CLSTdrawfunc draw;
    vec2         position;
    vec2         size;
    vec2         uv[4];
    CLSTtexture *texture;
    uint32_t     color;
} CLSTsprite;

typedef struct {
    CLSTdrawfunc draw;
    vec2         position;
    vec2         uv[4];
    CLSTfont    *font;
    char        *text;
    uint32_t     color;
} CLSTlabel;

typedef struct {
    CLSTdrawfunc draw;
    uint32_t current_frame;
    CLSTlist *frames;
    double   frame_time;
    double   last_animation_time;
} CLSTanimation;

typedef struct {
    CLSTdrawfunc draw;
    CLSTlist    *sprites;
    mat4         translation;
} CLSTgroup;

typedef enum {
    CELESTE_BUTTON_STATUS_NONE     = 0,
    CELESTE_BUTTON_STATUS_FOCUSED  = 1,
    CELESTE_BUTTON_STATUS_CLICKED  = 2,
    CELESTE_BUTTON_STATUS_RELEASED = 3,
} CLSTbuttonstatus;

typedef struct {
    CLSTdrawfunc draw;
    uint32_t     status;
    uint32_t     disabled;
    CLSTsprite  *sprite;
    mat4         translation;
} CLSTbutton;

/*
 * Creates a textured sprite.
 */
CLSTsprite *clstSprite(vec2 position, vec2 size, CLSTtexture *texture);

/*
 * Creates a textured sprite from an atlas.
 */
CLSTsprite *clstSpriteTexAtlas(vec2 position, vec2 size, CLSTtexture *texture_atlas, vec2 offset, vec2 texsize);

/*
 * Creates a colored sprite.
 */
CLSTsprite *clstSpriteCol(vec2 position, vec2 size, uint32_t color);

/*
 * Destroys the sprite, and frees its memory.
 */
void clstSpriteDestroy(CLSTsprite *sprite);

/*
 * Creates a label from the text with the font.
 */
CLSTlabel *clstLabel(vec2 position, char *text, CLSTfont *font);

/*
 * Creates a colored label from the text with the font.
 */
CLSTlabel *clstLabelCol(vec2 position, char *text, CLSTfont *font, uint32_t color);

/*
 * Destroys the label, and frees its memory.
 */
void clstLabelDestroy(CLSTlabel *label);

/*
 * Creates an animation.
 *
 * returns a CLSTanimation pointer,
 * which must be destroyed by clstAnimationDestroy().
 */
CLSTanimation *clstAnimation(CLSTsprite **frames, uint32_t frames_count, double frame_time);

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
CLSTgroup *clstGroup(vec2 position);

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
CLSTbutton *clstButton(CLSTsprite *sprite);

/*
 * Destroys the button, and frees its memory.
 */
void clstButtonDestroy(CLSTbutton *button);

/*
 * Destroys the renderable, and frees its memory.
 */
void clstRenderableDestroy(void *renderable);

/*
 * Checks for a button event.
 */
void clstButtonEv(CLSTbutton *button, float projection_x, float projection_y);

#endif /* __CELESTE_SPRITE_H__ */
