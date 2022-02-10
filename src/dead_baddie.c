#include "dead_baddie.h"
#include "baddie.h"
#include "raymath.h"
#include <stdlib.h>
#include "math_utils.h"

DeadBaddie *add_dead_baddie(DeadBaddie dead_baddie[], Vector2 baddie_pos, Vector2 player_pos) {
    for (int i = 0; i < BADDIE_N; i++) {
        DeadBaddie *b = &dead_baddie[i];

        if (b->active)
            continue;

        b->active = true;
        b->visible = true;
        b->pos = baddie_pos;
        Vector2 dist = Vector2Subtract(baddie_pos, player_pos);
        Vector2 normal = Vector2Normalize(dist);
        b->vel = Vector2Scale(normal, DEAD_BADDIE_VEL);
        b->angle = Vector2Angle(Vector2Zero(), b->vel);
        b->timer = 0;
        return b;
    }
    return NULL;
}

void update_dead_baddies(DeadBaddie dead_baddie[], float dt) {
    for (int i = 0; i < BADDIE_N; i++) {
        DeadBaddie *b = &dead_baddie[i];

        if (!b->active)
            continue;

        b->vel.x = approach(b->vel.x, 0, DEAD_BADDIE_DECEL * dt);
        b->vel.y = approach(b->vel.y, 0, DEAD_BADDIE_DECEL * dt);
        b->pos.x += b->vel.x * dt;
        b->pos.y += b->vel.y * dt;

        int game_width = b->world->width;
        int game_height = b->world->height;

        if (b->pos.x < 0) {
            b->pos.x = 0;
            b->vel.x *= -1;
        } else if (b->pos.x > game_width) {
            b->pos.x = (float) game_width;
            b->vel.x *= -1;
        }
        if (b->pos.y < 0) {
            b->pos.y = 0;
            b->vel.y *= -1;
        } else if (b->pos.y > game_height) {
            b->pos.y = game_height;
            b->vel.y *= -1;
        }

        b->timer += dt;
        if (b->timer > DEAD_BADDIE_BLINK_START_TIME) {
            b->visible = fmodf(b->timer, DEAD_BADDIE_BLINK_TIME * 2) < DEAD_BADDIE_BLINK_TIME;
            if (!b->visible && b->timer > DEAD_BADDIE_REMOVE_TIME)
                b->active = false;
        }
    }
}

void draw_dead_baddies(DeadBaddie dead_baddie[], void(*draw_func)(Texture2D, Rectangle, Vector2, float, float)) {
    Rectangle src_rect = {32, 0, 32, 32};

    for (int i = 0; i < BADDIE_N; i++) {
        DeadBaddie b = dead_baddie[i];
        if (!b.active || !b.visible)
            continue;
        draw_func(*b.texture, src_rect, b.pos, b.angle, 1);
    }
}
