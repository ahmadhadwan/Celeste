#ifndef __CELESTE_PHYSICS_H__
#define __CELESTE_PHYSICS_H__

#include <cglm/vec2.h>

typedef enum {
    CELESTE_BODY_GRAVITY  = 0x01,
    CELESTE_BODY_COLLIDER = 0x02,
    CELESTE_BODY_DYNAMIC  = 0x04,
} CLSTbodyflags;

typedef struct {
    uint32_t layer;
    uint32_t flags;
    vec2     position;
    vec2     size;
    vec2     velocity;
    char    *name;
} CLSTbody;

/*
 * Creates a physical body in a specified layer,
 * pointing to a specific position and size vectors.
 * 
 * It returns a CLSTbody pointer,
 * which must be destroyed by clstBodyDestroy().
 */
CLSTbody *clstBody(uint32_t layer, uint32_t flags, vec2 position, vec2 size, char *name);

/*
 * Destroys a CLSTbody.
 */
void clstBodyDestroy(CLSTbody *body);

/*
 * Checks if a point is colliding with a rectangle.
 */
uint32_t clstCollisionRectanglePoint(vec2 recpos, vec2 recsize, vec2 point);

/*
 * Checks if two rectangles are colliding.
 */
uint32_t clstCollisionRectangles(vec2 rec1pos, vec2 rec1size, vec2 rec2pos, vec2 rec2size);

#endif /* __CELESTE_PHYSICS_H__ */
