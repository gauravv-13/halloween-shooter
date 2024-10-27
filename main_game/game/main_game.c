#include "/opt/homebrew/include/SDL2/SDL.h"
#include "/opt/homebrew/include/SDL2/SDL_image.h"
#include <stdio.h>
#include <math.h>
#include "../include/common.h"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define BULLET_SPEED 10

typedef struct {
    SDL_Texture* texture;
    SDL_Rect position;
    double angle;  // Angle for aiming
    int active;
} Bullet;

Player players[MAX_PLAYERS];
Bullet bullets[MAX_PLAYERS];
SDL_Renderer* renderer;

// Load texture helper function
SDL_Texture* load_texture(SDL_Renderer* renderer, const char* file) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, file);
    if (!texture) {
        printf("Failed to load texture %s: %s\n", file, IMG_GetError());
    }
    return texture;
}

// Initialize a player with a given character type and initial position
void init_player(Player* player, CharacterType type, int x, int y) {
    switch (type) {
        case ZOMBIE:
            player->texture = load_texture(renderer, "resources/zombie.png");
            break;
        case PUMPKIN:
            player->texture = load_texture(renderer, "resources/pumpkin.png");
            break;
        case WITCH:
            player->texture = load_texture(renderer, "resources/witch.png");
            break;
        case GHOST:
        default:
            player->texture = load_texture(renderer, "resources/ghost.png");
            break;
    }
    player->position.x = x;
    player->position.y = y;
    player->position.w = 50;
    player->position.h = 50;
    player->shots_received = 0;
    player->type = type;
}

// Initialize bullet attributes
void init_bullet(Bullet* bullet) {
    bullet->texture = load_texture(renderer, "resources/fireball.png");
    bullet->position.w = 20;
    bullet->position.h = 20;
    bullet->active = 0;
}

// Fire a bullet in the direction the player is aiming
void fire_bullet(Player* player, Bullet* bullet, double angle) {
    bullet->position.x = player->position.x + player->position.w / 2;
    bullet->position.y = player->position.y + player->position.h / 2;
    bullet->angle = angle;
    bullet->active = 1;
}

// Update bullet position
void update_bullet(Bullet* bullet) {
    if (bullet->active) {
        bullet->position.x += BULLET_SPEED * cos(bullet->angle);
        bullet->position.y += BULLET_SPEED * sin(bullet->angle);
        if (bullet->position.x < 0 || bullet->position.x > WINDOW_WIDTH ||
            bullet->position.y < 0 || bullet->position.y > WINDOW_HEIGHT) {
            bullet->active = 0;
        }
    }
}

// Render players and bullets
void render_game() {
    SDL_RenderClear(renderer);
    
    for (int i = 0; i < MAX_PLAYERS; i++) {
        SDL_RenderCopyEx(renderer, players[i].texture, NULL, &players[i].position, 0, NULL, SDL_FLIP_NONE);
        if (bullets[i].active) {
            SDL_RenderCopyEx(renderer, bullets[i].texture, NULL, &bullets[i].position, bullets[i].angle * 180 / M_PI, NULL, SDL_FLIP_NONE);
        }
    }
    
    SDL_RenderPresent(renderer);
}

// Handle player movement and aiming
void handle_input(SDL_Event* event, Player* player, Bullet* bullet) {
    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    if (keystate[SDL_SCANCODE_W] || keystate[SDL_SCANCODE_UP]) {
        player->position.y -= 5;
    }
    if (keystate[SDL_SCANCODE_S] || keystate[SDL_SCANCODE_DOWN]) {
        player->position.y += 5;
    }
    if (keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_LEFT]) {
        player->position.x -= 5;
    }
    if (keystate[SDL_SCANCODE_D] || keystate[SDL_SCANCODE_RIGHT]) {
        player->position.x += 5;
    }

    // Aiming and shooting with mouse
    if (event->type == SDL_MOUSEMOTION) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        double deltaX = mouseX - (player->position.x + player->position.w / 2);
        double deltaY = mouseY - (player->position.y + player->position.h / 2);
        bullet->angle = atan2(deltaY, deltaX);
    }
    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) {
        if (!bullet->active) {
            fire_bullet(player, bullet, bullet->angle);
        }
    }
}

// Cleanup resources
void cleanup() {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        SDL_DestroyTexture(players[i].texture);
        SDL_DestroyTexture(bullets[i].texture);
    }
    SDL_DestroyRenderer(renderer);
    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    SDL_Window* window = SDL_CreateWindow("Halloween Game - Multiplayer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Initialize players with different character types
    init_player(&players[0], ZOMBIE, 100, 100);
    init_player(&players[1], PUMPKIN, 300, 100);
    init_player(&players[2], WITCH, 100, 300);
    init_player(&players[3], GHOST, 300, 300);

    // Initialize bullets
    for (int i = 0; i < MAX_PLAYERS; i++) {
        init_bullet(&bullets[i]);
    }

    int running = 1;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else {
                handle_input(&event, &players[0], &bullets[0]); // Player 1 controlled by local input
            }
        }

        // Update bullet positions
        for (int i = 0; i < MAX_PLAYERS; i++) {
            update_bullet(&bullets[i]);
        }

        render_game();
    }

    cleanup();
    SDL_DestroyWindow(window);
    return 0;
}
