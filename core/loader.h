#ifndef __CELESTE_LOADER_H__
#define __CELESTE_LOADER_H__

#include <stdint.h>
#include "celeste.h"

#define CELESTE_FILE_MAGIC_NUMBER  0x04200615

typedef struct {
    char    *name;
    void    *data;
    uint32_t data_size;
    uint8_t  type;
} CLSTloadable;

typedef enum {
    CELESTE_DELETED      = 0,
    CELESTE_TEXTURE_FILE = 1,
    CELESTE_TEXTURE_BIN  = 2,
    CELESTE_FONT_FILE    = 3,
    CELESTE_FONT_BIN     = 4,
    CELESTE_AUDIO_FILE   = 5,
    CELESTE_AUDIO_BIN    = 6,
    CELESTE_SPRITE       = 7,
    CELESTE_LAYER        = 8,
    CELESTE_SPRITE_ATLAS = 9,
    CELESTE_ANIMATION    = 10,
    CELESTE_LABEL        = 11,
} CLSTloadabletype;

typedef struct {
    char     *filepath;
    CLSTlist *loadables;
} CLSTloader;

CLSTloader *clstLoader(char *filepath);
void clstLoaderDestroy(CLSTloader *loader);

void clstLoaderSaveData(CLSTloader *loader);
void clstLoaderLoadData(CLSTloader *loader);

CLSTloadable *clstLoadable(char *name, void *data, int data_size, uint8_t type);
void clstLoadableDestroy(CLSTloadable *loadable);

CLSTtexture *clstTextureSave(char *filepath, char *name);
CLSTtexture *clstTextureMemSave(uint8_t *buffer, uint32_t bufsize, char *name);

CLSTfont *clstFontSave(char *filepath, float size, char *name);
CLSTfont *clstFontMemSave(uint8_t *buffer, uint32_t bufsize, float size, char *name);

CLSTaudio *clstAudioSave(const char *filepath, char *name);
CLSTaudio *clstAudioMemSave(unsigned char *buffer, unsigned int bufsize, char *name);

CLSTsprite *clstSpriteSave(vec2 position, vec2 size, CLSTtexture *texture, char *name);
CLSTsprite *clstSpriteTexAtlasSave(vec2 position, vec2 size, CLSTtexture *texture_atlas, vec2 offset, vec2 texsize, char *name);
CLSTanimation *clstAnimationSave(CLSTsprite **frames, uint32_t frames_count, double frame_time, char *name);
CLSTlabel *clstLabelSave(vec2 position, char *text, CLSTfont *font, char *name);
CLSTlayer *clstLayerCameraSave(CLSTcamera *camera, float right, float top, char *name);

#endif /* __CELESTE_LOADER_H__ */
