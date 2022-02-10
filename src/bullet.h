#ifndef GUN_ROOM_BULLET_H
#define GUN_ROOM_BULLET_H

#include "world.h"
#include "raylib.h"
#include "raymath.h"

#define BULLET_N 20
#define BULLET_SPEED 500

typedef struct Bullet {
    World *world;
    Texture2D *texture;
    Vector2 pos;
    Vector2 vel;
    bool active;
} Bullet;

Bullet *add_bullet(Bullet bullets[], Vector2 pos, float angle);

void update_bullets(Bullet bullets[], float dt);

void draw_bullets(Bullet bullets[], void(*draw_func)(Texture2D, Rectangle, Vector2, float, float));

#endif //GUN_ROOM_BULLET_H
