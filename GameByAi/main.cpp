#include <iostream>
#include <vector>
#include "../include/SDL.h"
#include <Windows.h>
#include <ctime> // Для использования функции time()
#define main SDL_main
#include <random>

// Игрок
struct Player {
    SDL_FRect rect;
    int hp;
    int hurtResistance = 0;
    bool wasHurt = false;
    bool isAlive;
};

// НПС
struct NPC {
    SDL_FRect rect;
    int hp;
    bool isAlive;
};

// Зомби
struct Zombie {
    SDL_FRect rect;
    int hp;
    bool isAlive;
    SDL_FRect targetRect;  // Цель зомби (игрок или НПС)
};


// Карта
struct Map {
    std::vector<SDL_FRect> obstacles; // Препятствия на карте
    SDL_Rect bounds; // Границы карты
};

float randomFloat(const float min, const float max)
{
    std::random_device rdd;
    std::mt19937 rnd(rdd());
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(rnd);
}
int randomInt(const int min, const int max)
{
    std::random_device rdd;
    std::mt19937 rnd(rdd());
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(rnd);
}

// Глобальные переменные
SDL_Window* window;
SDL_Renderer* renderer;
Player player;
//NPC npc;
std::vector<Zombie> zombies;
Map gameMap; // Карта игры

// Генерация случайной позиции в пределах границ карты
SDL_FRect generateRandomPosition() {
    float x = rand() % (gameMap.bounds.w - 32); // Ширина карты минус ширина персонажа
    float y = rand() % (gameMap.bounds.h - 32); // Высота карты минус высота персонажа
    return { x, y, 10, 10 }; // Возвращаем прямоугольник для позиции персонажа
}
// Инициализация
bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Не удалось инициализировать SDL: %s", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow("2D Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 810, 710, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        SDL_Log("Не удалось создать окно: %s", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer == nullptr) {
        SDL_Log("Не удалось создать рендерер: %s", SDL_GetError());
        return false;
    }

    // Инициализация игрока, НПС и зомби
    player.rect = { 25, 25, 10, 10 };
    player.hp = 100;
    player.isAlive = true;

    // Создание карты
    gameMap.bounds = { 0, 0, 800, 600 };

    for (int i = 0; i < randomInt(2, 10); i++) {
        SDL_FRect obstacle = { randomInt(10, 600), randomInt(10, 400), randomInt(30, 100),randomInt(30,100) };
        gameMap.obstacles.push_back(obstacle);
    }

    // Создание зомби
    srand(time(nullptr)); // Инициализация генератора случайных чисел
    for (int i = 0; i < randomInt(1,5); ++i) {
        Zombie zombie;
        zombie.rect = generateRandomPosition();
        zombie.hp = 100;
        zombie.isAlive = true;
        zombie.targetRect = player.rect;
        zombies.push_back(zombie);
    }

    // Спаун игрока в случайной позиции
    player.rect = generateRandomPosition();

    return true;
}



// Обработка ввода
void handleInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            exit(0);
        }
        else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.scancode) {
            case SDL_SCANCODE_W:
                player.rect.y -= 10;
                break;
            case SDL_SCANCODE_A:
                player.rect.x -= 10;
                break;
            case SDL_SCANCODE_S:
                player.rect.y += 10;
                break;
            case SDL_SCANCODE_D:
                player.rect.x += 10;
                break;
            }

           
        }
    }
}

// Проверка столкновений с препятствиями
bool checkCollision(const SDL_FRect& rect) {
    for (const auto& obstacle : gameMap.obstacles) {
        if (SDL_HasIntersectionF(&rect, &obstacle)) {
            return true; // Обнаружено столкновение
        }
    }
    return false; // Нет столкновений
}

// Обновление игры
// Обновление игры
void update() {
    // Обновление зомби
    for (Zombie& zombie : zombies) {
        if (zombie.isAlive) {
            // Зомби следует за целью (игроком или НПС)
            zombie.targetRect = player.rect;

            int dx = zombie.targetRect.x - zombie.rect.x;
            int dy = zombie.targetRect.y - zombie.rect.y;

            if (dx < 0) {
                zombie.rect.x -= randomFloat(0.01, 0.6);
            }
            if (dx > 0) {
                zombie.rect.x += randomFloat(0.01,0.6);
            }

            if (dy < 0) {
                zombie.rect.y -= randomFloat(0.01, 0.6);
            }
            if (dy > 0) {
                zombie.rect.y += randomFloat(0.01, 0.6);
            }

            if (abs(dx) < 10 && abs(dy) < 10 && !player.wasHurt) {
                player.wasHurt = true;
                player.hurtResistance = 400;
                player.hp -= 10;
            }
            // Проверка столкновений с препятствиями
            if (checkCollision(zombie.rect)) {
                // Если обнаружено столкновение, отменяем перемещение зомби
                zombie.rect.x -= 1;
                zombie.rect.y -= 1;
            }
        }
    }
    
    if (player.wasHurt)
        player.hurtResistance -= 1;

    if (player.hurtResistance <= 0)
        player.wasHurt = false;

    // Проверка столкновений игрока с препятствиями
    if (checkCollision(player.rect)) {
        // Если обнаружено столкновение, отменяем перемещение игрока
        player.rect.x -= 1;
        player.rect.y -= 1;
    }

    if (player.hp <= 0)
        player.isAlive = false;
}

// Отрисовка игры
void render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Отрисовка карты
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &gameMap.bounds);
    for (const auto& obstacle : gameMap.obstacles) {
        SDL_RenderFillRectF(renderer, &obstacle);
    }

    // Отрисовка игрока и зомби
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRectF(renderer, &player.rect);

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    for (const auto& zombie : zombies) {
        SDL_RenderFillRectF(renderer, &zombie.rect);
    }

    SDL_RenderPresent(renderer);
}

void cleanup() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char** argv) {
    if (!init()) {
        return -1;
    }

    // Основной игровой цикл
    bool running = true;
    while (running) {
        handleInput();
        update();   
        render();
        std::cout << player.hp << "\n";
        // Проверка на окончание игры
        if (!player.isAlive) {
            Sleep(1000);
            running = false;
        }
    }

    cleanup();
    std::cout << player.hp << "Игра окончена, к сожалению вы пройграли\n";
    Sleep(10000);
    return 0;
}