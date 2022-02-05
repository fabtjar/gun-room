#ifndef GUN_ROOM_BADDIE_H
#define GUN_ROOM_BADDIE_H

#include "world.h"
#include "raylib.h"

#define BADDIE_N 100
#define BADDIE_SPEED 80

typedef struct Baddie {
    World *world;
    Vector2 pos;
    Vector2 vel;
    bool active;
} Baddie;

Baddie *add_baddie(Baddie baddie[], Vector2 pos);

void update_baddies(Baddie baddies[], float dt);

void draw_baddies(Baddie baddies[], Texture2D texture);

#endif //GUN_ROOM_BADDIE_H
