#include "celeste.h"
#include "physics.h"
#include <string.h>

CLSTbody *clstBody(unsigned int layer, unsigned char gravity, unsigned char collider,
                   unsigned char dynamic, vec2 *position, vec2 *size)
{
    CLSTbody *body;
    CLST *celeste;

    body = malloc(sizeof(CLSTbody));
    body->layer = layer;
    body->gravity = gravity;
    body->collider = collider;
    body->dynamic = dynamic;
    body->position = position;
    body->size = size;
    body->velocity[0] = 0.0f;
    body->velocity[1] = 0.0f;

    celeste = clstInstance();
    celeste->bodies = realloc(celeste->bodies, (celeste->bodies_count + 1) * sizeof(CLSTbody *));
    celeste->bodies[celeste->bodies_count] = body;
    celeste->bodies_count++;
    return body;
}

void clstBodyDestroy(CLSTbody *body)
{
    CLST *celeste;

    celeste = clstInstance();
    for (int i = 0; i < celeste->bodies_count; i++) {
        if (celeste->bodies[i] == body)
        {
            memmove(celeste->bodies + i, celeste->bodies + i + 1, (celeste->bodies_count - i - 1) * sizeof(CLSTbody *));
            celeste->bodies = realloc(celeste->bodies, (celeste->bodies_count - 1) * sizeof(CLSTbody *));
            celeste->bodies_count--;
            break;
        }
    }
    free(body);
}

int clstCollisionRectanglePoint(vec2 *recpos, vec2 *recsize, vec2 point)
{
    const float posx = (*recpos)[0];
    const float posy = (*recpos)[1];

    const float sizex = (*recsize)[0];
    const float sizey = (*recsize)[1];

    const float pointx = point[0];
    const float pointy = point[1];

    return (
            (pointx > posx)
         && (pointx < (posx + sizex))
         && (pointy > posy)
         && (pointy < (posy + sizey))
        );
}

static int internal_celeste_collision_rectangles(vec2 *rec1pos, vec2 *rec1size, vec2 *rec2pos, vec2 *rec2size)
{
    const float pos1x = (*rec1pos)[0];
    const float pos1y = (*rec1pos)[1];
    const float size1x = (*rec1size)[0];
    const float size1y = (*rec1size)[1];

    const float pos2x = (*rec2pos)[0];
    const float pos2y = (*rec2pos)[1];
    const float size2x = (*rec2size)[0];
    const float size2y = (*rec2size)[1];

    return (
            (pos1x < pos2x + size2x)
         && (pos1x + size1x > pos2x)
         && (pos1y < pos2y + size2y)
         && (size1y + pos1y > pos2y)
        );
}

int clstCollisionRectangles(vec2 *rec1pos, vec2 *rec1size, vec2 *rec2pos, vec2 *rec2size)
{
    const float pos1x = (*rec1pos)[0];
    const float pos1y = (*rec1pos)[1];
    const float size1x = (*rec1size)[0];
    const float size1y = (*rec1size)[1];

    const float pos2x = (*rec2pos)[0];
    const float pos2y = (*rec2pos)[1];
    const float size2x = (*rec2size)[0];
    const float size2y = (*rec2size)[1];

    return (
            (pos1x <= pos2x + size2x)
         && (pos1x + size1x >= pos2x)
         && (pos1y <= pos2y + size2y)
         && (size1y + pos1y >= pos2y)
        );
}

void clstPhysicsUpdate(CLST *celeste)
{
    CLSTbody *body;
    float delta;

    delta = clstTime() - celeste->last_physics_update;
    for (int i = 0; i < celeste->bodies_count; i++)
    {
        body = celeste->bodies[i];
        if (!body->dynamic)
            continue;

        if (body->velocity[0] > 0.0f)
        {
            (*body->position)[0] += body->velocity[0] * delta;
            body->velocity[0] -= body->velocity[0] * delta;
            for (int j = 0; j < celeste->bodies_count; j++)
            {
                if (i == j || !celeste->bodies[j]->collider)
                    continue;

                if (internal_celeste_collision_rectangles(body->position, body->size, celeste->bodies[j]->position, celeste->bodies[j]->size)) {
                    (*body->position)[0] = (*celeste->bodies[j]->position)[0] - (*body->size)[0];
                    body->velocity[0] = 0.0f;
                }
            }
        }
        else if (body->velocity[0] < 0.0f)
        {
            (*body->position)[0] += body->velocity[0] * delta;
            body->velocity[0] -= body->velocity[0] * delta;
            for (int j = 0; j < celeste->bodies_count; j++)
            {
                if (i == j || !celeste->bodies[j]->collider)
                    continue;

                if (internal_celeste_collision_rectangles(body->position, body->size, celeste->bodies[j]->position, celeste->bodies[j]->size)) {
                    (*body->position)[0] = (*celeste->bodies[j]->position)[0] + (*celeste->bodies[j]->size)[0];
                    body->velocity[0] = 0.0f;
                }
            }
        }

        if (body->velocity[1] > 0.0f)
        {
            (*body->position)[1] += body->velocity[1] * delta;
            body->velocity[1] -= body->velocity[1] * delta;
            for (int j = 0; j < celeste->bodies_count; j++)
            {
                if (i == j || !celeste->bodies[j]->collider)
                    continue;

                if (internal_celeste_collision_rectangles(body->position, body->size, celeste->bodies[j]->position, celeste->bodies[j]->size)) {
                    (*body->position)[1] = (*celeste->bodies[j]->position)[1] - (*body->size)[1];
                    body->velocity[1] = 0.0f;
                }
            }
        }
        else if (body->velocity[1] < 0.0f)
        {
            (*body->position)[1] += body->velocity[1] * delta;
            if (body->gravity && celeste->world_gravity > 0.0f)
                body->velocity[1] -= celeste->world_gravity * delta;
            else
                body->velocity[1] -= body->velocity[1] * delta;

            for (int j = 0; j < celeste->bodies_count; j++)
            {
                if (i == j || !celeste->bodies[j]->collider)
                    continue;

                if (internal_celeste_collision_rectangles(body->position, body->size, celeste->bodies[j]->position, celeste->bodies[j]->size)) {
                    (*body->position)[1] = (*celeste->bodies[j]->position)[1] + (*celeste->bodies[j]->size)[1];
                    body->velocity[1] = 0.0f;
                }
            }
        }

        if (body->gravity && celeste->world_gravity > 0.0f)
            body->velocity[1] -= celeste->world_gravity * delta;
    }
    celeste->last_physics_update = clstTime();
}
