#include "internal/celeste.h"
#include "physics.h"
#include <stdlib.h>

CLSTbody *clstBody(uint32_t layer, uint32_t flags, vec2 position, vec2 size, char *name)
{
    CLSTbody *body;

    body = malloc(sizeof(CLSTbody));
    body->layer = layer;
    body->flags = flags;
    glm_vec2_copy(position, body->position);
    glm_vec2_copy(size, body->size);
    glm_vec2_copy((vec2){ 0.0f, 0.0f }, body->velocity);
    body->name = name;
    return body;
}

void clstBodyDestroy(CLSTbody *body)
{
    free(body);
}

uint32_t clstCollisionRectanglePoint(vec2 recpos, vec2 recsize, vec2 point)
{
    const float posx   = recpos[0];
    const float posy   = recpos[1];

    const float sizex  = recsize[0];
    const float sizey  = recsize[1];

    const float pointx = point[0];
    const float pointy = point[1];

    return (
            (pointx > posx)
         && (pointx < (posx + sizex))
         && (pointy > posy)
         && (pointy < (posy + sizey))
        );
}

static uint32_t internal_celeste_collision_rectangles(vec2 rec1pos, vec2 rec1size, vec2 rec2pos, vec2 rec2size)
{
    const float pos1x  = rec1pos[0];
    const float pos1y  = rec1pos[1];
    const float size1x = rec1size[0];
    const float size1y = rec1size[1];

    const float pos2x  = rec2pos[0];
    const float pos2y  = rec2pos[1];
    const float size2x = rec2size[0];
    const float size2y = rec2size[1];

    return (
            (pos1x < pos2x + size2x)
         && (pos1x + size1x > pos2x)
         && (pos1y < pos2y + size2y)
         && (size1y + pos1y > pos2y)
        );
}

uint32_t clstCollisionRectangles(vec2 rec1pos, vec2 rec1size, vec2 rec2pos, vec2 rec2size)
{
    const float pos1x  = rec1pos[0];
    const float pos1y  = rec1pos[1];
    const float size1x = rec1size[0];
    const float size1y = rec1size[1];

    const float pos2x  = rec2pos[0];
    const float pos2y  = rec2pos[1];
    const float size2x = rec2size[0];
    const float size2y = rec2size[1];

    return (
            (pos1x <= pos2x + size2x)
         && (pos1x + size1x >= pos2x)
         && (pos1y <= pos2y + size2y)
         && (size1y + pos1y >= pos2y)
        );
}

void clstPhysicsUpdate(CLST *clst)
{
    CLSTbody *body;
    uint32_t count;
    float delta;

    if (!clst->window.focused) {
        clst->last_physics_update = clstGetTime();
        return;
    }

    count = clst->scene->bodies->count;
    delta = clstGetTime() - clst->last_physics_update;
    for (int i = 0; i < count; i++)
    {
        body = ((CLSTbody **)clst->scene->bodies->items)[i];
        if (!(body->flags & CELESTE_BODY_DYNAMIC))
            continue;

        if (body->velocity[0] > 0.0f)
        {
            body->position[0] += body->velocity[0] * delta;
            body->velocity[0] -= body->velocity[0] * delta;
            for (int j = 0; j < count; j++)
            {
                CLSTbody *body2;

                body2 = ((CLSTbody **)clst->scene->bodies->items)[j];
                if (i == j || !(body2->flags & CELESTE_BODY_COLLIDER))
                    continue;

                if (internal_celeste_collision_rectangles(body->position, body->size, body2->position, body2->size)) {
                    body->position[0] = body2->position[0] - body->size[0];
                    body->velocity[0] = 0.0f;
                }
            }
        }
        else if (body->velocity[0] < 0.0f)
        {
            body->position[0] += body->velocity[0] * delta;
            body->velocity[0] -= body->velocity[0] * delta;
            for (int j = 0; j < count; j++)
            {
                CLSTbody *body2;

                body2 = ((CLSTbody **)clst->scene->bodies->items)[j];
                if (i == j || !(body2->flags & CELESTE_BODY_COLLIDER))
                    continue;

                if (internal_celeste_collision_rectangles(body->position, body->size, body2->position, body2->size)) {
                    body->position[0] = body2->position[0] + body2->size[0];
                    body->velocity[0] = 0.0f;
                }
            }
        }

        if (body->velocity[1] > 0.0f)
        {
            body->position[1] += body->velocity[1] * delta;
            body->velocity[1] -= body->velocity[1] * delta;
            for (int j = 0; j < count; j++)
            {
                CLSTbody *body2;

                body2 = ((CLSTbody **)clst->scene->bodies->items)[j];
                if (i == j || !(body2->flags & CELESTE_BODY_COLLIDER))
                    continue;

                if (internal_celeste_collision_rectangles(body->position, body->size, body2->position, body2->size)) {
                    body->position[1] = body2->position[1] - body->size[1];
                    body->velocity[1] = 0.0f;
                }
            }
        }
        else if (body->velocity[1] < 0.0f)
        {
            body->position[1] += body->velocity[1] * delta;
            if ((body->flags & CELESTE_BODY_GRAVITY) && clst->world_gravity > 0.0f)
                body->velocity[1] -= clst->world_gravity * delta;
            else
                body->velocity[1] -= body->velocity[1] * delta;

            for (int j = 0; j < count; j++)
            {
                CLSTbody *body2;

                body2 = ((CLSTbody **)clst->scene->bodies->items)[j];
                if (i == j || !(body2->flags & CELESTE_BODY_COLLIDER))
                    continue;

                if (internal_celeste_collision_rectangles(body->position, body->size, body2->position, body2->size)) {
                    body->position[1] = body2->position[1] + body2->size[1];
                    body->velocity[1] = 0.0f;
                }
            }
        }

        if ((body->flags & CELESTE_BODY_GRAVITY) && clst->world_gravity > 0.0f)
            body->velocity[1] -= clst->world_gravity * delta;
    }
    clst->last_physics_update = clstGetTime();
}
