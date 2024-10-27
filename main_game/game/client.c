#include "/opt/homebrew/include/SDL2/SDL.h"
#include "/opt/homebrew/include/SDL2/SDL_image.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "../include/common.h"
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

Player player;
int sockfd;
SDL_Renderer* renderer;

// Function to load texture
SDL_Texture* load_texture(SDL_Renderer* renderer, const char* file) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, file);
    if (!texture) {
        printf("Failed to load texture %s: %s\n", file, IMG_GetError());
    }
    return texture;
}

// Function to initialize player's character based on chosen type
void init_player(Player* player, CharacterType type) {
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
    player->position.x = 100;  // Initial X position
    player->position.y = 100;  // Initial Y position
    player->position.w = 50;   // Player width
    player->position.h = 50;   // Player height
    player->shots_received = 0;
    player->type = type;
}

// Function to handle client-server communication setup
void setup_networking() {
    struct sockaddr_in server_addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection to server failed");
        exit(EXIT_FAILURE);
    }
}

// Function to send player updates to server
void send_player_update(Player* player) {
    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "%d %d %d", player->position.x, player->position.y, player->shots_received);
    send(sockfd, buffer, strlen(buffer), 0);
}

// Function to handle player movement
void handle_input(SDL_Event* event) {
    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    if (keystate[SDL_SCANCODE_W] || keystate[SDL_SCANCODE_UP]) {
        player.position.y -= 5;
    }
    if (keystate[SDL_SCANCODE_S] || keystate[SDL_SCANCODE_DOWN]) {
        player.position.y += 5;
    }
    if (keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_LEFT]) {
        player.position.x -= 5;
    }
    if (keystate[SDL_SCANCODE_D] || keystate[SDL_SCANCODE_RIGHT]) {
        player.position.x += 5;
    }
    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) {
        // Send "shoot" message to the server
        char buffer[BUFFER_SIZE] = "shoot";
        send(sockfd, buffer, strlen(buffer), 0);
    }
}

// Function to render player and other objects
void render() {
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, player.texture, NULL, &player.position);
    SDL_RenderPresent(renderer);
}

// Function to cleanup resources
void cleanup() {
    close(sockfd);
    SDL_DestroyTexture(player.texture);
    SDL_DestroyRenderer(renderer);
    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <character_type>\n", argv[0]);
        printf("Character types: zombie, pumpkin, witch, ghost\n");
        return 1;
    }

    CharacterType character_type;
    if (strcmp(argv[1], "zombie") == 0) {
        character_type = ZOMBIE;
    } else if (strcmp(argv[1], "pumpkin") == 0) {
        character_type = PUMPKIN;
    } else if (strcmp(argv[1], "witch") == 0) {
        character_type = WITCH;
    } else {
        character_type = GHOST;
    }

    // Initialize SDL and create window/renderer
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    SDL_Window* window = SDL_CreateWindow("Halloween Game Client", 
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Initialize player character and network connection
    init_player(&player, character_type);
    setup_networking();

    int running = 1;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else {
                handle_input(&event);
            }
        }

        // Send player's updated position to server and render
        send_player_update(&player);
        render();
    }

    cleanup();
    SDL_DestroyWindow(window);
    return 0;
}
