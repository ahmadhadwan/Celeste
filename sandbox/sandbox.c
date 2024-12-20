#include <celeste/celeste.h>
#include <cglm/cglm.h>
#include <stdio.h>
#include <string.h>

#include "../res/shaders/fb.c"
#include "../res/icons/celeste_48x48.h"

static void destroy_window(void *_)
{
    clstSetWindowAlive(0);
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
    clstLoaderLoadData(loader, scene);

    button = (CLSTbutton *)clstLayerGetRenderable(clstSceneGetLayer(scene, "Base Layer"), "Button");
    button_col = (CLSTsprite *)clstLayerGetRenderable(clstSceneGetLayer(scene, "Base Layer"), "Button Sprite");
    button_label = (CLSTlabel *)clstLayerGetRenderable(clstSceneGetLayer(scene, "Base Layer"), "Button Label");

    audio_player = clstAudioPlayer(clstSceneGetAudio(scene, "space"), 1, 0);
    clstAudioPlayerSetPitch(audio_player, 0.6f);
    clstAudioPlayerPlay(audio_player);

    clstKeyAddCallback((CLSTkey){CELESTE_KEY_ESCAPE, destroy_window, NULL});
    clstInputSetListener(input_str, 1024);

    memset(input_str, 0, sizeof(input_str));
    input_label = (CLSTlabel *)clstLayerGetRenderable(clstSceneGetLayer(scene, "Base Layer"), "Input Label");

    CLSTsprite *quad1;
    CLSTbody *body1, *body2, *button_body, *floor;
    vec2 button_pos = { -4.0f, -7.5f };
    
    quad1 = (CLSTsprite *)clstLayerGetRenderable(clstSceneGetLayer(scene, "Base Layer"), "Quad1 Sprite");
    body1 = clstSceneGetBody(scene, "Quad1 Body");
    body2 = clstSceneGetBody(scene, "Quad2 Body");
    button_body = clstSceneGetBody(scene, "Button Body");
    floor = clstSceneGetBody(scene, "Floor");

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

    CLSTlabel *collision_label;
    collision_label = (CLSTlabel *)clstLayerGetRenderable(clstSceneGetLayer(scene, "Base Layer"), "Collision Label");

    clstLoaderSaveData(loader);

    clstResetPhysics();
    prevtime = prevtime2 = clstGetTime();
    frames = 0;
    while (clstGetWindowAlive())
    {
        clstWindowClear();

        if (clstKey(CELESTE_KEY_LEFT_CONTROL))
            clstFrameBufferBind(fb);

        clstSceneRender(clstGetScene());

        if (clstKey(CELESTE_KEY_LEFT_SHIFT))
            clstLayerRender(layer_debug);

        if (clstKey(CELESTE_KEY_LEFT_CONTROL)) {
            clstFrameBufferUnbind();
            clstLayerRender(fblayer);
        }

        if (!clstGetWindowFocused())
            clstWaitEv(clst);
        clstUpdate(clst);

        frames++;
        if ((clstGetTime() - prevtime) >= 1.0)
        {
            prevtime = clstGetTime();
            sprintf(fps_text, "%d fps", frames);
            clstLabelSetText(fps, fps_text);
            CELESTE_LOG("%d fps\n", frames);
            frames = 0;
        }

        if ((clstGetTime() - prevtime2) >= 0.1)
        {
            prevtime2 = clstGetTime();
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
                clstSetWindowAlive(0);
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

            if (clstKey(CELESTE_KEY_SPACE) && body1->position[1] >= button_pos[1] + button_col->size[1])
                body1->velocity[1] = jump;
        }

        if (clstCollisionRectangles(body1->position, body1->size, body2->position, body2->size)) {
            clstLabelSetText(collision_label, "Colliding");

            if (clstKey(CELESTE_KEY_SPACE) && body1->position[1] >= body2->position[1] + body2->size[1])
                body1->velocity[1] = jump;
        }
        else {
            clstLabelSetText(collision_label, "");
        }

        clstLabelSetText(input_label, input_str);
        glm_vec2_copy(body1->position, quad1->position);
    }

    clstKeyRemoveCallback(CELESTE_KEY_ESCAPE);

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
