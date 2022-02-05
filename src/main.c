#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "raymath.h"
#include "world.h"
#include "player.h"
#include "baddie.h"
#include "bullet.h"

#define WIDTH 400
#define HEIGHT 300
#define SCALE 2

#define SHOOT_DELAY 0.1
#define COIN_MIN_DIST 100
#define COIN_EDGE_GAP 50
#define TOUCHING_DIST 20

#define DEBUG_TEXT_N 1000

void move_coin(Vector2 *coin_pos, Vector2 player_pos);

bool is_touching(Vector2 a, Vector2 b);

int main(void) {
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(WIDTH, HEIGHT, "Gun Room");
    SetWindowMinSize(WIDTH, HEIGHT);
    SetWindowSize(WIDTH * SCALE, HEIGHT * SCALE);

    RenderTexture2D renderTexture = LoadRenderTexture(WIDTH, HEIGHT);

    Texture2D player_texture = LoadTexture("content/gunner.png");
    Texture2D baddie_texture = LoadTexture("content/baddie.png");
    Texture2D bullet_texture = LoadTexture("content/bullet.png");
    Texture2D coin_texture = LoadTexture("content/coin.png");
    Texture2D bg_texture = LoadTexture("content/grid.png");

    Rectangle game_rect = {0, 0, WIDTH, HEIGHT};
    Rectangle render_rect = {0, HEIGHT, WIDTH, -HEIGHT};
    Rectangle screen_rect = {0, 0, WIDTH * SCALE, HEIGHT * SCALE};
    Rectangle grid_rect = {0, 0, 32, 32};
    Vector2 grid_middle = {grid_rect.width / 2, grid_rect.height / 2};
    Rectangle src_rect;
    Rectangle dest_rect;

    World world = {.width=WIDTH, .height=HEIGHT};

    Player player = {0};
    player.world = &world;
    player.pos = (Vector2) {WIDTH / 2, HEIGHT / 2};

    Baddie baddies[BADDIE_N];
    for (int i = 0; i < BADDIE_N; i++) {
        baddies[i].world = &world;
        baddies[i].active = false;
    }

    add_baddie(baddies, (Vector2) {20, 20});
    add_baddie(baddies, (Vector2) {50, 20});
    add_baddie(baddies, (Vector2) {60, 20});
    add_baddie(baddies, (Vector2) {80, 20});


    Vector2 coin_pos;
    move_coin(&coin_pos, player.pos);

    Bullet bullets[BULLET_N];
    for (int i = 0; i < BULLET_N; i++) {
        bullets[i].world = &world;
        bullets[i].active = false;
    }

    char debug_text[DEBUG_TEXT_N];

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        update_player(&player, dt);

        if (player.is_shooting && GetTime() > player.shoot_timer) {
            player.shoot_timer = GetTime() + SHOOT_DELAY;
            add_bullet(bullets, player.pos, player.angle);
        }

        if (is_touching(player.pos, coin_pos)) move_coin(&coin_pos, player.pos);

        update_baddies(baddies, dt);

        update_bullets(bullets, dt);

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

        if (IsKeyPressed(KEY_F)) ToggleFullscreen();
        
        sprintf(debug_text, "%d\n%d", (int) player.pos.x, (int) player.pos.y);

        BeginTextureMode(renderTexture);
        {
            ClearBackground(WHITE);
            DrawTextureTiled(bg_texture, grid_rect, game_rect, (Vector2) {0, 0}, 0, 1, WHITE);
            DrawTexture(coin_texture, coin_pos.x - 12, coin_pos.y - 12, WHITE);

            draw_baddies(baddies, baddie_texture);
            draw_bullets(bullets, bullet_texture);
            draw_player(&player, player_texture);

            DrawText(debug_text, 4, 4, 20, BLACK);
        }
        EndTextureMode();

        BeginDrawing();
        {
            ClearBackground(BLACK);
            DrawTexturePro(renderTexture.texture, render_rect, screen_rect, Vector2Zero(), 0, WHITE);
        }
        EndDrawing();
    }

    UnloadTexture(player_texture);
    UnloadTexture(baddie_texture);
    UnloadTexture(bullet_texture);
    UnloadTexture(coin_texture);
    UnloadTexture(bg_texture);
    UnloadRenderTexture(renderTexture);

    CloseWindow();

    return 0;
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
