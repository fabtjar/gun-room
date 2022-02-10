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

    coin->vel = (Vector2) {0, 0};
}

void update_coin(Coin *coin, float dt) {
    coin->angleVel = approach(coin->angleVel, 0, COIN_ANGLE_DECEL * DEG2RAD * dt);
    coin->angle += coin->angleVel * dt;

    coin->vel.x = approach(coin->vel.x, 0, COIN_DECEL * dt);
    coin->vel.y = approach(coin->vel.y, 0, COIN_DECEL * dt);

    coin->pos.x += coin->vel.x * dt;
    coin->pos.y += coin->vel.y * dt;

    if (coin->pos.x < 0) {
        coin->pos.x = 0;
        coin->vel.x *= -1;
    } else if (coin->pos.x > coin->world->width) {
        coin->pos.x = coin->world->width;
        coin->vel.x *= -1;
    }
    if (coin->pos.y < 0) {
        coin->pos.y = 0;
        coin->vel.y *= -1;
    } else if (coin->pos.y > coin->world->height) {
        coin->pos.y = coin->world->height;
        coin->vel.y *= -1;
    }
}

void draw_coin(Coin *coin, void(*draw_func)(Texture2D, Rectangle, Vector2, float, float)) {
    Rectangle src_rect = {0, 0, 24, 24};
    draw_func(*coin->texture, src_rect, coin->pos, coin->angle, 1);
}

void coin_follow(Coin *coin, Vector2 pos) {
    if (Vector2LengthSqr(Vector2Subtract(coin->pos, pos)) > COIN_FOLLOW_DIST * COIN_FOLLOW_DIST)
        return;

    Vector2 d = Vector2Subtract(pos, coin->pos);
    Vector2 n = Vector2Normalize(d);
    Vector2 v = Vector2Scale(n, COIN_FOLLOW_VEL);
    coin->vel.x = v.x;
    coin->vel.y = v.y;
    coin->angleVel = coin->vel.x * DEG2RAD;
}
