#ifndef __CELESTE_H__
#define __CELESTE_H__

#include "scene.h"

#ifndef __CELESTE_INTERNAL_H__
    typedef struct CLST CLST;
#endif /* __CELESTE_INTERNAL_H__ */

CLST *clstInit();
void clstTerminate();

CLST *clstInstance();

CLSTscene *clstGetScene();
void clstSetScene(CLSTscene *scene);

float clstGetWorldGravity();
void clstSetWorldGravity(float gravity);

void clstResetPhysics();

double clstGetTime();

void clstUpdate(CLST *clst);
void clstWaitEv(CLST *clst);

#endif /* __CELESTE_H__ */
