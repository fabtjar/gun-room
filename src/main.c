#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "raylib.h"
#include "raymath.h"

#define WIDTH 400
#define HEIGHT 300
#define SCALE 2

#define PLAYER_SPEED 150
#define PLAYER_DECEL 100
#define BADDIE_N 100
#define BADDIE_SPEED 80
#define PLAYER_DECEL 100
#define BULLET_N 20
#define BULLET_SPEED 500
#define SHOOT_DELAY 0.1
#define COIN_MIN_DIST 100
#define COIN_EDGE_GAP 50
#define TOUCHING_DIST 20

#define DEBUG_TEXT_N 1000

typedef struct World {
    int width;
    int height;
} World;

typedef struct Player {
    World *world;
    Vector2 pos;
    Vector2 vel;
    float angle;
    bool is_shooting;
    double shoot_timer;
    int frame;
} Player;

typedef struct Baddie {
    World *world;
    Vector2 pos;
    Vector2 vel;
    bool active;
} Baddie;

typedef struct Bullet {
    World *world;
    Vector2 pos;
    Vector2 vel;
    bool active;
} Bullet;

void update_player(Player *player, float dt);

Baddie *add_baddie(Baddie baddie[], Vector2 pos);

void update_baddies(Baddie baddies[], float dt);

Bullet *add_bullet(Bullet bullets[], Vector2 pos, float angle);

void update_bullets(Bullet bullets[], float dt);

void move_coin(Vector2 *coin_pos, Vector2 player_pos);

bool is_touching(Vector2 a, Vector2 b);

float approach(float t, float target, float delta);

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

        int baddies_active = 0;
        for (int i = 0; i < BADDIE_N; i++) {
            if (baddies[i].active) baddies_active++;
        }
        sprintf(debug_text, "%d\n%d", (int) player.pos.x, (int) player.pos.y);

        BeginTextureMode(renderTexture);
        {
            ClearBackground(WHITE);
            DrawTextureTiled(bg_texture, grid_rect, game_rect, (Vector2) {0, 0}, 0, 1, WHITE);

            DrawTexture(coin_texture, coin_pos.x - 12, coin_pos.y - 12, WHITE);

            for (int i = 0; i < BADDIE_N; i++) {
                Baddie b = baddies[i];
                if (!b.active) continue;
                src_rect = (Rectangle) {0, 0, 32, 32};
                dest_rect = (Rectangle) {b.pos.x, b.pos.y, 32, 32};
                float angle = Vector2Angle(Vector2Zero(), b.vel);
                DrawTexturePro(baddie_texture, src_rect, dest_rect, grid_middle, angle * RAD2DEG, WHITE);
            }

            for (int i = 0; i < BULLET_N; i++) {
                Bullet b = bullets[i];
                if (b.active) DrawTexture(bullet_texture, b.pos.x - 10, b.pos.y - 10, WHITE);
            }

            src_rect = (Rectangle) {player.frame * 32, 0, 32, 32};
            dest_rect = (Rectangle) {player.pos.x, player.pos.y, 32, 32};
            DrawTexturePro(player_texture, src_rect, dest_rect, grid_middle, player.angle * RAD2DEG, WHITE);

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
        if (!player->is_shooting) player->vel = Vector2Scale(Vector2Normalize(dir), PLAYER_SPEED);
        player->angle = Vector2Angle(Vector2Zero(), dir);
    }

    player->pos.x += player->vel.x * dt;
    player->pos.y += player->vel.y * dt;

    if (player->pos.x < 0) player->pos.x = 0;
    else if (player->pos.x > WIDTH) player->pos.x = WIDTH;
    if (player->pos.y < 0) player->pos.y = 0;
    else if (player->pos.y > HEIGHT) player->pos.y = HEIGHT;

    player->frame = player->is_shooting ? 1 : 0;
}

Baddie *add_baddie(Baddie baddie[], Vector2 pos) {
    for (int i = 0; i < BADDIE_N; i++) {
        Baddie *b = &baddie[i];
        if (b->active) continue;
        b->active = true;
        b->pos = pos;
        Vector2 middle = (Vector2) {b->world->width / 2, b->world->height / 2};
        Vector2 dist = Vector2Subtract(middle, b->pos);
        Vector2 normal = Vector2Normalize(dist);
        b->vel = Vector2Scale(normal, BADDIE_SPEED);
        return b;
    }
    return NULL;
}

void update_baddies(Baddie baddies[], float dt) {
    for (int i = 0; i < BADDIE_N; i++) {
        Baddie *b = &baddies[i];
        if (!b->active) continue;
        b->pos = Vector2Add(b->pos, Vector2Scale(b->vel, dt));
        if (b->pos.x < 0) {
            b->pos.x = 0;
            b->vel.x *= -1;
        } else if (b->pos.x > (float) b->world->width) {
            b->pos.x = (float) b->world->width;
            b->vel.x *= -1;
        }
        if (b->pos.y < 0) {
            b->pos.y = 0;
            b->vel.y *= -1;
        } else if (b->pos.y > (float) b->world->height) {
            b->pos.y = (float) b->world->height;
            b->vel.y *= -1;
        }
    }
}

Bullet *add_bullet(Bullet bullets[], Vector2 pos, float angle) {
    for (int i = 0; i < BULLET_N; i++) {
        Bullet *b = &bullets[i];
        if (b->active) continue;
        b->active = true;
        b->pos = pos;
        b->vel = Vector2Rotate((Vector2) {BULLET_SPEED, 0}, angle);
        return b;
    }
    return NULL;
}

void update_bullets(Bullet bullets[], float dt) {
    for (int i = 0; i < BULLET_N; i++) {
        Bullet *b = &bullets[i];
        if (!b->active) continue;
        b->pos = Vector2Add(b->pos, Vector2Scale(b->vel, dt));
        if (b->pos.x + 10 < 0 || b->pos.x - 10 > (float) b->world->width ||
            b->pos.y + 10 < 0 || b->pos.y - 10 > (float) b->world->height)
            b->active = false;
    }
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


float approach(float t, float target, float delta) {
    return t < target ? fminf(t + delta, target) : fmaxf(t - delta, target);
}
