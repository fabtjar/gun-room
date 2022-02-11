#include <stdio.h>
#include <math.h>
#include "raylib.h"
#include "raymath.h"
#include "world.h"
#include "player.h"
#include "baddie.h"
#include "bullet.h"
#include "coin.h"
#include "boom.h"
#include "dead_baddie.h"
#include "math_utils.h"
#include <stdlib.h>

#define WIDTH 480
#define HEIGHT 270
#define SCALE 2

#define TOUCHING_DIST 22
#define RED_ALPHA_SPEED 3
#define FONT_SIZE 64

#define SHAKE_SMALL 5
#define SHAKE_BIG 15
#define SHAKE_DECEL 10

#define BADDIE_SPAWN_INIT_DELAY 1
#define BADDIE_SPAWN_DELAY_REDUCE 0.005
#define BADDIE_SPAWN_MIN_DELAY 0.25
#define BADDIE_SPAWN_INCREASE 0.3

static Texture2D player_texture;
static Texture2D baddie_texture;
static Texture2D bullet_texture;
static Texture2D coin_texture;
static Texture2D boom_texture;
static Texture2D bg_texture;
static World world = {.width=WIDTH, .height=HEIGHT};
static Player player = {0};
static Baddie baddies[BADDIE_N] = {0};
static DeadBaddie dead_baddies[BADDIE_N] = {0};
static Bullet bullets[BULLET_N] = {0};
static Boom booms[BOOM_N] = {0};
static Coin coin = {0};
static bool game_start;
static bool game_over;
static float red_alpha;
static float baddie_spawn_timer;
static float baddie_spawn_delay;
static float baddies_to_spawn;
static int score;
static char score_text[4];
static char top_text[100];
static float shake_amount;
static Vector2 cam_pos;

static void init();

static void update();

static void draw();

static void unload();

static void scaled_draw(Texture2D texture, Rectangle src_rect, Vector2 pos, float angle, float scale);

static bool is_touching(Vector2 a, Vector2 b);

void draw_text(const char *text, int x, int y);

int main(void) {
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(WIDTH * SCALE, HEIGHT * SCALE, "Gun Room");
    SetWindowMinSize(WIDTH, HEIGHT);

    player_texture = LoadTexture("content/sprites/gunner.png");
    baddie_texture = LoadTexture("content/sprites/baddie.png");
    bullet_texture = LoadTexture("content/sprites/bullet.png");
    coin_texture = LoadTexture("content/sprites/coin.png");
    boom_texture = LoadTexture("content/sprites/boom.png");
    bg_texture = LoadTexture("content/sprites/grid.png");

    init();
    while (!WindowShouldClose()) {
        if (!game_over)
            update();
        else if (red_alpha > 0)
            red_alpha -= RED_ALPHA_SPEED * GetFrameTime();

        if (IsKeyPressed(KEY_R))
            init();

        sprintf(score_text, "%d", score);

        shake_amount = approach(shake_amount, 0, SHAKE_DECEL * GetFrameTime());
        float angle = (rand() % 360) * DEG2RAD;
        cam_pos = Vector2Rotate((Vector2) {shake_amount}, angle);

        draw();
    }
    unload();

    CloseWindow();
    return 0;
}

void init() {
    game_start = false;
    game_over = false;
    score = 0;
    red_alpha = 1;
    shake_amount = 0;
    cam_pos = Vector2Zero();

    baddie_spawn_timer = 0;
    baddie_spawn_delay = BADDIE_SPAWN_INIT_DELAY;
    baddies_to_spawn = 1;

    player.world = &world;
    player.texture = &player_texture;
    player.pos = (Vector2) {WIDTH / 2, HEIGHT / 2};
    player.vel = Vector2Zero();
    player.scale = 1;
    player.is_shooting = false;
    player.shoot_timer = 0;

    for (int i = 0; i < BADDIE_N; i++) {
        baddies[i].world = &world;
        baddies[i].texture = &baddie_texture;
        baddies[i].active = false;
    }

    for (int i = 0; i < BADDIE_N; i++) {
        dead_baddies[i].world = &world;
        dead_baddies[i].texture = &baddie_texture;
        dead_baddies[i].active = false;
    }

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
    update_baddies(baddies, player.pos, dt);
    update_bullets(bullets, dt);
    update_coin(&coin, dt);
    update_booms(booms, dt);
    update_dead_baddies(dead_baddies, dt);

    if (player.is_shooting)
        add_bullet(bullets, player.pos, player.angle);

    if (is_touching(player.pos, coin.pos)) {
        move_coin(&coin, player.pos);
        add_boom(booms, coin.pos);
        game_start = true;
        score++;
        player_get_coin(&player);
        shake_amount = SHAKE_SMALL;

        for (int i = 0; i < (int) baddies_to_spawn; i++)
            add_baddie(baddies, player.pos);
        baddies_to_spawn += BADDIE_SPAWN_INCREASE;
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
                add_dead_baddie(dead_baddies, baddie->pos, player.pos);
                shake_amount = SHAKE_SMALL;
            }
        }
    }

    for (int i = 0; i < BADDIE_N; i++) {
        Baddie *b = &baddies[i];
        if (b->active && is_touching(b->pos, player.pos)) {
            game_over = true;
            player.frame = 2;
            player.scale = 1;
            shake_amount = SHAKE_BIG;
            return;
        }
    }

    baddie_spawn_timer -= dt;
    if (game_start && baddie_spawn_timer < 0) {
        baddie_spawn_delay = fmaxf(BADDIE_SPAWN_MIN_DELAY, baddie_spawn_delay - BADDIE_SPAWN_DELAY_REDUCE);
        baddie_spawn_timer = baddie_spawn_delay;
        add_baddie(baddies, player.pos);
    }
}


