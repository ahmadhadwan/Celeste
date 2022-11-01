#ifndef __CELESTE_CAMERA_H__
#define __CELESTE_CAMERA_H__

#include <cglm/vec2.h>

typedef struct {
    vec2 position;
} CLSTcameraortho;

typedef CLSTcameraortho CLSTcamera;

/*
 * Creates a camera with the position arg.
 */
void clstCameraOrtho(vec2 position, CLSTcameraortho *camera);

#endif /* __CELESTE_CAMERA_H__ */
