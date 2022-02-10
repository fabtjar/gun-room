#include "player.h"
#include "raymath.h"
#include "math_utils.h"

void update_player(Player *player, float dt) {
    player->vel.x = approach(player->vel.x, 0, PLAYER_DECEL * dt);
    player->vel.y = approach(player->vel.y, 0, PLAYER_DECEL * dt);

    Vector2 dir = {0, 0};
    if (IsKeyDown(KEY_LEFT)) dir.x--;
    if (IsKeyDown(KEY_RIGHT)) dir.x++;
    if (IsKeyDown(KEY_UP)) dir.y--;
    if (IsKeyDown(KEY_DOWN)) dir.y++;

    bool shoot_pressed = IsKeyDown(KEY_Z);

    if (dir.x != 0 || dir.y != 0) {
        if (!shoot_pressed)
            player->vel = Vector2Scale(Vector2Normalize(dir), PLAYER_SPEED);
        player->target_angle = Vector2Angle(Vector2Zero(), dir);
    }

    player->angle = approach_angle(player->angle, player->target_angle, PLAYER_ANGLE_VEL * DEG2RAD * dt);

    player->pos.x += player->vel.x * dt;
    player->pos.y += player->vel.y * dt;

    player->is_shooting = false;
    player->shoot_timer -= dt;
    if (shoot_pressed && player->shoot_timer <= 0) {
        player->shoot_timer = PLAYER_SHOOT_DELAY;
        player->is_shooting = true;
    }

    if (player->pos.x < 0) player->pos.x = 0;
    else if (player->pos.x > player->world->width) player->pos.x = player->world->width;
    if (player->pos.y < 0) player->pos.y = 0;
    else if (player->pos.y > player->world->height) player->pos.y = player->world->height;

    if (player->scale > 1)
        player->scale = approach(player->scale, 1, PLAYER_SCALE_SPEED * dt);

    player->frame = shoot_pressed ? 1 : 0;
}

void player_get_coin(Player *player) {
    player->scale = PLAYER_SCALE_BIG;
}

void draw_player(Player *player, void(*draw_func)(Texture2D, Rectangle, Vector2, float, float)) {
    Rectangle src_rect = {player->frame * 32, 0, 32, 32};
    draw_func(*player->texture, src_rect, player->pos, player->angle, player->scale);
}
