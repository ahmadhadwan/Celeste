#include "camera.h"

void clstCameraOrtho(vec2 position, CLSTcameraortho *camera)
{
    glm_vec2_copy(position, camera->position);
}
