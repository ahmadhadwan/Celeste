#include <celeste/celeste.h>
#include <cglm/cglm.h>
#include <stdio.h>
#ifdef __linux__
    #include <sys/resource.h>
#endif /* __linux__ */

static celeste_animation_t *space_animation(celeste_sprite_t *sprites, celeste_texture_t *texture_space)
{
    float pos[3] = { -16.0f, -9.0f, 0.0f };
    float size[2] = { 32.0f, 18.0f };

    celeste_sprite_create_tex(pos, size, texture_space, &(sprites[0]));
    celeste_sprite_create_tex(pos, size, texture_space, &(sprites[1]));
    celeste_sprite_create_tex(pos, size, texture_space, &(sprites[2]));
    celeste_sprite_create_tex(pos, size, texture_space, &(sprites[3]));

    glm_vec2((float[]){ 0.0f, 0.0f }, sprites[0].uv[0]);
    glm_vec2((float[]){ 0.0f, 4.0f }, sprites[0].uv[1]);
    glm_vec2((float[]){ 1.0f, 4.0f }, sprites[0].uv[2]);
    glm_vec2((float[]){ 1.0f, 0.0f }, sprites[0].uv[3]);

    glm_vec2((float[]){ 0.0f + (3.0f / 4.0f), 0.0f }, sprites[1].uv[0]);
    glm_vec2((float[]){ 0.0f + (3.0f / 4.0f), 4.0f }, sprites[1].uv[1]);
    glm_vec2((float[]){ 1.0f + (3.0f / 4.0f), 4.0f }, sprites[1].uv[2]);
    glm_vec2((float[]){ 1.0f + (3.0f / 4.0f), 0.0f }, sprites[1].uv[3]);

    glm_vec2((float[]){ 0.0f + (2.0f / 4.0f), 0.0f }, sprites[2].uv[0]);
    glm_vec2((float[]){ 0.0f + (2.0f / 4.0f), 4.0f }, sprites[2].uv[1]);
    glm_vec2((float[]){ 1.0f + (2.0f / 4.0f), 4.0f }, sprites[2].uv[2]);
    glm_vec2((float[]){ 1.0f + (2.0f / 4.0f), 0.0f }, sprites[2].uv[3]);

    glm_vec2((float[]){ 0.0f + (1.0f / 4.0f), 0.0f }, sprites[3].uv[0]);
    glm_vec2((float[]){ 0.0f + (1.0f / 4.0f), 4.0f }, sprites[3].uv[1]);
    glm_vec2((float[]){ 1.0f + (1.0f / 4.0f), 4.0f }, sprites[3].uv[2]);
    glm_vec2((float[]){ 1.0f + (1.0f / 4.0f), 0.0f }, sprites[3].uv[3]);

    return celeste_animation_create((celeste_sprite_t*[]){sprites, sprites + 1, sprites + 2, sprites + 3}, 4, 0.25f);
}

static celeste_animation_t *carpet_animation(celeste_sprite_t *sprites, celeste_texture_t *texture_atlas)
{
    vec3 pos = { -8.0f, -6.0f, 0.0f };
    vec2 size = { 16.0f, 16.0f };
    const int x = 16;
    const int y = 10;
    const float spriteWidth = 48.0f;
    const float spriteHeight = 48.0f;

    celeste_sprite_create_tex_from_atlas(pos, size, texture_atlas, (float[]){x, y}, (float[]){spriteWidth, spriteHeight}, sprites + 0);
    celeste_sprite_create_tex_from_atlas(pos, size, texture_atlas, (float[]){x, y + 3}, (float[]){spriteWidth, spriteHeight}, sprites + 1);

    return celeste_animation_create((celeste_sprite_t*[]){sprites, sprites + 1}, 2, 0.75f);
}