void draw() {
    BeginDrawing();
    {
        ClearBackground(BLACK);

        float over = 32;
        Rectangle screen_rect = {-over + cam_pos.x, -over + cam_pos.y, (WIDTH + over) * SCALE, (HEIGHT + over) * SCALE};
        Rectangle grid_rect = {0, 0, 32, 32};
        DrawTextureTiled(bg_texture, grid_rect, screen_rect, (Vector2) {0, 0}, 0, SCALE, WHITE);

        draw_dead_baddies(dead_baddies, scaled_draw);
        draw_coin(&coin, scaled_draw);
        draw_baddies(baddies, scaled_draw);
        draw_bullets(bullets, scaled_draw);
        draw_player(&player, scaled_draw);
        draw_booms(booms, scaled_draw);

        // Dark bars.
        if (!game_start || game_over) {
            Color col = BLACK;
            col.a = 150;
            int bar_height = FONT_SIZE + 16;
            DrawRectangle(0, 0, GetScreenWidth(), bar_height, col);
            DrawRectangle(0, GetScreenHeight() - bar_height, GetScreenWidth(), bar_height, col);
        }

        if (!game_start) {
            char *top_text = "Gun Room";
            draw_text(top_text, GetScreenWidth() / 2 - MeasureText(top_text, FONT_SIZE) / 2, 8);
            char *bottom_text = "Arrows to move, Z to shoot";
            draw_text(
                    bottom_text,
                    GetScreenWidth() / 2 - MeasureText(bottom_text, FONT_SIZE) / 2,
                    GetScreenHeight() - FONT_SIZE - 8
            );
        }

        if (game_start && !game_over) {
            draw_text(score_text, 8, 8);
        }

        if (game_over) {
            sprintf(top_text, "you scored %d!", score);
            draw_text(top_text, GetScreenWidth() / 2 - MeasureText(top_text, FONT_SIZE) / 2, 8);
            char *bottom_text = "press R to restart";
            draw_text(
                    bottom_text,
                    GetScreenWidth() / 2 - MeasureText(bottom_text, FONT_SIZE) / 2,
                    GetScreenHeight() - FONT_SIZE - 8
            );
        }

        // Red fade.
        if (game_over && red_alpha > 0) {
            Color red = RED;
            red.a = (int) (red_alpha * 255);
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), red);
        }
    }
    EndDrawing();
}

void draw_text(const char *text, int x, int y) {
    DrawText(text, x, y + SCALE * 2, FONT_SIZE, BLACK);
    DrawText(text, x, y, FONT_SIZE, WHITE);
}

void unload() {
    UnloadTexture(player_texture);
    UnloadTexture(baddie_texture);
    UnloadTexture(bullet_texture);
    UnloadTexture(coin_texture);
    UnloadTexture(boom_texture);
    UnloadTexture(bg_texture);
}

void scaled_draw(Texture2D texture, Rectangle src_rect, Vector2 pos, float angle, float scale) {
    Rectangle dest_rect = {pos.x, pos.y, src_rect.width * scale, src_rect.height * scale};
    Vector2 middle = {dest_rect.width / 2, dest_rect.height / 2};

    middle.x *= SCALE;
    middle.y *= SCALE;
    dest_rect.x *= SCALE;
    dest_rect.y *= SCALE;
    dest_rect.width *= SCALE;
    dest_rect.height *= SCALE;

    dest_rect.x += cam_pos.x;
    dest_rect.y += cam_pos.y;

    DrawTexturePro(texture, src_rect, dest_rect, middle, angle * RAD2DEG, WHITE);
}

bool is_touching(Vector2 a, Vector2 b) {
    return Vector2LengthSqr(Vector2Subtract(a, b)) < TOUCHING_DIST * TOUCHING_DIST;
}
