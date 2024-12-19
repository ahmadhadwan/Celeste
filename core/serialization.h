#ifndef __CELESTE_SERIALIZATION_H__
#define __CELESTE_SERIALIZATION_H__

#include "sprite.h"
#include "physics.h"

void *clstSerializeSprite(CLSTsprite *sprite, uint32_t *out_data_size);
CLSTsprite *clstDeserializeSprite(void *data, uint32_t *data_size);

void *clstSerializeLabel(CLSTlabel *label, uint32_t *out_data_size);
CLSTlabel *clstDeserializeLabel(void *data, uint32_t *data_size);

void *clstSerializeAnimation(CLSTanimation *animation, uint32_t *out_data_size);
CLSTanimation *clstDeserializeAnimation(void *data, uint32_t *data_size);

void *clstSerializeButton(CLSTbutton *button, uint32_t *out_data_size);
CLSTbutton *clstDeserializeButton(void *data, uint32_t *data_size);

void *clstSerializeGroup(CLSTgroup *group, uint32_t *out_data_size);
CLSTgroup *clstDeserializeGroup(void *data, uint32_t *data_size);

void *clstSerializeBody(CLSTbody *body, uint32_t *out_data_size);
CLSTbody *clstDeserializeBody(void *data);

#endif /* __CELESTE_SERIALIZATION_H__ */