int main()
{
    celeste_t *clst;
    celeste_layer_t *layer;
    celeste_texture_t *texture_atlas, *texture_space, *texture_celeste;

    celeste_animation_t *space_anim, *carpet_anim;
    celeste_sprite_t space[4];

    celeste_sprite_t carpet, carpet2, celeste;
    celeste_label_t fps;
    celeste_font_t *font;

    celeste_button_t button;
    celeste_group_t *button_group;
    celeste_sprite_t button_col;
    celeste_label_t button_label;

    celeste_audio_t *audio;

    double prevtime;
    unsigned int frames;

    char fps_text[20];
    celeste_scene_t *scene;

    clst = celeste_init();
    if (!clst)
        return 1;

    if (celeste_window_create(clst, "Celeste Engine Sandbox"))
        return 1;

    layer = celeste_layer_create(-16.0f, 16.0f, -9.0f, 9.0f, 1.0f, -1.0f);

    texture_atlas = celeste_texture_create("res/textures/atlas_48x.png");
    texture_space = celeste_texture_create("res/textures/space8_4-frames.png");
    texture_celeste = celeste_texture_create("res/icons/celeste_48x48.png");

    space_anim = space_animation(space, texture_space);
    celeste_layer_add_sprite(layer, space_anim);

    carpet_anim = carpet_animation((celeste_sprite_t[]){carpet, carpet2}, texture_atlas);
    celeste_layer_add_sprite(layer, carpet_anim);

    celeste_sprite_create_tex((float[]){ -4.0f, 1.0f, 0.0f }, (float[]){ 8.0f, 8.0f }, texture_celeste, &celeste);
    celeste_layer_add_sprite(layer, &celeste);

    font = celeste_font_create("res/fonts/SourceSansPro-Light.ttf", 72);

    button_group = celeste_group_create((float[]){-4.0f, -7.5f, 0.0f});
    celeste_sprite_create_col((float[]){ 0.0f, 0.0f, 0.0f }, (float[]){ 8.0f, 4.0f }, 0x3A555555, &button_col);
    celeste_group_add_sprite(button_group, &button_col);
    celeste_label_create_col((float[]){ 3.0f, 2.0f, 0.0f }, "Quit", font, 0xFFFFFFFF, &button_label);
    celeste_group_add_sprite(button_group, &button_label);
    celeste_button_create(button_group, &button);
    celeste_layer_add_sprite(layer, &button);

    celeste_label_create((float[]){ -16.0f, 8.0f, 0.0f }, fps_text, font, &fps);

    celeste_layer_add_sprite(layer, &fps);

    scene = celeste_scene_create();
    celeste_scene_add_layer(scene, layer);
    clst->scene = scene;

    audio = celeste_audio_create("res/audio/fss.ogg", 1);
    celeste_audio_play(audio);

#ifdef __linux__
    {
        struct rusage ru;
        getrusage(RUSAGE_SELF, &ru);
        printf("Max RAM Usage: %ld Kb\n", ru.ru_maxrss);
    }
#endif /* __linux__ */

    prevtime = celeste_get_time();
    frames = 0;
    while (clst->winalive)
    {
        celeste_window_clear();

        celeste_scene_render(clst->scene);

        celeste_update(clst);

        frames++;
        if ((celeste_get_time() - prevtime) >= 1.0)
        {
            prevtime = celeste_get_time();
            sprintf(fps_text, "%d fps", frames);
            printf("%d fps\n", frames);
            frames = 0;
        }

        switch (button.status)
        {
            case CELESTE_BUTTON_STATUS_NONE:
                button_col.color = 0x3A55555;
                button_label.color = 0xFFFFFFFF;
                if (celeste_mouse_button_clicked(CELESTE_MOUSE_LEFT))
                    celeste_audio_set_gain(audio, 1.5f);
                if (celeste_mouse_button_clicked(CELESTE_MOUSE_RIGHT))
                    celeste_audio_set_gain(audio, 0.25f);
                if (celeste_mouse_button_clicked(CELESTE_MOUSE_MIDDLE))
                    celeste_audio_set_gain(audio, 1.0f);
                break;
            case CELESTE_BUTTON_STATUS_FOCUSED:
                button_col.color = 0x6FFFFF00;
                button_label.color = 0x88BBBBBB;
                break;
            case CELESTE_BUTTON_STATUS_CLICKED:
                clst->winalive = 0;
                break;
        }
    }

    celeste_audio_destroy(audio);

    celeste_animation_destroy(space_anim);
    celeste_animation_destroy(carpet_anim);

    celeste_font_destroy(font);

    celeste_scene_destroy(scene);
    celeste_group_destroy(button_group);

    celeste_texture_destroy(texture_atlas);
    celeste_texture_destroy(texture_space);
    celeste_texture_destroy(texture_celeste);

    celeste_terminate();
    return 0;
}
