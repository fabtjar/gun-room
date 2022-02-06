#include "coin.h"
#include "raymath.h"
#include <stdlib.h>
#include "math_utils.h"

void move_coin(Coin *coin, Vector2 player_pos) {
    coin->pos = player_pos;
    while (Vector2Distance(coin->pos, player_pos) < COIN_MIN_DIST) {
        coin->pos.x = COIN_EDGE_GAP + rand() % (coin->world->width - COIN_EDGE_GAP * 2);
        coin->pos.y = COIN_EDGE_GAP + rand() % (coin->world->height - COIN_EDGE_GAP * 2);
    }

    coin->angle = ((float) rand() / (float) RAND_MAX) * PI * 2;
    float angleVel = (COIN_ANGLE_VEL_MIN + (rand() % (COIN_ANGLE_VEL_MAX - COIN_ANGLE_VEL_MIN))) * DEG2RAD;
    coin->angleVel = rand() % 2 == 0 ? -angleVel : angleVel;
}

void update_coin(Coin *coin, float dt) {
    coin->angleVel = approach(coin->angleVel, 0, COIN_ANGLE_DECEL * DEG2RAD * dt);
    coin->angle += coin->angleVel * dt;
}

void draw_coin(Coin *coin, void(*draw_func)(Texture2D, Rectangle, Vector2, float)) {
    Rectangle src_rect = {0, 0, 24, 24};
    draw_func(*coin->texture, src_rect, coin->pos, coin->angle);
}
