#include "boom.h"
#include <stdlib.h>

Boom *add_boom(Boom booms[], Vector2 pos) {
    for (int i = 0; i < BOOM_N; i++) {
        Boom *b = &booms[i];
        if (b->active)
            continue;
        b->active = true;
        b->pos = pos;
        b->time = 0;
        b->angle = (rand() % 4) * PI / 2;
        return b;
    }
    return NULL;
}

void update_booms(Boom booms[], float dt) {
    for (int i = 0; i < BOOM_N; i++) {
        Boom *b = &booms[i];
        if (!b->active)
            continue;

        b->time += dt;
        int frame_count = b->texture->width / 32;
        float anim_len = (float) frame_count / BOOM_FPS;
        if (b->time >= anim_len)
            b->active = false;
    }
}

void draw_booms(Boom booms[], void(*draw_func)(Texture2D, Rectangle, Vector2, float, float)) {
    for (int i = 0; i < BOOM_N; i++) {
        Boom b = booms[i];
        if (!b.active)
            continue;

        int frame = (int) (b.time / (1.0 / BOOM_FPS));
        Rectangle src_rect = {32 * frame, 0, 32, 32};
        draw_func(*b.texture, src_rect, b.pos, b.angle, 1);
    }
}
