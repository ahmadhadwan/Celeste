#ifndef __CELESTE_SPRITE_H__
#define __CELESTE_SPRITE_H__

#include <cglm/vec2.h>
#include <cglm/vec3.h>
#include <cglm/mat4.h>
#include "gapi.h"

enum { SPRITE, LABEL, ANIMATION, GROUP, BUTTON };

typedef struct {
    int          type;
    vec3         position;
    vec2         size;
    vec2         uv[4];
    unsigned int tid;
    unsigned int color;
} celeste_sprite_t;

typedef struct {
    int             type;
    vec3            position;
    vec2            uv[4];
    celeste_font_t *font;
    char           *text;
    unsigned int    color;
} celeste_label_t;

typedef struct {
    int                type;
    unsigned int       frames_count;
    unsigned int       current_frame;
    celeste_sprite_t **frames;
    double             frame_time;
    double             last_animation_time;
} celeste_animation_t;

typedef struct {
    int                type;
    unsigned int       sprites_count;
    celeste_sprite_t **sprites;
    mat4               translation;
} celeste_group_t;

#define CELESTE_STATUS_NONE     0x00
#define CELESTE_STATUS_FOCUSED  0x01
#define CELESTE_STATUS_CLICKED  0x02

typedef struct {
    int                type;
    int                status;
    celeste_group_t   *group;
} celeste_button_t;

void celeste_sprite_create_tex(vec3 position, vec2 size, celeste_texture_t *texture, celeste_sprite_t *sprite);
void celeste_sprite_create_tex_from_atlas(vec3 position, vec2 size, celeste_texture_t *texture_atlas, vec2 offset, vec2 texsize, celeste_sprite_t *sprite);
void celeste_sprite_create_col(vec3 position, vec2 size, unsigned int color, celeste_sprite_t *sprite);

void celeste_label_create(vec3 position, char *text, celeste_font_t *font, celeste_label_t *label);
void celeste_label_create_col(vec3 position, char *text, celeste_font_t *font, unsigned int color, celeste_label_t *label);

celeste_animation_t *celeste_animation_create(celeste_sprite_t **frames, int frames_count, double frame_time);
void celeste_animation_destroy(celeste_animation_t *animation);

celeste_group_t *celeste_group_create(vec3 position);
void celeste_group_destroy(celeste_group_t *group);
void celeste_group_add_sprite(celeste_group_t *group, void *sprite);

void celeste_button_create(celeste_group_t *group, celeste_button_t *button);

#endif /* __CELESTE_SPRITE_H__ */
