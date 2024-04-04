#include <SDL.h>
#include <vector>

// Игрок
struct Player {
    SDL_Rect rect;
    int hp;
    bool isAlive;
};

// НПС
struct NPC {
    SDL_Rect rect;
    int hp;
    bool isAlive;
};

// Зомби
struct Zombie {
    SDL_Rect rect;
    int hp;
    bool isAlive;
    SDL_Rect targetRect;  // Цель зомби (игрок или НПС)
};

// Глобальные переменные
SDL_Window* window;
SDL_Renderer* renderer;
Player player;
NPC npc;
std::vector<Zombie> zombies;

// Инициализация
bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Не удалось инициализировать SDL: %s", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow("2D Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
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
    player.rect = { 100, 100, 32, 32 };
    player.hp = 100;
    player.isAlive = true;

    npc.rect = { 200, 200, 32, 32 };
    npc.hp = 100;
    npc.isAlive = true;

    Zombie zombie;
    zombie.rect = { 400, 400, 32, 32 };
    zombie.hp = 100;
    zombie.isAlive = true;
    zombie.targetRect = player.rect;
    zombies.push_back(zombie);

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
                player.rect.y -= 5;
                break;
            case SDL_SCANCODE_A:
                player.rect.x -= 5;
                break;
            case SDL_SCANCODE_S:
                player.rect.y += 5;
                break;
            case SDL_SCANCODE_D:
                player.rect.x += 5;
                break;
            }
        }
    }
}

// Обновление игры
void update() {
    // Обновление НПС
    

    // Обновление зомби
    for (Zombie& zombie : zombies) {
        if (npc.isAlive) {
            // НПС убегает от зомби
            int dx = npc.rect.x - zombie.targetRect.x;
            int dy = npc.rect.y - zombie.targetRect.y;
            npc.rect.x -= dx / 10;
            npc.rect.y -= dy / 10;
        }

        if (zombie.isAlive) {
            // Зомби следует за целью (игроком или НПС)
            int dx = zombie.targetRect.x - zombie.rect.x;
            int dy = zombie.targetRect.y - zombie.rect.y;
            zombie.rect.x += dx / 10;
            zombie.rect.y += dy / 10;

            // Зомби наносит урон игроку или НПС
            if (SDL_HasIntersection(&zombie.rect, &player.rect)) {
                player.hp -= 10;
                if (player.hp <= 0) {
                    player.isAlive = false;
                }
            }
            else if (SDL_HasIntersection(&zombie.rect, &npc.rect)) {
                npc.hp -= 10;
                if (npc.hp <= 0) {
                    npc.isAlive = false;
                }
            }
        }
    }

    // Проверка на столкновение зомби со стенами
    for (Zombie& zombie : zombies) {
        if (zombie.rect.x < 0 || zombie.rect.x > 800 - zombie.rect.w) {
            zombie.rect.x = 800 - zombie.rect.w - zombie.rect.x;
        }
        if (zombie.rect.y < 0 || zombie.rect.y > 600 - zombie.rect.h) {
            zombie.rect.y = 600 - zombie.rect.h - zombie.rect.y;
        }
    }

    // Выбор новой цели для зомби
    for (Zombie& zombie : zombies) {
        if (!zombie.isAlive) continue;

        // Выбираем цель с меньшим расстоянием до зомби
        int minDistance = INT_MAX;
        if (player.isAlive) {
            int dx = zombie.rect.x - player.rect.x;
            int dy = zombie.rect.y - player.rect.y;
            int distance = dx * dx + dy * dy;
            if (distance < minDistance) {
                minDistance = distance;
                zombie.targetRect = player.rect;
            }
        }
        if (npc.isAlive) {
            int dx = zombie.rect.x - npc.rect.x;
            int dy = zombie.rect.y - npc.rect.y;
            int distance = dx * dx + dy * dy;
            if (distance < minDistance) {
                minDistance = distance;
                zombie.targetRect = npc.rect;
            }
        }
    }
}

// Отрисовка игры
void render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Отрисовка игрока, НПС и зомби
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &player.rect);

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &npc.rect);

    for (Zombie& zombie : zombies) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_RenderFillRect(renderer, &zombie.rect);
    }

    SDL_RenderPresent(renderer);
}

// Освобождение ресурсов
void cleanup() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

// Точка входа
int main(int argv, char** args) {
    if (!init()) {
        return -1;
    }

    // Основной игровой цикл
    bool running = true;
    while (running) {
        handleInput();
        update();
        render();

        // Проверка на окончание игры
        if (!player.isAlive) {
            //running = false;
        }
        if (!npc.isAlive) {
            //running = false;
        }
        if (zombies.empty()) {
            //running = false;
        }
    }

    cleanup();
    return 0;
}