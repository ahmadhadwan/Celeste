#include "camera.h"

CLSTcameraortho *clstCameraOrtho(vec2 position)
{
    CLSTcameraortho *camera = malloc(sizeof(CLSTcameraortho));
    glm_vec2_copy(position, camera->position);
    return camera;
}
