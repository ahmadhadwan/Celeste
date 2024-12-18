#include "celeste.h"
#include "physics.h"
#include <string.h>

CLSTbody *clstBody(uint32_t layer, uint8_t gravity, uint8_t collider,
                   uint8_t dynamic, vec2 *position, vec2 *size)
{
    CLSTbody *body;
    CLST *clst;

    body = malloc(sizeof(CLSTbody));
    body->layer = layer;
    body->gravity = gravity;
    body->collider = collider;
    body->dynamic = dynamic;
    body->position = position;
    body->size = size;
    body->velocity[0] = 0.0f;
    body->velocity[1] = 0.0f;

    clst = clstInstance();
    clstListAdd(clst->scene->bodies, body);
    return body;
}

void clstBodyDestroy(CLSTbody *body)
{
    free(body);
}

uint32_t clstCollisionRectanglePoint(vec2 *recpos, vec2 *recsize, vec2 point)
{
    const float posx   = (*recpos)[0];
    const float posy   = (*recpos)[1];

    const float sizex  = (*recsize)[0];
    const float sizey  = (*recsize)[1];

    const float pointx = point[0];
    const float pointy = point[1];

    return (
            (pointx > posx)
         && (pointx < (posx + sizex))
         && (pointy > posy)
         && (pointy < (posy + sizey))
        );
}

static uint32_t internal_celeste_collision_rectangles(vec2 *rec1pos, vec2 *rec1size, vec2 *rec2pos, vec2 *rec2size)
{
    const float pos1x  = (*rec1pos)[0];
    const float pos1y  = (*rec1pos)[1];
    const float size1x = (*rec1size)[0];
    const float size1y = (*rec1size)[1];

    const float pos2x  = (*rec2pos)[0];
    const float pos2y  = (*rec2pos)[1];
    const float size2x = (*rec2size)[0];
    const float size2y = (*rec2size)[1];

    return (
            (pos1x < pos2x + size2x)
         && (pos1x + size1x > pos2x)
         && (pos1y < pos2y + size2y)
         && (size1y + pos1y > pos2y)
        );
}

uint32_t clstCollisionRectangles(vec2 *rec1pos, vec2 *rec1size, vec2 *rec2pos, vec2 *rec2size)
{
    const float pos1x  = (*rec1pos)[0];
    const float pos1y  = (*rec1pos)[1];
    const float size1x = (*rec1size)[0];
    const float size1y = (*rec1size)[1];

    const float pos2x  = (*rec2pos)[0];
    const float pos2y  = (*rec2pos)[1];
    const float size2x = (*rec2size)[0];
    const float size2y = (*rec2size)[1];

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
        clst->last_physics_update = clstTime();
        return;
    }

    count = clst->scene->bodies->count;
    delta = clstTime() - clst->last_physics_update;
    for (int i = 0; i < count; i++)
    {
        body = ((CLSTbody **)clst->scene->bodies->items)[i];
        if (!body->dynamic)
            continue;

        if (body->velocity[0] > 0.0f)
        {
            (*body->position)[0] += body->velocity[0] * delta;
            body->velocity[0] -= body->velocity[0] * delta;
            for (int j = 0; j < count; j++)
            {
                CLSTbody *body2;

                body2 = ((CLSTbody **)clst->scene->bodies->items)[j];
                if (i == j || !body2->collider)
                    continue;

                if (internal_celeste_collision_rectangles(body->position, body->size, body2->position, body2->size)) {
                    (*body->position)[0] = (*body2->position)[0] - (*body->size)[0];
                    body->velocity[0] = 0.0f;
                }
            }
        }
        else if (body->velocity[0] < 0.0f)
        {
            (*body->position)[0] += body->velocity[0] * delta;
            body->velocity[0] -= body->velocity[0] * delta;
            for (int j = 0; j < count; j++)
            {
                CLSTbody *body2;

                body2 = ((CLSTbody **)clst->scene->bodies->items)[j];
                if (i == j || !body2->collider)
                    continue;

                if (internal_celeste_collision_rectangles(body->position, body->size, body2->position, body2->size)) {
                    (*body->position)[0] = (*body2->position)[0] + (*body2->size)[0];
                    body->velocity[0] = 0.0f;
                }
            }
        }

        if (body->velocity[1] > 0.0f)
        {
            (*body->position)[1] += body->velocity[1] * delta;
            body->velocity[1] -= body->velocity[1] * delta;
            for (int j = 0; j < count; j++)
            {
                CLSTbody *body2;

                body2 = ((CLSTbody **)clst->scene->bodies->items)[j];
                if (i == j || !body2->collider)
                    continue;

                if (internal_celeste_collision_rectangles(body->position, body->size, body2->position, body2->size)) {
                    (*body->position)[1] = (*body2->position)[1] - (*body->size)[1];
                    body->velocity[1] = 0.0f;
                }
            }
        }
        else if (body->velocity[1] < 0.0f)
        {
            (*body->position)[1] += body->velocity[1] * delta;
            if (body->gravity && clst->world_gravity > 0.0f)
                body->velocity[1] -= clst->world_gravity * delta;
            else
                body->velocity[1] -= body->velocity[1] * delta;

            for (int j = 0; j < count; j++)
            {
                CLSTbody *body2;

                body2 = ((CLSTbody **)clst->scene->bodies->items)[j];
                if (i == j || !body2->collider)
                    continue;

                if (internal_celeste_collision_rectangles(body->position, body->size, body2->position, body2->size)) {
                    (*body->position)[1] = (*body2->position)[1] + (*body2->size)[1];
                    body->velocity[1] = 0.0f;
                }
            }
        }

        if (body->gravity && clst->world_gravity > 0.0f)
            body->velocity[1] -= clst->world_gravity * delta;
    }
    clst->last_physics_update = clstTime();
}
