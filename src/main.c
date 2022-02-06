#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "raymath.h"
#include "world.h"
#include "player.h"
#include "baddie.h"
#include "bullet.h"

#define WIDTH 480
#define HEIGHT 270
#define SCALE 2

#define SHOOT_DELAY 0.1
#define COIN_MIN_DIST 100
#define COIN_EDGE_GAP 50
#define TOUCHING_DIST 20

#define DEBUG_TEXT_N 1000

void scaled_draw(Texture2D texture, Rectangle src_rect, Vector2 pos, float angle);

void move_coin(Vector2 *coin_pos, Vector2 player_pos);

bool is_touching(Vector2 a, Vector2 b);

int main(void) {
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(WIDTH * SCALE, HEIGHT * SCALE, "Gun Room");
    SetWindowMinSize(WIDTH, HEIGHT);

    Texture2D player_texture = LoadTexture("content/gunner.png");
    Texture2D baddie_texture = LoadTexture("content/baddie.png");
    Texture2D bullet_texture = LoadTexture("content/bullet.png");
    Texture2D coin_texture = LoadTexture("content/coin.png");
    Texture2D bg_texture = LoadTexture("content/grid.png");

    Rectangle screen_rect = {0, 0, WIDTH * SCALE, HEIGHT * SCALE};
    Rectangle grid_rect = {0, 0, 32, 32};
    Rectangle coin_rect = {0, 0, 24, 24};

    World world = {.width=WIDTH, .height=HEIGHT};

    Player player = {0};
    player.world = &world;
    player.texture = &player_texture;
    player.pos = (Vector2) {WIDTH / 2, HEIGHT / 2};

    Baddie baddies[BADDIE_N];
    for (int i = 0; i < BADDIE_N; i++) {
        baddies[i].world = &world;
        baddies[i].texture = &baddie_texture;
        baddies[i].active = false;
    }

    add_baddie(baddies, player.pos);

    Vector2 coin_pos;
    move_coin(&coin_pos, player.pos);

    Bullet bullets[BULLET_N];
    for (int i = 0; i < BULLET_N; i++) {
        bullets[i].world = &world;
        bullets[i].texture = &bullet_texture;
        bullets[i].active = false;
    }

    char debug_text[DEBUG_TEXT_N];

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        update_player(&player, dt);
        update_baddies(baddies, dt);
        update_bullets(bullets, dt);

        if (player.is_shooting && GetTime() > player.shoot_timer) {
            player.shoot_timer = GetTime() + SHOOT_DELAY;
            add_bullet(bullets, player.pos, player.angle);
        }

        if (is_touching(player.pos, coin_pos)) move_coin(&coin_pos, player.pos);

        for (int i = 0; i < BULLET_N; i++) {
            Bullet *bullet = &bullets[i];
            if (!bullet->active) continue;
            for (int j = 0; j < BADDIE_N && bullet->active; j++) {
                Baddie *baddie = &baddies[j];
                if (!baddie->active) continue;
                if (is_touching(bullet->pos, baddie->pos)) {
                    bullet->active = false;
                    baddie->active = false;
                }
            }
        }

        sprintf(debug_text, "%d\n%d", (int) player.pos.x, (int) player.pos.y);

        if (IsKeyPressed(KEY_SPACE)) add_baddie(baddies, player.pos);

        BeginDrawing();
        {
            ClearBackground(BLACK);

            DrawTextureTiled(bg_texture, grid_rect, screen_rect, (Vector2) {0, 0}, 0, SCALE, WHITE);
            scaled_draw(coin_texture, coin_rect, coin_pos, 0);

            draw_baddies(baddies, scaled_draw);
            draw_bullets(bullets, scaled_draw);
            draw_player(&player, scaled_draw);

            DrawText(debug_text, 8, 8, 40, BLACK);
        }
        EndDrawing();
    }

    UnloadTexture(player_texture);
    UnloadTexture(baddie_texture);
    UnloadTexture(bullet_texture);
    UnloadTexture(coin_texture);
    UnloadTexture(bg_texture);

    CloseWindow();

    return 0;
}

void scaled_draw(Texture2D texture, Rectangle src_rect, Vector2 pos, float angle) {
    Rectangle dest_rect = {pos.x, pos.y, src_rect.width, src_rect.height};
    Vector2 middle = {src_rect.width / 2, src_rect.height / 2};

    middle.x *= SCALE;
    middle.y *= SCALE;
    dest_rect.x *= SCALE;
    dest_rect.y *= SCALE;
    dest_rect.width *= SCALE;
    dest_rect.height *= SCALE;

    DrawTexturePro(texture, src_rect, dest_rect, middle, angle * RAD2DEG, WHITE);
}

void move_coin(Vector2 *coin_pos, Vector2 player_pos) {
    *coin_pos = player_pos;
    while (Vector2Distance(*coin_pos, player_pos) < COIN_MIN_DIST) {
        coin_pos->x = COIN_EDGE_GAP + rand() % (WIDTH - COIN_EDGE_GAP * 2);
        coin_pos->y = COIN_EDGE_GAP + rand() % (HEIGHT - COIN_EDGE_GAP * 2);
    }
}

bool is_touching(Vector2 a, Vector2 b) {
    return Vector2LengthSqr(Vector2Subtract(a, b)) < TOUCHING_DIST * TOUCHING_DIST;
}
