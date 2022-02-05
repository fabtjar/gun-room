#ifndef GUN_ROOM_PLAYER_H
#define GUN_ROOM_PLAYER_H

#include "raylib.h"
#include "world.h"

typedef struct Player {
    World *world;
    Vector2 pos;
    Vector2 vel;
    float angle;
    bool is_shooting;
    double shoot_timer;
    int frame;
} Player;

void update_player(Player *player, float dt);

void draw_player(Player *player, Texture2D texture);

#endif //GUN_ROOM_PLAYER_H
