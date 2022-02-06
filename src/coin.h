#ifndef GUN_ROOM_COIN_H
#define GUN_ROOM_COIN_H

#define COIN_MIN_DIST 100
#define COIN_EDGE_GAP 50

#define COIN_ANGLE_VEL_MIN 1000
#define COIN_ANGLE_VEL_MAX 1200
#define COIN_ANGLE_DECEL 1000

#include "raylib.h"
#include "world.h"

typedef struct Coin {
    World *world;
    Texture2D *texture;
    Vector2 pos;
    float angle;
    float angleVel;
} Coin;

void move_coin(Coin *coin, Vector2 player_pos);

void update_coin(Coin *coin, float dt);

void draw_coin(Coin *coin, void(*draw_func)(Texture2D, Rectangle, Vector2, float));

#endif //GUN_ROOM_COIN_H
