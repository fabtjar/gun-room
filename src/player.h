#ifndef GUN_ROOM_PLAYER_H
#define GUN_ROOM_PLAYER_H

#include "raylib.h"
#include "world.h"

#define PLAYER_SPEED 150
#define PLAYER_DECEL 100
#define PLAYER_ANGLE_VEL 1000
#define PLAYER_SHOOT_DELAY 0.1
#define PLAYER_SCALE_BIG 1.5
#define PLAYER_SCALE_SPEED 4

typedef struct Player {
    World *world;
    Texture2D *texture;
    Vector2 pos;
    Vector2 vel;
    float scale;
    float angle;
    float target_angle;
    bool is_shooting;
    float shoot_timer;
    int frame;
} Player;

void update_player(Player *player, float dt);

void player_get_coin(Player *player);

void draw_player(Player *player, void(*draw_func)(Texture2D, Rectangle, Vector2, float, float));

#endif //GUN_ROOM_PLAYER_H
