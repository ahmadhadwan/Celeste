#include "camera.h"

void celeste_camera_create(vec3 position, celeste_camera_t *camera)
{
    glm_vec3_copy(position, camera->position);
}
