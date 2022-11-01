#ifndef __CELESTE_PHYSICS_H__
#define __CELESTE_PHYSICS_H__

#include <cglm/vec2.h>
#include <cglm/vec3.h>

typedef struct {
    unsigned int  layer;    /* Makes grouping bodies easier in layers */
    unsigned char gravity;  /* boolean, 0 means that it's not affected by gravity */
    unsigned char collider; /* boolean, 0 means that it isn't a collider */
    unsigned char dynamic;  /* boolean, 0 means that it doesn't collide on its own */
    vec2         *position; /* should point to something like a sprite's position */
    vec2         *size;     /* should point to something like a sprite's size */
    vec2          velocity;
} CLSTbody;

/*
 * Creates a physical body in a specified layer,
 * pointing to a specific position and size vectors.
 * 
 * It returns a CLSTbody pointer,
 * which must be destroyed by clstBodyDestroy().
 */
CLSTbody *clstBody(unsigned int layer, unsigned char gravity, unsigned char collider, unsigned char dynamic, vec2 *position, vec2 *size);

/*
 * Destroys a CLSTbody.
 */
void clstBodyDestroy(CLSTbody *body);

/*
 * Checks if a point is colliding with a rectangle.
 */
int clstCollisionRectanglePoint(vec2 *recpos, vec2 *recsize, vec2 point);

/*
 * Checks if two rectangles are colliding.
 */
int clstCollisionRectangles(vec2 *rec1pos, vec2 *rec1size, vec2 *rec2pos, vec2 *rec2size);

#endif /* __CELESTE_PHYSICS_H__ */
