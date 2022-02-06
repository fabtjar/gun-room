#ifndef GUN_ROOM_BADDIE_H
#define GUN_ROOM_BADDIE_H

#include "world.h"
#include "raylib.h"

#define BADDIE_N 100
#define BADDIE_SPEED 80
#define BADDIE_MIN_DIST 200
#define BADDIE_OUTSIDE_DIST 20

typedef struct Baddie {
    World *world;
    Texture2D *texture;
    Vector2 pos;
    Vector2 vel;
    bool active;
    bool checkBoundary;
} Baddie;

Baddie *add_baddie(Baddie baddie[], Vector2 player_pos);

void update_baddies(Baddie baddies[], float dt);

void draw_baddies(Baddie baddies[], void(*draw_func)(Texture2D, Rectangle, Vector2, float));

#endif //GUN_ROOM_BADDIE_H
