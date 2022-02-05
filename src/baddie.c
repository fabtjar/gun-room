#include "baddie.h"
#include "raymath.h"
#include <stdlib.h>

Baddie *add_baddie(Baddie baddie[]) {
    int width = baddie[0].world->width;
    int height = baddie[0].world->height;
    Vector2 pos;

    if (rand() % 2 == 0) {
        pos.x = rand() % 2 == 0 ? 0 : width;
        pos.y = rand() % height;
    } else {
        pos.y = rand() % 2 == 0 ? 0 : height;
        pos.x = rand() % width;
    }

    for (int i = 0; i < BADDIE_N; i++) {
        Baddie *b = &baddie[i];
        if (b->active) continue;
        b->active = true;
        b->pos = pos;
        Vector2 middle = (Vector2) {b->world->width / 2, b->world->height / 2};
        Vector2 dist = Vector2Subtract(middle, b->pos);
        Vector2 normal = Vector2Normalize(dist);
        b->vel = Vector2Scale(normal, BADDIE_SPEED);
        return b;
    }
    return NULL;
}

void update_baddies(Baddie baddies[], float dt) {
    for (int i = 0; i < BADDIE_N; i++) {
        Baddie *b = &baddies[i];
        if (!b->active) continue;
        b->pos = Vector2Add(b->pos, Vector2Scale(b->vel, dt));
        if (b->pos.x < 0) {
            b->pos.x = 0;
            b->vel.x *= -1;
        } else if (b->pos.x > (float) b->world->width) {
            b->pos.x = (float) b->world->width;
            b->vel.x *= -1;
        }
        if (b->pos.y < 0) {
            b->pos.y = 0;
            b->vel.y *= -1;
        } else if (b->pos.y > (float) b->world->height) {
            b->pos.y = (float) b->world->height;
            b->vel.y *= -1;
        }
    }
}

void draw_baddies(Baddie baddies[], Texture2D texture) {
    Rectangle src_rect = {0, 0, 32, 32};
    Rectangle dest_rect = {0, 0, 32, 32};
    Vector2 middle = {src_rect.width / 2, src_rect.height / 2};

    for (int i = 0; i < BADDIE_N; i++) {
        Baddie b = baddies[i];
        if (!b.active) continue;
        dest_rect.x = b.pos.x;
        dest_rect.y = b.pos.y;
        float angle = Vector2Angle(Vector2Zero(), b.vel);
        DrawTexturePro(texture, src_rect, dest_rect, middle, angle * RAD2DEG, WHITE);
    }
}
