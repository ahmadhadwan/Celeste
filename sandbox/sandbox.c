#include <celeste/celeste.h>
#include <cglm/cglm.h>
#include <stdio.h>
#include <string.h>

#include "../res/shaders/fb.c"
#include "../res/icons/celeste_48x48.h"

static void destroy_window(void *winalive)
{
    *((int*)winalive) = 0;
}

static CLSTlayer *setup_framebuffer_layer(CLSTframebuffer **fb, CLSTtexture **fbtex, CLSTshader **fbshader)
{
    CLSTlayer *fblayer;
    CLSTsprite *fbsprite;
    *fb = clstFrameBuffer();
    *fbtex = clstTextureInline(NULL, 1920, 1080, 24, "fb");
    clstFrameBufferAttachTexture(*fb, *fbtex);
    clstFrameBufferAttachRenderBuffer(*fb, NULL);

    fbsprite = clstSprite((float[]){-16.0f, -9.0f}, (float[]){32.0f, 18.0f}, *fbtex, "FBSprite");
    *fbshader = clstShaderSrc(fb_glsl);
    fblayer = clstLayerShader(16.0f, 9.0f, *fbshader, "FB Layer");
    clstLayerAddRenderable(fblayer, fbsprite);
    return fblayer;
}

int main()
{
    CLST *clst;
    CLSTscene *scene;
    CLSTloader *loader;

    CLSTbutton *button;
    CLSTsprite *button_col;
    CLSTlabel *button_label;

    CLSTaudioplayer *audio_player;
    CLSTlayer *layer_debug;
    char fps_text[20];
    CLSTlabel *fps;

    double prevtime, prevtime2;
    uint32_t frames;
    float speed = 8.0f;
    float jump = 18.0f;
    int pantoright = 1;

    char input_str[1024];
    CLSTlabel *input_label;

    clst = clstInit();
    if (clst == NULL)
        return 1;

    if (clstWindow("Celeste Engine Sandbox") != CELESTE_OK)
        return 1;

    loader = clstLoader("sandbox.clst");
    scene = clstScene();
    clstSetScene(scene);
    clstLoaderLoadData(loader);

    button = (CLSTbutton *)clstLayerGetRenderable(clstSceneGetLayer(scene, "Base Layer"), "Button");
    button_col = (CLSTsprite *)clstLayerGetRenderable(clstSceneGetLayer(scene, "Base Layer"), "Button Sprite");
    button_label = (CLSTlabel *)clstLayerGetRenderable(clstSceneGetLayer(scene, "Base Layer"), "Button Label");

    audio_player = clstAudioPlayer(clstSceneGetAudio(scene, "space"), 1, 0);
    clstAudioPlayerSetPitch(audio_player, 0.6f);
    clstAudioPlayerPlay(audio_player);

    clstKeyAddCallback((CLSTkey){CELESTE_KEY_ESCAPE, destroy_window, &(clst->window.alive)});
    clstInputSetListener(input_str, 1024);

    memset(input_str, 0, sizeof(input_str));
    input_label = clstLabel((vec2){ -15.5f, -8.5f }, input_str, clstSceneGetFont(scene, "ThaleahFat-72"), "Input Label");
    clstLayerAddRenderable(clstSceneGetLayer(scene, "Base Layer"), input_label);

    CLSTsprite *quad1, *quad2;
    quad1 = clstSpriteCol((vec2){ -10.0f, -2.0f }, (vec2){ 4.0f, 4.0f }, 0xFFFFFF00, "Quad1 Sprite");
    quad2 = clstSpriteCol((vec2){   8.0f, -2.0f }, (vec2){ 4.0f, 4.0f }, 0xFF0000FF, "Quad2 Sprite");
    clstLayerAddRenderable(clstSceneGetLayer(scene, "Base Layer"), quad1);
    clstLayerAddRenderable(clstSceneGetLayer(scene, "Base Layer"), quad2);

    CLSTbody *body1 = clstBody(0, 1, 1, 1, &(quad1->position), &(quad1->size));
    CLSTbody *body2 = clstBody(0, 0, 1, 0, &(quad2->position), &(quad2->size));

    vec2 button_pos = { -4.0f, -7.5f };
    CLSTbody *button_body = clstBody(0, 0, 1, 0, &button_pos, &(button_col->size));

    vec2 floor_pos = { -16.0f, -10.0f };
    vec2 floor_size = { 32.0f, 1.0f };
    CLSTbody *floor = clstBody(1, 0, 1, 0, &floor_pos, &floor_size);
    vec2 left_pos = { -17.0f, -9.0f };
    vec2 left_size = { 1.0f, 18.0f };
    clstBody(1, 0, 1, 0, &left_pos, &left_size);
    vec2 right_pos = { 16.0f, -9.0f };
    vec2 right_size = { 1.0f, 18.0f };
    clstBody(1, 0, 1, 0, &right_pos, &right_size);

    clstSetWorldGravity(12.0f);

    layer_debug = clstLayer(16.0f, 9.0f, "Debug Layer");
    memset(fps_text, 0, sizeof(fps_text));
    fps = clstLabel((vec2){ -15.5f, 8.0f }, "0 fps", clstSceneGetFont(scene, "ThaleahFat-72"), "FPS Label");
    clstLayerAddRenderable(layer_debug, fps);

    CLSTframebuffer *fb;
    CLSTtexture *fbtex;
    CLSTshader *fbshader;
    CLSTlayer *fblayer;
    fblayer = setup_framebuffer_layer(&fb, &fbtex, &fbshader);

    clstLoaderSaveData(loader);

    CLSTlabel *collision_label;
    collision_label = (CLSTlabel *)clstLayerGetRenderable(clstSceneGetLayer(scene, "Base Layer"), "Collision Label");

    clst->last_physics_update = prevtime = prevtime2 = clstTime();
    frames = 0;
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
            clstLabelSetText(fps, fps_text);
            CELESTE_LOG("%d fps\n", frames);
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

        switch (button->status)
        {
            case CELESTE_BUTTON_STATUS_NONE:
                button_col->color = 0x3A55555;
                button_label->color = 0xFFFFFFFF;
                break;
            case CELESTE_BUTTON_STATUS_FOCUSED:
                button_col->color = 0x6FFFFF00;
                button_label->color = 0x88BBBBBB;
                break;
            case CELESTE_BUTTON_STATUS_CLICKED:
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
            button_col->color = 0x6FFFFF00;
            button_label->color = 0x88BBBBBB;

            if (clstKey(CELESTE_KEY_SPACE) && (*body1->position)[1] >= button_pos[1] + button_col->size[1])
                body1->velocity[1] = jump;
        }

        if (clstCollisionRectangles(body1->position, body1->size, body2->position, body2->size)) {
            clstLabelSetText(collision_label, "Colliding");

            if (clstKey(CELESTE_KEY_SPACE) && (*body1->position)[1] >= (*body2->position)[1] + (*body2->size)[1])
                body1->velocity[1] = jump;
        }
        else {
            clstLabelSetText(collision_label, "");
        }
    }

    clstShaderDestroy(fbshader);
    clstLayerDestroy(fblayer);
    clstTextureDestroy(fbtex);
    clstFrameBufferDestroy(fb);

    clstAudioPlayerDestroy(audio_player);

    clstSceneDestroy(scene);
    clstLayerDestroy(layer_debug);

    clstLoaderDestroy(loader);

    clstWindowDestroy();
    clstTerminate();
    return 0;
}
