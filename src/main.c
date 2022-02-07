#include <stdio.h>
#include "raylib.h"
#include "raymath.h"
#include "world.h"
#include "player.h"
#include "baddie.h"
#include "bullet.h"
#include "coin.h"
#include "boom.h"

#define WIDTH 480
#define HEIGHT 270
#define SCALE 2

#define SHOOT_DELAY 0.1
#define TOUCHING_DIST 20

#define DEBUG_TEXT_N 1000

static Texture2D player_texture;
static Texture2D baddie_texture;
static Texture2D bullet_texture;
static Texture2D coin_texture;
static Texture2D boom_texture;
static Texture2D bg_texture;
static World world = {.width=WIDTH, .height=HEIGHT};
static Player player = {0};
static Baddie baddies[BADDIE_N] = {0};
static Bullet bullets[BULLET_N] = {0};
static Boom booms[BOOM_N] = {0};
static Coin coin = {0};
static char debug_text[DEBUG_TEXT_N] = "";

static void init();

static void update();

static void draw();

static void unload();

static void scaled_draw(Texture2D texture, Rectangle src_rect, Vector2 pos, float angle);

static bool is_touching(Vector2 a, Vector2 b);

int main(void) {
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(WIDTH * SCALE, HEIGHT * SCALE, "Gun Room");
    SetWindowMinSize(WIDTH, HEIGHT);

    player_texture = LoadTexture("content/gunner.png");
    baddie_texture = LoadTexture("content/baddie.png");
    bullet_texture = LoadTexture("content/bullet.png");
    coin_texture = LoadTexture("content/coin.png");
    boom_texture = LoadTexture("content/boom.png");
    bg_texture = LoadTexture("content/grid.png");

    init();
    while (!WindowShouldClose()) {
        update();
        draw();
    }
    unload();

    CloseWindow();
    return 0;
}

void init() {
    player.world = &world;
    player.texture = &player_texture;
    player.pos = (Vector2) {WIDTH / 2, HEIGHT / 2};
    player.vel = Vector2Zero();

    for (int i = 0; i < BADDIE_N; i++) {
        baddies[i].world = &world;
        baddies[i].texture = &baddie_texture;
        baddies[i].active = false;
    }

    add_baddie(baddies, player.pos);

    for (int i = 0; i < BULLET_N; i++) {
        bullets[i].world = &world;
        bullets[i].texture = &bullet_texture;
        bullets[i].active = false;
    }

    for (int i = 0; i < BOOM_N; i++) {
        booms[i].texture = &boom_texture;
        booms[i].active = false;
    }

    coin.world = &world;
    coin.texture = &coin_texture;
    move_coin(&coin, player.pos);
    add_boom(booms, coin.pos);
}

void update() {
    float dt = GetFrameTime();

    update_player(&player, dt);
    update_baddies(baddies, dt);
    update_bullets(bullets, dt);
    update_coin(&coin, dt);
    update_booms(booms, dt);

    if (player.is_shooting && GetTime() > player.shoot_timer) {
        player.shoot_timer = GetTime() + SHOOT_DELAY;
        add_bullet(bullets, player.pos, player.angle);
    }

    if (is_touching(player.pos, coin.pos)) {
        move_coin(&coin, player.pos);
        add_boom(booms, coin.pos);
    }

    coin_follow(&coin, player.pos);

    for (int i = 0; i < BULLET_N; i++) {
        Bullet *bullet = &bullets[i];
        if (!bullet->active) continue;
        for (int j = 0; j < BADDIE_N && bullet->active; j++) {
            Baddie *baddie = &baddies[j];
            if (!baddie->active) continue;
            if (is_touching(bullet->pos, baddie->pos)) {
                bullet->active = false;
                baddie->active = false;
                add_boom(booms, baddie->pos);
            }
        }
    }

    sprintf(debug_text, "%d\n%d", (int) player.pos.x, (int) player.pos.y);

    if (IsKeyPressed(KEY_SPACE))
        add_baddie(baddies, player.pos);

    if (IsKeyPressed(KEY_R))
        init();
}


void draw() {
    BeginDrawing();
    {
        ClearBackground(BLACK);

        Rectangle screen_rect = {0, 0, WIDTH * SCALE, HEIGHT * SCALE};
        Rectangle grid_rect = {0, 0, 32, 32};
        DrawTextureTiled(bg_texture, grid_rect, screen_rect, (Vector2) {0, 0}, 0, SCALE, WHITE);

        draw_coin(&coin, scaled_draw);
        draw_baddies(baddies, scaled_draw);
        draw_bullets(bullets, scaled_draw);
        draw_player(&player, scaled_draw);
        draw_booms(booms, scaled_draw);

        DrawText(debug_text, 8, 8, 40, BLACK);
    }
    EndDrawing();
}

void unload() {
    UnloadTexture(player_texture);
    UnloadTexture(baddie_texture);
    UnloadTexture(bullet_texture);
    UnloadTexture(coin_texture);
    UnloadTexture(boom_texture);
    UnloadTexture(bg_texture);
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

bool is_touching(Vector2 a, Vector2 b) {
    return Vector2LengthSqr(Vector2Subtract(a, b)) < TOUCHING_DIST * TOUCHING_DIST;
}
