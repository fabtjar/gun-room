#ifndef GUN_ROOM_DEAD_BADDIE_H
#define GUN_ROOM_DEAD_BADDIE_H

#include "raylib.h"
#include "world.h"

#define DEAD_BADDIE_VEL 400
#define DEAD_BADDIE_DECEL 500
#define DEAD_BADDIE_BLINK_TIME 0.05
#define DEAD_BADDIE_BLINK_START_TIME 4
#define DEAD_BADDIE_REMOVE_TIME 5

typedef struct DeadBaddie {
    World *world;
    Texture2D *texture;
    Vector2 pos;
    Vector2 vel;
    float timer;
    float angle;
    bool active;
    bool visible;
} DeadBaddie;

DeadBaddie *add_dead_baddie(DeadBaddie dead_baddie[], Vector2 baddie_pos, Vector2 player_pos);

void update_dead_baddies(DeadBaddie dead_baddie[], float dt);

void draw_dead_baddies(DeadBaddie dead_baddie[], void(*draw_func)(Texture2D, Rectangle, Vector2, float, float));

#endif //GUN_ROOM_DEAD_BADDIE_H
