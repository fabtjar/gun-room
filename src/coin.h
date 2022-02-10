#ifndef GUN_ROOM_COIN_H
#define GUN_ROOM_COIN_H

#define COIN_MIN_DIST 100
#define COIN_EDGE_GAP 50

#define COIN_ANGLE_VEL_MIN 1000
#define COIN_ANGLE_VEL_MAX 1200
#define COIN_ANGLE_DECEL 1000
#define COIN_FOLLOW_DIST 50
#define COIN_FOLLOW_VEL 400
#define COIN_DECEL 100

#include "raylib.h"
#include "world.h"

typedef struct Coin {
    World *world;
    Texture2D *texture;
    Vector2 pos;
    Vector2 vel;
    float angle;
    float angleVel;
} Coin;

void move_coin(Coin *coin, Vector2 player_pos);

void update_coin(Coin *coin, float dt);

void draw_coin(Coin *coin, void(*draw_func)(Texture2D, Rectangle, Vector2, float, float));

void coin_follow(Coin *coin, Vector2 pos);

#endif //GUN_ROOM_COIN_H
