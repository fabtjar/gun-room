#include "bullet.h"
#include <stdlib.h>

Bullet *add_bullet(Bullet bullets[], Vector2 pos, float angle) {
    for (int i = 0; i < BULLET_N; i++) {
        Bullet *b = &bullets[i];
        if (b->active) continue;
        b->active = true;
        b->pos = pos;
        b->vel = Vector2Rotate((Vector2) {BULLET_SPEED, 0}, angle);
        return b;
    }
    return NULL;
}

void update_bullets(Bullet bullets[], float dt) {
    for (int i = 0; i < BULLET_N; i++) {
        Bullet *b = &bullets[i];
        if (!b->active) continue;
        b->pos = Vector2Add(b->pos, Vector2Scale(b->vel, dt));
        if (b->pos.x + 10 < 0 || b->pos.x - 10 > (float) b->world->width ||
            b->pos.y + 10 < 0 || b->pos.y - 10 > (float) b->world->height)
            b->active = false;
    }
}

void draw_bullets(Bullet bullets[], Texture2D texture) {
    for (int i = 0; i < BULLET_N; i++) {
        Bullet b = bullets[i];
        if (b.active) DrawTexture(texture, b.pos.x - 10, b.pos.y - 10, WHITE);
    }
}
