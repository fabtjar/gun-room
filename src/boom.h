#ifndef GUN_ROOM_BOOM_H
#define GUN_ROOM_BOOM_H

#include "raylib.h"

#define BOOM_N 200
#define BOOM_FPS 15

typedef struct Boom {
    Texture2D *texture;
    Vector2 pos;
    float time;
    float angle;
    bool active;
} Boom;

Boom *add_boom(Boom booms[], Vector2 pos);

void update_booms(Boom booms[], float dt);

void draw_booms(Boom booms[], void(*draw_func)(Texture2D, Rectangle, Vector2, float, float));

#endif //GUN_ROOM_BOOM_H
