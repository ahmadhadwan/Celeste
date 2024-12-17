#include <celeste/celeste.h>
#include "../core/loader.h"
#include <cglm/cglm.h>
#include <stdio.h>
#include <string.h>
#ifdef __linux__
    #include <sys/resource.h>
#endif /* __linux__ */

#ifdef CELESTE_PACK_RESOURCES
    #include "../res/fonts/ThaleahFat.c"
    #include "../res/textures/atlas_48x.c"
    #include "../res/textures/space8_4-frames.c"
    #include "../res/audio/space.c"
#endif /* CELESTE_PACK_RESOURCES */

#include "../res/shaders/fb.c"
#include "../res/icons/celeste_48x48.h"

static CLSTanimation *space_animation(CLSTsprite *sprites, CLSTtexture *texture_space)
{
    vec2 pos = { -16.0f, -9.0f };
    vec2 size = { 32.0f, 18.0f };

    clstSpriteTexAtlas(pos, size, texture_space, (vec2){ 0, 0 }, (vec2){ clstTextureWidth(texture_space) / 4.0f, clstTextureHeight(texture_space) }, sprites);
    clstSpriteTexAtlas(pos, size, texture_space, (vec2){ 1, 0 }, (vec2){ clstTextureWidth(texture_space) / 4.0f, clstTextureHeight(texture_space) }, sprites + 1);
    clstSpriteTexAtlas(pos, size, texture_space, (vec2){ 2, 0 }, (vec2){ clstTextureWidth(texture_space) / 4.0f, clstTextureHeight(texture_space) }, sprites + 2);
    clstSpriteTexAtlas(pos, size, texture_space, (vec2){ 3, 0 }, (vec2){ clstTextureWidth(texture_space) / 4.0f, clstTextureHeight(texture_space) }, sprites + 3);

    return clstAnimation((CLSTsprite*[]){sprites, sprites + 1, sprites + 2, sprites + 3}, 4, 0.25f);
}

static CLSTanimation *carpet_animation(CLSTsprite *sprites, CLSTtexture *texture_atlas)
{
    vec2 pos = { -8.0f, -6.0f };
    vec2 size = { 16.0f, 16.0f };

    clstSpriteTexAtlas(pos, size, texture_atlas, (vec2){ 16, 10 }, (vec2){ 48.0f, 48.0f }, sprites);
    clstSpriteTexAtlas(pos, size, texture_atlas, (vec2){ 16, 10 + 3 }, (vec2){ 48.0f, 48.0f }, sprites + 1);

    return clstAnimation((CLSTsprite*[]){sprites, sprites + 1}, 2, 0.75f);
}

static void destroy_window(void *winalive)
{
    *((int*)winalive) = 0;
}

static void pointless(void *arg)
{
    CELESTE_LOG("THIS CALL WAS POINTLESS (pointless pointer = %p)\n", arg);
}

