#ifndef __CELESTE_CAMERA_H__
#define __CELESTE_CAMERA_H__

#include <cglm/vec3.h>

typedef struct {
    vec3 position;
} celeste_camera_t;

void celeste_camera_create(vec3 position, celeste_camera_t *camera);

#endif /* __CELESTE_CAMERA_H__ */
