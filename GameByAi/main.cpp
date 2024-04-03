#include <iostream>
#include <vector>
#include <string>
#include <SDL2/SDL.h>

using namespace std;

// Screen size
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Player position
int playerX = SCREEN_WIDTH / 2;
int playerY = SCREEN_HEIGHT / 2;

// NPC position
int npcX = rand() % SCREEN_WIDTH;
int npcY = rand() % SCREEN_HEIGHT;

// Zombie position
int zombieX = rand() % SCREEN_WIDTH;
int zombieY = rand() % SCREEN_HEIGHT;

// Game board
vector<vector<char>> gameBoard(SCREEN_HEIGHT, vector<char>(SCREEN_WIDTH, ' '));

// Function to draw the game board
void drawGameBoard(SDL_Renderer* renderer) {
    // Clear the screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Draw the boundaries of the maze
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        SDL_RenderDrawLine(renderer, 0, i, SCREEN_WIDTH, i);
    }
    for (int j = 0; j < SCREEN_WIDTH; j++) {
        SDL_RenderDrawLine(renderer, j, 0, j, SCREEN_HEIGHT);
    }

    // Draw the player, NPC, and zombie
    SDL_Rect playerRect = { playerX, playerY, 10, 10 };
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &playerRect);

    SDL_Rect npcRect = { npcX, npcY, 10, 10 };
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &npcRect);

    SDL_Rect zombieRect = { zombieX, zombieY, 10, 10 };
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderFillRect(renderer, &zombieRect);

    // Present the rendered frame
    SDL_RenderPresent(renderer);
}

// Function to move the player
void movePlayer(char direction) {
    switch (direction) {
    case 'w':
        if (playerY > 0) {
            playerY--;
        }
        break;
    case 's':
        if (playerY < SCREEN_HEIGHT - 1) {
            playerY++;
        }
        break;
    case 'a':
        if (playerX > 0) {
            playerX--;
        }
        break;
    case 'd':
        if (playerX < SCREEN_WIDTH - 1) {
            playerX++;
        }
        break;
    }
}

// Function to move the NPC
void moveNPC() {
    // Calculate the distance between the NPC and the player
    int dx = playerX - npcX;
    int dy = playerY - npcY;

    // Move the NPC towards the player
    if (abs(dx) > abs(dy)) {
        if (dx > 0) {
            npcX++;
        }
        else {
            npcX--;
        }
    }
    else {
        if (dy > 0) {
            npcY++;
        }
        else {
            npcY--;
        }
    }
}

// Function to move the zombie
void moveZombie() {
    // Calculate the distance between the zombie and the NPC
    int dx = npcX - zombieX;
    int dy = npcY - zombieY;

    // Move the zombie towards the NPC
    if (abs(dx) > abs(dy)) {
        if (dx > 0) {
            zombieX++;
        }