int main()
{
    CLST *clst;
    CLSTloader *loader;
    CLSTcamera camera;
    CLSTlayer *layer, *layer_debug;
    CLSTtexture *texture_atlas, *texture_space, *texture_celeste;

    CLSTanimation *space_anim, *carpet_anim;
    CLSTsprite space[4];

    CLSTsprite carpet, carpet2, celeste;
    CLSTlabel fps;
    CLSTfont *font;

    CLSTbutton button;
    CLSTgroup *button_group;
    CLSTsprite button_col;
    CLSTlabel button_label;

    CLSTaudio *audio;
    CLSTaudioplayer *audio_player;

    double prevtime, prevtime2;
    unsigned int frames;

    char fps_text[20];
    CLSTscene *scene;
    char pointless_str[1024];
    CLSTlabel pointless_label;

    clst = clstInit();
    if (clst == NULL)
        return 1;

    if (clstWindow("Celeste Engine Sandbox") != CELESTE_OK)
        return 1;

    loader = clstLoader("sandbox.clst");

    clstCameraOrtho((vec2){ 0.0f, 0.0f }, &camera);
    layer = clstLayerCamera(&camera, 16.0f, 9.0f);
    layer_debug = clstLayer(16.0f, 9.0f);

#ifdef CELESTE_PACK_RESOURCES
    texture_atlas = clstTextureMem(atlas_48x_png, sizeof(atlas_48x_png) / sizeof(unsigned char));
    texture_space = clstTextureMem(space8_4_frames_png, sizeof(space8_4_frames_png) / sizeof(unsigned char));
    font = clstFontMem(ThaleahFat_ttf, sizeof(ThaleahFat_ttf) / sizeof(unsigned char), 72.0f);
    audio = clstAudioMem(space_ogg, sizeof(space_ogg) / sizeof(unsigned char));
#else
    texture_atlas = clstTextureSave("res/textures/atlas_48x.png");
    texture_space = clstTextureSave("res/textures/space8_4-frames.png");
    font = clstFont("res/fonts/ThaleahFat.ttf", 72.0f);
    audio = clstAudio("res/audio/space.ogg");
#endif /* CELESTE_PACK_RESOURCES */
    texture_celeste = clstTextureMem(celeste_48x48_png, CELESTE_48X48_PNG_SIZE);

    space_anim = space_animation(space, texture_space);
    clstLayerAddSprite(layer, space_anim);

    carpet_anim = carpet_animation((CLSTsprite[]){carpet, carpet2}, texture_atlas);
    clstLayerAddSprite(layer, carpet_anim);

    clstSprite((vec2){ -4.0f, 1.0f }, (vec2){ 8.0f, 8.0f }, texture_celeste, &celeste);
    clstLayerAddSprite(layer, &celeste);

    button_group = clstGroup((vec2){ -4.0f, -7.5f });
    clstSpriteCol((vec2){ 0.0f, 0.0f }, (vec2){ 8.0f, 4.0f }, 0x3A555555, &button_col);
    clstButton(&button_col, &button);
    clstGroupAddSprite(button_group, &button);
    clstLabelCol((vec2){ 3.0f, 2.0f }, "Quit", font, 0xFFFFFFFF, &button_label);
    clstGroupAddSprite(button_group, &button_label);
    clstLayerAddSprite(layer, button_group);

    memset(fps_text, 0, sizeof(fps_text));
    clstLabel((vec2){ -15.5f, 8.0f }, fps_text, font, &fps);
    clstLayerAddSprite(layer_debug, &fps);

    scene = clstScene();
    clst->scene = scene;
    clstSceneAddLayer(scene, layer);

    audio_player = clstAudioPlayer(audio, 1, 0);
    clstAudioPlayerPlay(audio_player);

#ifdef __linux__
    {
        struct rusage ru;
        getrusage(RUSAGE_SELF, &ru);
        CELESTE_LOG("Max RAM Usage: %ld Kb\n", ru.ru_maxrss);
    }
#endif /* __linux__ */

    clstKeyAddCallback((CLSTkey){CELESTE_KEY_ESCAPE, destroy_window, &(clst->window.alive)});
    clstClickAddCallback((CLSTclick){CELESTE_MOUSE_RIGHT, pointless, NULL});
    clstInputSetListener(pointless_str, 1024);

    memset(pointless_str, 0, sizeof(pointless_str));
    clstLabel((vec2){ -15.5f, -8.5f }, pointless_str, font, &pointless_label);
    clstLayerAddSprite(layer, &pointless_label);

    CLSTsprite quad1, quad2;
    char colliding[10];
    CLSTlabel colliding_label;
    clstSpriteCol((vec2){ -10.0f, -2.0f }, (vec2){ 4.0f, 4.0f }, 0xFFFFFF00, &quad1);
    clstSpriteCol((vec2){   8.0f, -2.0f }, (vec2){ 4.0f, 4.0f }, 0xFF0000FF, &quad2);
    memset(colliding, 0, sizeof(colliding));
    clstLabel((vec2){ -2.0f, 8.0f }, colliding, font, &colliding_label);
    clstLayerAddSprite(layer, &quad1);
    clstLayerAddSprite(layer, &quad2);
    clstLayerAddSprite(layer, &colliding_label);

    CLSTbody *body1 = clstBody(0, 1, 1, 1, (vec2*)&(quad1.position), &(quad1.size));
    CLSTbody *body2 = clstBody(0, 0, 1, 0, (vec2*)&(quad2.position), &(quad2.size));

    vec2 button_pos = { -4.0f, -7.5f };
    CLSTbody *button_body = clstBody(0, 0, 1, 0, &button_pos, &(button_col.size));

    vec2 floor_pos = { -16.0f, -10.0f };
    vec2 floor_size = { 32.0f, 1.0f };
    CLSTbody *floor = clstBody(1, 0, 1, 0, &floor_pos, &floor_size);

    vec2 left_pos = { -17.0f, -9.0f };
    vec2 left_size = { 1.0f, 18.0f };
    CLSTbody *left = clstBody(1, 0, 1, 0, &left_pos, &left_size);

    vec2 right_pos = { 16.0f, -9.0f };
    vec2 right_size = { 1.0f, 18.0f };
    CLSTbody *right = clstBody(1, 0, 1, 0, &right_pos, &right_size);

    CLSTframebuffer *fb;
    CLSTtexture *fbtex;
    fb = clstFrameBuffer();
    fbtex = clstTextureInline(NULL, 1920, 1080, 24);
    clstFrameBufferAttachTexture(fb, fbtex);
    clstFrameBufferAttachRenderBuffer(fb, NULL);

    CLSTsprite fbsprite;
    clstSprite((float[]){-16.0f, -9.0f}, (float[]){32.0f, 18.0f}, fbtex, &fbsprite);
    CLSTshader *fbshader = clstShaderSrc(fb_glsl);
    CLSTlayer *fblayer;
    fblayer = clstLayerShader(16.0f, 9.0f, fbshader);
    clstLayerAddSprite(fblayer, &fbsprite);

    clstAudioPlayerSetPitch(audio_player, 0.6f);

    clst->world_gravity = 12.0f;

    clst->last_physics_update = prevtime = prevtime2 = clstTime();
    memset(fps_text, 0, 20);
    frames = 0;
    float speed = 8.0f;
    float jump = 18.0f;
    int pantoright = 1;
    int vsync = 0;
    while (clst->window.alive)
    {
        clstWindowClear();

        if (clstKey(CELESTE_KEY_LEFT_CONTROL))
            clstFrameBufferBind(fb);

        clstSceneRender(clst->scene);

        if (clstKey(CELESTE_KEY_LEFT_SHIFT))
            clstLayerRender(layer_debug);

        if (clstKey(CELESTE_KEY_LEFT_CONTROL)) {
            clstFrameBufferUnbind();
            clstLayerRender(fblayer);
        }

        if (!clst->window.focused)
            clstWaitEv(clst);
        clstUpdate(clst);

        frames++;
        if ((clstTime() - prevtime) >= 1.0)
        {
            prevtime = clstTime();
            sprintf(fps_text, "%d fps", frames);
            CELESTE_LOG("%d fps\n", frames);
            clstClickRemoveCallback(CELESTE_MOUSE_RIGHT);
            frames = 0;
        }

        if ((clstTime() - prevtime2) >= 0.1)
        {
            prevtime2 = clstTime();
            float pan = clstAudioPlayerGetPan(audio_player);
            if (pan >= 1.0f)
                pantoright = 0;
            else if (pan <= -1.0f)
                pantoright = 1;

            if (pantoright)
                pan += 0.01f;
            else
                pan -= 0.01f;

            clstAudioPlayerSetPan(audio_player, pan);
        }

        switch (button.status)
        {
            case BUTTON_STATUS_NONE:
                button_col.color = 0x3A55555;
                button_label.color = 0xFFFFFFFF;
                break;
            case BUTTON_STATUS_FOCUSED:
                button_col.color = 0x6FFFFF00;
                button_label.color = 0x88BBBBBB;
                break;
            case BUTTON_STATUS_CLICKED:
                clst->window.alive = 0;
                break;
        }

        if (clstKey(CELESTE_KEY_LEFT))
            body1->velocity[0] = -speed;
        if (clstKey(CELESTE_KEY_RIGHT))
            body1->velocity[0] = speed;

        if (!clstKey(CELESTE_KEY_LEFT) && !clstKey(CELESTE_KEY_RIGHT))
            body1->velocity[0] = 0.0f;

        if (clstKey(CELESTE_KEY_SPACE) && clstCollisionRectangles(body1->position, body1->size, floor->position, floor->size))
            body1->velocity[1] = jump;

        if (clstCollisionRectangles(body1->position, body1->size, button_body->position, button_body->size))
        {
            button_col.color = 0x6FFFFF00;
            button_label.color = 0x88BBBBBB;

            if (clstKey(CELESTE_KEY_SPACE) && (*body1->position)[1] >= button_pos[1] + button_col.size[1])
                body1->velocity[1] = jump;
        }

        if (clstCollisionRectangles(body1->position, body1->size, body2->position, body2->size)) {
            sprintf(colliding, "Colliding");

            if (clstKey(CELESTE_KEY_SPACE) && (*body1->position)[1] >= (*body2->position)[1] + (*body2->size)[1])
                body1->velocity[1] = jump;
        }
        else {
            colliding[0] = '\0';
        }

        if (clstKey(CELESTE_KEY_LEFT_CONTROL) && clstKey(CELESTE_KEY_V)) {
            vsync = !vsync;
            clstWindowSwapInterval(vsync);
        }
    }

    clstShaderDestroy(fbshader);
    clstLayerDestroy(fblayer);
    clstTextureDestroy(fbtex);
    clstFrameBufferDestroy(fb);

    clstAudioPlayerDestroy(audio_player);
    clstAudioDestroy(audio);

    clstBodyDestroy(body1);
    clstBodyDestroy(body2);
    clstBodyDestroy(button_body);
    clstBodyDestroy(floor);
    clstBodyDestroy(left);
    clstBodyDestroy(right);

    clstAnimationDestroy(space_anim);
    clstAnimationDestroy(carpet_anim);

    clstFontDestroy(font);

    clstSceneDestroy(scene);
    clstLayerDestroy(layer_debug);
    clstGroupDestroy(button_group);

    clstTextureDestroy(texture_atlas);
    clstTextureDestroy(texture_space);
    clstTextureDestroy(texture_celeste);

    clstLoaderSaveData(loader);
    clstLoaderLoadData(loader);
    clstLoaderDestroy(loader);

    clstWindowDestroy();
    clstTerminate();
    return 0;
}
