#ifndef GUN_ROOM_PLAYER_H
#define GUN_ROOM_PLAYER_H

#define PLAYER_SPEED 150
#define PLAYER_DECEL 100
#define PLAYER_ANGLE_VEL 1000

#include "raylib.h"
#include "world.h"

typedef struct Player {
    World *world;
    Texture2D *texture;
    Vector2 pos;
    Vector2 vel;
    float angle;
    float target_angle;
    bool is_shooting;
    double shoot_timer;
    int frame;
} Player;

void update_player(Player *player, float dt);

void draw_player(Player *player, void(*draw_func)(Texture2D, Rectangle, Vector2, float));

#endif //GUN_ROOM_PLAYER_H
