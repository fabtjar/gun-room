#include "baddie.h"
#include "raymath.h"
#include <stdlib.h>
#include "math_utils.h"

Baddie *add_baddie(Baddie baddie[], Vector2 player_pos) {
    for (int i = 0; i < BADDIE_N; i++) {
        Baddie *b = &baddie[i];
        if (b->active) continue;

        int game_width = b->world->width;
        int game_height = b->world->height;

        b->pos = player_pos;

        while (Vector2Distance(b->pos, player_pos) < BADDIE_MIN_DIST) {
            if (rand() % 2 == 0) {
                b->pos.x = rand() % 2 == 0 ? -BADDIE_OUTSIDE_DIST : game_width + BADDIE_OUTSIDE_DIST;
                b->pos.y = rand() % game_height;
            } else {
                b->pos.y = rand() % 2 == 0 ? -BADDIE_OUTSIDE_DIST : game_height + BADDIE_OUTSIDE_DIST;
                b->pos.x = rand() % game_width;
            }
        }

        b->active = true;
        b->checkBoundary = false;
        Vector2 dist = Vector2Subtract((Vector2) {b->world->width / 2, b->world->height / 2}, b->pos);
        Vector2 normal = Vector2Normalize(dist);
        b->vel = Vector2Scale(normal, BADDIE_SPEED);
        b->angle = Vector2Angle(Vector2Zero(), b->vel);
        b->wait_timer = 0;
        return b;
    }
    return NULL;
}

void update_baddies(Baddie baddies[], Vector2 player_pos, float dt) {
    for (int i = 0; i < BADDIE_N; i++) {
        Baddie *b = &baddies[i];

        if (!b->active)
            continue;

        b->pos = Vector2Add(b->pos, Vector2Scale(b->vel, dt));

        int game_width = b->world->width;
        int game_height = b->world->height;

        if (b->pos.x > 0 && b->pos.x < game_width && b->pos.y > 0 && b->pos.y < game_height)
            b->checkBoundary = true;

        if (!b->checkBoundary)
            continue;

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

        b->wait_timer -= dt;
        if (b->wait_timer < 0) {
            b->wait_timer = BADDIE_FOLLOW_DELAY_MIN + rand() % (BADDIE_FOLLOW_DELAY_MAX - BADDIE_FOLLOW_DELAY_MIN);
            Vector2 d = Vector2Subtract(player_pos, b->pos);
            float angle = atan2f(d.y, d.x);
            angle += (rand() % BADDIE_FOLLOW_ANGLE * DEG2RAD) * (rand() % 2 == 0 ? -1 : 1);
            b->vel = Vector2Rotate((Vector2) {BADDIE_SPEED, 0}, angle);
        }

        b->target_angle = Vector2Angle(Vector2Zero(), b->vel);
        b->angle = approach_angle(b->angle, b->target_angle, BADDIE_ANGLE_VEL * DEG2RAD * dt);
    }
}

void draw_baddies(Baddie baddies[], void(*draw_func)(Texture2D, Rectangle, Vector2, float)) {
    Rectangle src_rect = {0, 0, 32, 32};

    for (int i = 0; i < BADDIE_N; i++) {
        Baddie b = baddies[i];
        if (!b.active)
            continue;
        draw_func(*b.texture, src_rect, b.pos, b.angle);
    }
}
