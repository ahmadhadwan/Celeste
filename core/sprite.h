#ifndef __CELESTE_SPRITE_H__
#define __CELESTE_SPRITE_H__

#include <cglm/vec2.h>
#include <cglm/vec3.h>
#include <cglm/mat4.h>
#include "gapi.h"

#define BUTTON_STATUS_NONE     0
#define BUTTON_STATUS_FOCUSED  1
#define BUTTON_STATUS_CLICKED  2
#define BUTTON_STATUS_RELEASED 3

typedef void (*draw_func)(void *renderer, void *renderable);

typedef struct {
    draw_func    draw;
    vec2         position;
    vec2         size;
    vec2         uv[4];
    CLSTtexture *texture;
    unsigned int color;
} CLSTsprite;

typedef struct {
    draw_func    draw;
    vec2         position;
    vec2         uv[4];
    CLSTfont    *font;
    char        *text;
    unsigned int color;
} CLSTlabel;

typedef struct {
    draw_func    draw;
    unsigned int frames_count;
    unsigned int current_frame;
    CLSTsprite **frames;
    double       frame_time;
    double       last_animation_time;
} CLSTanimation;

typedef struct {
    draw_func    draw;
    unsigned int sprites_count;
    CLSTsprite **sprites;
    mat4         translation;
} CLSTgroup;

typedef struct {
    draw_func   draw;
    int         status;
    int         disabled;
    CLSTsprite *sprite;
    mat4        translation;
} CLSTbutton;

/*
 * Creates a textured sprite.
 */
void clstSprite(vec2 position, vec2 size, CLSTtexture *texture, CLSTsprite *sprite);

/*
 * Creates a textured sprite from an atlas.
 */
void clstSpriteTexAtlas(vec2 position, vec2 size, CLSTtexture *texture_atlas, vec2 offset, vec2 texsize, CLSTsprite *sprite);

/*
 * Creates a colored sprite.
 */
void clstSpriteCol(vec2 position, vec2 size, unsigned int color, CLSTsprite *sprite);

/*
 * Creates a label from the text with the font.
 */
void clstLabel(vec2 position, char *text, CLSTfont *font, CLSTlabel *label);

/*
 * Creates a colored label from the text with the font.
 */
void clstLabelCol(vec2 position, char *text, CLSTfont *font, unsigned int color, CLSTlabel *label);

/*
 * Creates an animation.
 *
 * returns a CLSTanimation pointer,
 * which must be destroyed by clstAnimationDestroy().
 */
CLSTanimation *clstAnimation(CLSTsprite **frames, int frames_count, double frame_time);

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
void clstGroupAddSprite(CLSTgroup *group, void *sprite);

/*
 * Creates a button.
 */
void clstButton(CLSTsprite *sprite, CLSTbutton *button);

/*
 * Checks for a button event.
 */
void clstButtonEv(CLSTbutton *button, float projection_x, float projection_y);

#endif /* __CELESTE_SPRITE_H__ */
