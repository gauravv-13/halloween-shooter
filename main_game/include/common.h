#ifndef COMMON_H
#define COMMON_H

#include "/opt/homebrew/include/SDL2/SDL.h"
#include "/opt/homebrew/include/SDL2/SDL_image.h"

#define MAX_PLAYERS 4

// Enumeration for different character types
typedef enum {
    GHOST,
    ZOMBIE,
    PUMPKIN,
    WITCH
} CharacterType;

// Structure representing a player
typedef struct {
    char name[50];            // Player's name
    SDL_Texture* texture;     // Texture for the player's character
    SDL_Rect position;        // Position of the character in the window
    int shots_received;       // Number of shots received by the character
    CharacterType type;       // Type of character (e.g., Ghost, Zombie, etc.)
} Player;

// Function to load textures given a file path (declaration only)
SDL_Texture* load_texture(SDL_Renderer* renderer, const char* file);

#endif // COMMON_H
