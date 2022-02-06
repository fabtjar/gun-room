#include "player.h"
#include "raymath.h"
#include "math_utils.h"

#define PLAYER_SPEED 150
#define PLAYER_DECEL 100
#define PLAYER_DECEL 100

void update_player(Player *player, float dt) {
    player->vel.x = approach(player->vel.x, 0, PLAYER_DECEL * dt);
    player->vel.y = approach(player->vel.y, 0, PLAYER_DECEL * dt);

    Vector2 dir = {0, 0};
    if (IsKeyDown(KEY_LEFT)) dir.x--;
    if (IsKeyDown(KEY_RIGHT)) dir.x++;
    if (IsKeyDown(KEY_UP)) dir.y--;
    if (IsKeyDown(KEY_DOWN)) dir.y++;

    player->is_shooting = IsKeyDown(KEY_Z);

    if (dir.x != 0 || dir.y != 0) {
        if (!player->is_shooting)
            player->vel = Vector2Scale(Vector2Normalize(dir), PLAYER_SPEED);
        player->angle = Vector2Angle(Vector2Zero(), dir);
    }

    player->pos.x += player->vel.x * dt;
    player->pos.y += player->vel.y * dt;

    if (player->pos.x < 0) player->pos.x = 0;
    else if (player->pos.x > player->world->width) player->pos.x = player->world->width;
    if (player->pos.y < 0) player->pos.y = 0;
    else if (player->pos.y > player->world->height) player->pos.y = player->world->height;

    player->frame = player->is_shooting ? 1 : 0;
}

void draw_player(Player *player, void(*draw_func)(Texture2D, Rectangle, Vector2, float)) {
    Rectangle src_rect = {player->frame * 32, 0, 32, 32};
    draw_func(*player->texture, src_rect, player->pos, player->angle);
}
