//############################################################################################
// DO NAPISANIA PROJEKTU ZOSTA£Y U¯YTE MATERIA£Y PODES£ANE PRZEZ PROFESORA DERENIOWSKIEGO,
// TAKIE JAK PRZYK£ADOWY KOD, KTÓRY WYKORZYSTUJE BIBLIOTEKI SDL2 JAK I BITMAPA Z ALFABETEM
//############################################################################################

#define _CRT_SECURE_NO_WARNINGS
#include <SDL.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>

// WINDOW SETTINGS
#define SCREEN_WIDTH 640        // window width in px
#define SCREEN_HEIGHT 480    // window height in px

// GAME AREA SETTINGS
#define GAME_AREA_WIDTH 300     // game area width in px
#define GAME_AREA_HEIGHT 460    // game area height in px
#define GAME_AREA_X 10
#define GAME_AREA_Y 10

// STATS AREA SETTINGS
#define STATS_AREA_WIDTH 200                                // stats area width in px
#define STATS_AREA_HEIGHT 100                                   // stats area height in px
#define STATS_AREA_X GAME_AREA_X + GAME_AREA_WIDTH + 10
#define STATS_AREA_Y GAME_AREA_Y

// PROGGRES BAR
#define PROGRERS_BAR_HEIGHT 10
#define PROGRES_BAR_WIDTH STATS_AREA_WIDTH

// SNAKE
#define SNAKE_LENGTH 10             // snake length in parts
#define SNAKE_SPEED 100     // lower is faster
#define CHANGE_SPEED 10     
#define LONGER_SNAKE 1
#define SHORTER_SNAKE 2 * LONGER_SNAKE

// TIMER
#define FASTER_SNAKE 15                  // time in s
#define RED_BERRY_RANDOM_TIME 10         // time in s
#define RED_BERRY_TIME 7                        // time in s

// BEST SCORE
#define BEST_SCORES_MAX_COUNT 3


// COLORS
#define BLACK 0x00000000
#define WHITE 0xFFFFFFFF
#define GREEN 0xFF00FF00
#define RED 0xFFFF0000
#define BLUE 0xFF0000FF

// BMP FILES
#define CHARSET_PATH "./cs8x8.bmp"

// DATA FILES
#define GAME_DATA "./game_data.txt"


//------------------------------------------------------------------------
//--------------------------- DATA STRUCTURES ----------------------------
//------------------------------------------------------------------------



struct SNAKE_PARTS {
    int x, y;
};

struct SNAKE {
    SNAKE_PARTS* parts;
    int length;
    int direction;
    int longerSnake;
    int shorterSnake;
    int speed;
    int speedChange;
    int points;
    int ableToMove;
};

struct BLUEBERRY {
    int x, y;
};

struct REDBERRY {
    int x, y;
    int enabled;
};


struct TIMER {
    // game time
    int lastTimeUpdate;
    double gameTime;

    // snake time
    double snakeTime;

    int fasterSnakeTime;
    int lastSnakeTime;

    // red berry time

    double redBerryTime;

    int redBerryLastTime;
    int redBerryRandomTime;
    int redBerryDuration;
    int redBerryNextTime;
};




//------------------------------------------------------------------------
//---------------------------- DRAW FUNCTIONS ----------------------------
//------------------------------------------------------------------------


// draw single pixel on the screen
void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color) {
    int bpp = surface->format->BytesPerPixel;
    Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;
    *(Uint32*)p = color;
};


// draw lines on the screen
void DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color) {
    for (int i = 0; i < l; i++) {
        DrawPixel(screen, x, y, color);
        x += dx;
        y += dy;
    };
};


// draw rectangle on the screen
void DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k, Uint32 outlineColor, Uint32 fillColor) {
    int i;
    DrawLine(screen, x, y, k, 0, 1, outlineColor);
    DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
    DrawLine(screen, x, y, l, 1, 0, outlineColor);
    DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
    for (i = y + 1; i < y + k - 1; i++)
        DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
};

// draw letters on the screen
void DrawString(SDL_Surface* screen, int x, int y, const char* text, SDL_Surface* charset) {
    int px, py, c;
    SDL_Rect s, d;
    s.w = 8;
    s.h = 8;
    d.w = 8;
    d.h = 8;
    while (*text) {
        c = *text & 255;
        px = (c % 16) * 8;
        py = (c / 16) * 8;
        s.x = px;
        s.y = py;
        d.x = x;
        d.y = y;
        SDL_BlitSurface(charset, &s, screen, &d);
        x += 8;
        text++;
    };
};


void DrawGameArea(SDL_Surface* screen) {
    DrawRectangle(screen, GAME_AREA_X, GAME_AREA_Y, GAME_AREA_WIDTH, GAME_AREA_HEIGHT, WHITE, BLACK);
}


void DrawStats(SDL_Surface* screen, TIMER* timer, SDL_Surface* charset, SNAKE* snake) {
    DrawRectangle(screen, STATS_AREA_X, STATS_AREA_Y, STATS_AREA_WIDTH, STATS_AREA_HEIGHT, WHITE, BLACK);
    char statsText[32];
    sprintf(statsText, "Game time: %.1lf s", timer->gameTime);
    DrawString(screen, STATS_AREA_X + 5, STATS_AREA_Y + 5, statsText, charset);

    sprintf(statsText, "Snake length: %d", snake->length);
    DrawString(screen, STATS_AREA_X + 5, STATS_AREA_Y + 15, statsText, charset);

    sprintf(statsText, "Points: %d", snake->points);
    DrawString(screen, STATS_AREA_X + 5, STATS_AREA_Y + 25, statsText, charset);

    DrawString(screen, STATS_AREA_X + 5, STATS_AREA_Y + STATS_AREA_HEIGHT - 40, "Name: Maciej", charset);
    DrawString(screen, STATS_AREA_X + 5, STATS_AREA_Y + STATS_AREA_HEIGHT - 30, "Surname: Drywa", charset);
    DrawString(screen, STATS_AREA_X + 5, STATS_AREA_Y + STATS_AREA_HEIGHT - 20, "Album number: 203556", charset);
    DrawString(screen, STATS_AREA_X + 5, STATS_AREA_Y + STATS_AREA_HEIGHT - 10, "DONE: A, B, C, D, E", charset);
}



void DrawProgressBar(SDL_Surface* screen, TIMER* timer) {
    // border
    DrawRectangle(screen, STATS_AREA_X, STATS_AREA_Y + STATS_AREA_HEIGHT + 10, PROGRES_BAR_WIDTH, PROGRERS_BAR_HEIGHT, WHITE, BLACK);

    // progress bar percentage
    double progress = timer->redBerryTime / timer->redBerryDuration;
    int progressBarWidth = PROGRES_BAR_WIDTH - (int)(PROGRES_BAR_WIDTH * progress);

    // Draw the progress bar
    DrawRectangle(screen, STATS_AREA_X, STATS_AREA_Y + STATS_AREA_HEIGHT + 10, progressBarWidth, PROGRERS_BAR_HEIGHT, RED, RED);
}

void DrawSnakeHead(SDL_Surface* screen, SNAKE* snake) {
    DrawRectangle(screen, GAME_AREA_X + snake->parts[0].x * 10, GAME_AREA_Y + snake->parts[0].y * 10, 10, 10, RED, RED);
    if (snake->direction == SDL_SCANCODE_UP || snake->direction == SDL_SCANCODE_DOWN) {
        DrawLine(screen, GAME_AREA_X + snake->parts[0].x * 10 + 2, GAME_AREA_Y + snake->parts[0].y * 10 + 2, 6, 0, 1, WHITE);
        DrawLine(screen, GAME_AREA_X + snake->parts[0].x * 10 + 6, GAME_AREA_Y + snake->parts[0].y * 10 + 2, 6, 0, 1, WHITE);
    }
    else if (snake->direction == SDL_SCANCODE_RIGHT || snake->direction == SDL_SCANCODE_LEFT) {
        DrawLine(screen, GAME_AREA_X + snake->parts[0].x * 10 + 2, GAME_AREA_Y + snake->parts[0].y * 10 + 2, 6, 1, 0, WHITE);
        DrawLine(screen, GAME_AREA_X + snake->parts[0].x * 10 + 2, GAME_AREA_Y + snake->parts[0].y * 10 + 6, 6, 1, 0, WHITE);
    }

}



//------------------------------------------------------------------------
//--------------------------- SNAKE FUNCTIONS ----------------------------
//------------------------------------------------------------------------

// initialize snake
SNAKE* InitSnake() {
    SNAKE* snake = new SNAKE;
    snake->length = SNAKE_LENGTH;
    snake->direction = SDL_SCANCODE_RIGHT;
    snake->speed = SNAKE_SPEED;
    snake->speedChange = CHANGE_SPEED;
    snake->longerSnake = LONGER_SNAKE;
    snake->shorterSnake = SHORTER_SNAKE;
    snake->parts = new SNAKE_PARTS[snake->length];
    snake->ableToMove = SDL_GetTicks();
    snake->points = 0;
    int startX = (GAME_AREA_WIDTH / 2) / 10;
    int startY = (GAME_AREA_HEIGHT / 2) / 10;
    for (int i = 0; i < snake->length; i++) {
        snake->parts[i].x = startX - i;
        snake->parts[i].y = startY;
    }

    return snake;
}

// draw snake on the screen
void DrawSnake(SDL_Surface* screen, SNAKE* snake) {
    for (int i = 0; i < snake->length; i++) {
        if (i == 0) {
            DrawSnakeHead(screen, snake);
        }
        else {
            DrawRectangle(screen, GAME_AREA_X + snake->parts[i].x * 10, GAME_AREA_Y + snake->parts[i].y * 10, 10, 10, GREEN, GREEN);
        }
    }
}

// update snake position
void UpdateSnake(SNAKE* snake) {
    for (int i = snake->length - 1; i > 0; i--) {
        snake->parts[i].x = snake->parts[i - 1].x;
        snake->parts[i].y = snake->parts[i - 1].y;
    }
    switch (snake->direction) {
    case SDL_SCANCODE_UP:
        if (snake->parts[0].y > 0) {
            snake->parts[0].y--;
        }
        else if (snake->parts[0].x == GAME_AREA_WIDTH / 10 - 1) {
            snake->direction = SDL_SCANCODE_LEFT;
            snake->parts[0].x--;
        }
        else {
            snake->direction = SDL_SCANCODE_RIGHT;
            snake->parts[0].x++;
        }
        break;
    case SDL_SCANCODE_DOWN:
        if (snake->parts[0].y < GAME_AREA_HEIGHT / 10 - 1) {
            snake->parts[0].y++;
        }
        else if (snake->parts[0].x == 0) {
            snake->direction = SDL_SCANCODE_RIGHT;
            snake->parts[0].x++;
        }
        else {
            snake->direction = SDL_SCANCODE_LEFT;
            snake->parts[0].x--;
        }
        break;
    case SDL_SCANCODE_LEFT:
        if (snake->parts[0].x > 0) {
            snake->parts[0].x--;
        }
        else if (snake->parts[0].y == 0) {
            snake->direction = SDL_SCANCODE_DOWN;
            snake->parts[0].y++;
        }
        else {
            snake->direction = SDL_SCANCODE_UP;
            snake->parts[0].y--;
        }
        break;
    case SDL_SCANCODE_RIGHT:
        if (snake->parts[0].x < GAME_AREA_WIDTH / 10 - 1) {
            snake->parts[0].x++;
        }
        else if (snake->parts[0].y == GAME_AREA_HEIGHT / 10 - 1) {
            snake->direction = SDL_SCANCODE_UP;
            snake->parts[0].y--;
        }
        else {
            snake->direction = SDL_SCANCODE_DOWN;
            snake->parts[0].y++;
        }
        break;
    }
}

// moving snake by arrows
void SnakeKeys(SNAKE* snake, SDL_Event event) {
    switch (event.key.keysym.scancode) {
    case SDL_SCANCODE_UP:
        if (snake->direction != SDL_SCANCODE_DOWN && (snake->direction != SDL_SCANCODE_LEFT || snake->parts[0].y != 0) && (snake->direction != SDL_SCANCODE_RIGHT || snake->parts[0].y != 0)) {
            snake->direction = SDL_SCANCODE_UP;
        }
        break;
    case SDL_SCANCODE_DOWN:
        if (snake->direction != SDL_SCANCODE_UP && (snake->direction != SDL_SCANCODE_RIGHT || snake->parts[0].y != GAME_AREA_HEIGHT / 10 - 1) && (snake->direction != SDL_SCANCODE_LEFT || snake->parts[0].y != GAME_AREA_HEIGHT / 10 - 1)) {
            snake->direction = SDL_SCANCODE_DOWN;
        }
        break;
    case SDL_SCANCODE_LEFT:
        if (snake->direction != SDL_SCANCODE_RIGHT && (snake->direction != SDL_SCANCODE_DOWN || snake->parts[0].x != 0) && (snake->direction != SDL_SCANCODE_UP || snake->parts[0].x != 0)) {
            snake->direction = SDL_SCANCODE_LEFT;
        }
        break;
    case SDL_SCANCODE_RIGHT:
        if (snake->direction != SDL_SCANCODE_LEFT && (snake->direction != SDL_SCANCODE_UP || snake->parts[0].x != GAME_AREA_WIDTH / 10 - 1) && (snake->direction != SDL_SCANCODE_DOWN || snake->parts[0].x != GAME_AREA_WIDTH / 10 - 1)) {
            snake->direction = SDL_SCANCODE_RIGHT;
        }
        break;
    }
}

// move snake on the screen
void MoveSnake(SDL_Surface* screen, SNAKE* snake, TIMER* timer) {
    int currentTime = SDL_GetTicks();

    if (currentTime - snake->ableToMove > snake->speed) {
        UpdateSnake(snake);
        snake->ableToMove = SDL_GetTicks();
    }

    DrawSnake(screen, snake);
}

// make snake longer
void LongerSnake(SNAKE* snake) {
    SNAKE_PARTS* newParts = new SNAKE_PARTS[snake->length + snake->longerSnake];
    for (int i = 0; i < snake->length + snake->longerSnake - 1; i++) {
        newParts[i] = snake->parts[i];
    }
    for (int i = 0; i < snake->longerSnake; i++) {
        newParts[snake->length + i] = snake->parts[snake->length - 1];
    }
    delete[] snake->parts;
    snake->parts = newParts;
    snake->length += snake->longerSnake;
}

// make snake shorter
void ShorterSnake(SNAKE* snake) {
    SNAKE_PARTS* newParts = new SNAKE_PARTS[snake->length - snake->shorterSnake];
    for (int i = 0; i < snake->length - snake->shorterSnake; i++) {
        newParts[i] = snake->parts[i];
    }
    delete[] snake->parts;
    snake->parts = newParts;
    snake->length -= snake->shorterSnake;
}

//------------------------------------------------------------------------
//-------------------- BLUEBERRY & REDBERRY FUNCTIONS --------------------
//------------------------------------------------------------------------
// 
// check if berry is on the snake
bool berryCordinates(SNAKE* snake, int x, int y) {
    for (int i = 0; i < snake->length; i++) {
        if (snake->parts[i].x == x && snake->parts[i].y == y) {
            return true;
        }
    }
    return false;
}

// initialize blueberry
BLUEBERRY* InitBlueberry(SNAKE* snake) {
    BLUEBERRY* blueberry = new BLUEBERRY;
    do {
        blueberry->x = rand() % (GAME_AREA_WIDTH / 10);
        blueberry->y = rand() % (GAME_AREA_HEIGHT / 10);
    } while (berryCordinates(snake, blueberry->x, blueberry->y));
    return blueberry;
}

// check if blueberry was eaten
bool CheckBlueberryEaten(BLUEBERRY* blueberry, SNAKE* snake) {
    if (snake->parts[0].x == blueberry->x && snake->parts[0].y == blueberry->y) {
        LongerSnake(snake);
        snake->points++;
        return true;
    }
    return false;
}

// update blue dot on the screen
void UpdateBlueberry(SDL_Surface* screen, BLUEBERRY* blueberry, SNAKE* snake) {

    if (CheckBlueberryEaten(blueberry, snake)) {
        do {
            blueberry->x = rand() % (GAME_AREA_WIDTH / 10);
            blueberry->y = rand() % (GAME_AREA_HEIGHT / 10);
        } while (berryCordinates(snake, blueberry->x, blueberry->y));
    }
    DrawRectangle(screen, GAME_AREA_X + blueberry->x * 10, GAME_AREA_Y + blueberry->y * 10, 10, 10, WHITE, BLUE);
}

// initialize redberry
REDBERRY* InitRedberry(BLUEBERRY* blueberry, SNAKE* snake) {
    REDBERRY* redberry = new REDBERRY;
    do {
        redberry->x = rand() % (GAME_AREA_WIDTH / 10);
        redberry->y = rand() % (GAME_AREA_HEIGHT / 10);
    } while (berryCordinates(snake, redberry->x, redberry->y) || (redberry->x == blueberry->x && redberry->y == blueberry->y));
    redberry->enabled = 0;
    return redberry;
}

// check if redberry was eaten
bool CheckRedberryEaten(REDBERRY* redberry, SNAKE* snake, TIMER* timer) {
    if (redberry->enabled && snake->parts[0].x == redberry->x && snake->parts[0].y == redberry->y) {
        timer->redBerryLastTime = SDL_GetTicks();
        snake->points++;
        redberry->enabled = false;
        return true;
    }
    return false;
}

// update red dot on the screen
void UpdateRedberry(SDL_Surface* screen, REDBERRY* redberry, BLUEBERRY* blueberry, SNAKE* snake, TIMER* timer) {
    if (CheckRedberryEaten(redberry, snake, timer)) {
        redberry->enabled = 0;
        timer->redBerryTime = 0;
        int choice = rand() % 2;
        if (choice == 0) {
            if (snake->length > 5) {
                ShorterSnake(snake);
            }
        }
        else {
            if (snake->speed <= 80) {
                snake->speed += 2 * snake->speedChange;
            }
        }
    }

    if (redberry->enabled) {
        DrawRectangle(screen, GAME_AREA_X + redberry->x * 10, GAME_AREA_Y + redberry->y * 10, 10, 10, WHITE, RED);
        DrawProgressBar(screen, timer);
    }
}

//------------------------------------------------------------------------
//---------------------------- TIMER FUNCTIONS ---------------------------
//------------------------------------------------------------------------

// initialize timer
TIMER* InitTimer() {
    TIMER* timer = new TIMER;

    // game time
    timer->gameTime = 0;
    timer->lastTimeUpdate = SDL_GetTicks();

    // snake time
    timer->fasterSnakeTime = FASTER_SNAKE;
    timer->snakeTime = 0;

    // red berry time
    timer->redBerryTime = 0;
    timer->redBerryRandomTime = RED_BERRY_RANDOM_TIME;
    timer->redBerryNextTime = timer->redBerryRandomTime + rand() % timer->redBerryRandomTime;
    timer->redBerryDuration = RED_BERRY_TIME;
    return timer;
}

// timing mechanics
void UpdateTimer(TIMER* timer, SNAKE* snake, REDBERRY* redberry, BLUEBERRY* blueberry, SDL_Surface* screen) {
    int currentTime = SDL_GetTicks();
    double delta = double(currentTime - timer->lastTimeUpdate) / 1000;
    timer->gameTime += delta;
    timer->redBerryTime += delta;
    timer->snakeTime += delta;
    timer->lastTimeUpdate = currentTime;

    if (timer->snakeTime > timer->fasterSnakeTime) {
        timer->snakeTime = 0;
        if (snake->speed > 10) {
            snake->speed -= snake->speedChange;
        }
    }

    if (!redberry->enabled && timer->redBerryTime > timer->redBerryNextTime) {
        redberry->enabled = 1;
        timer->redBerryTime = 0;
        timer->redBerryNextTime = timer->redBerryRandomTime + rand() % timer->redBerryRandomTime;
        do {
            redberry->x = rand() % (GAME_AREA_WIDTH / 10);
            redberry->y = rand() % (GAME_AREA_HEIGHT / 10);
        } while (berryCordinates(snake, redberry->x, redberry->y) || (redberry->x == blueberry->x && redberry->y == blueberry->y));
    }
    else if (redberry->enabled && timer->redBerryTime > timer->redBerryDuration) {
        redberry->enabled = 0;
        timer->redBerryTime = 0;
    }

}



//------------------------------------------------------------------------
//------------------------ GAME LOGIC FUNCTIONS --------------------------
//------------------------------------------------------------------------

// check if snake has collided with itself
bool CheckCollisions(SNAKE* snake) {
    for (int i = 1; i < snake->length; i++) {
        if (snake->parts[0].x == snake->parts[i].x && snake->parts[0].y == snake->parts[i].y) {
            return true;
        }
    }
    return false;
}

// save state of the game
void saveGame(SNAKE* snake, TIMER* timer, BLUEBERRY* blueberry, REDBERRY* redberry) {
    FILE* file = fopen(GAME_DATA, "w");
    if (file) {
        fprintf(file, "SNAKE_LENGTH: %d\n", snake->length);
        for (int i = 0; i < snake->length; i++) {
            fprintf(file, "SNAKE_PART %d: " "%d %d\n", i, snake->parts[i].x, snake->parts[i].y);
        }
        fprintf(file, "SNAKE_DIRECTION: %d\n", snake->direction);
        fprintf(file, "SNAKE_SPEED: %d\n", snake->speed);
        fprintf(file, "POINTS: %d\n", snake->points);
        fprintf(file, "GAME_TIME: %lf\n", timer->gameTime);
        fprintf(file, "BLUEBERRY_COORDINATES: %d %d\n", blueberry->x, blueberry->y);
        fprintf(file, "REDBERRY_COORDINATES: %d %d\n", redberry->x, redberry->y);
        fprintf(file, "REDBERRY_ENABLED: %d\n", redberry->enabled);
        fprintf(file, "REDBERRY_TIME: %lf\n", timer->redBerryTime);
        fclose(file);
    }
    else {
        printf("Nie udalo sie zapisac gry\n");
    }
}

// load saved game
void loadGame(SNAKE* snake, TIMER* timer, BLUEBERRY* blueberry, REDBERRY* redberry) {
    FILE* file = fopen(GAME_DATA, "r");
    if (file) {
        if (fscanf(file, "SNAKE_LENGTH: %d\n", &snake->length) == 1) {
            printf("Wczytano dlugosc weza: %d\n", snake->length);
        }

        delete[] snake->parts;
        snake->parts = new SNAKE_PARTS[snake->length];
        for (int i = 0; i < snake->length; i++) {
            if (fscanf(file, "SNAKE_PART %d: " "%d %d\n", &i, &snake->parts[i].x, &snake->parts[i].y) == 3) {
                printf("Wczytano segment %d: %d %d\n", i, snake->parts[i].x, snake->parts[i].y);
            }
        }
        if (fscanf(file, "SNAKE_DIRECTION: %d\n", &snake->direction) == 1) {
            printf("Wczytano kierunek weza: %d\n", snake->direction);
        }

        if (fscanf(file, "SNAKE_SPEED: %d\n", &snake->speed) == 1) {
            printf("Wczytano predkosc weza: %d\n", snake->speed);
        }

        if (fscanf(file, "POINTS: %d\n", &snake->points) == 1) {
            printf("Wczytano punkty: %d\n", snake->points);
        }

        if (fscanf(file, "GAME_TIME: %lf\n", &timer->gameTime) == 1) {
            printf("Wczytano czas gry: %lf\n", timer->gameTime);
        }

        if (fscanf(file, "BLUEBERRY_COORDINATES: %d %d\n", &blueberry->x, &blueberry->y) == 2) {
            printf("Wczytano pozycje blueberry: %d %d\n", blueberry->x, blueberry->y);
        }

        if (fscanf(file, "REDBERRY_COORDINATES: %d %d\n", &redberry->x, &redberry->y) == 2) {
            printf("Wczytano pozycje redberry: %d %d\n", redberry->x, redberry->y);
        }

        if (fscanf(file, "REDBERRY_ENABLED: %d\n", &redberry->enabled) == 1) {
            printf("Wczytano czy redberry jest wlaczony: %d\n", redberry->enabled);
        }

        if (fscanf(file, "REDBERRY_TIME: %lf\n", &timer->redBerryTime) == 1) {
            printf("Wczytano czas redberry: %lf\n", timer->redBerryTime);
        }
        fclose(file);
    }
    else {
        printf("Nie znaleziono zapisu gry\n");
    }
}


//------------------------------------------------------------------------
//----------------- INITIALIZATION & CLEAN UP FUNCTIONS ------------------
//------------------------------------------------------------------------

// clean up game
void CleanUpGame(SDL_Surface* screen, SDL_Texture* scrtex, SNAKE* snake, BLUEBERRY* blueberry, TIMER* timer, REDBERRY* redberry) {
    SDL_FreeSurface(screen);
    SDL_DestroyTexture(scrtex);
    delete[] snake->parts;
    delete snake;
    delete blueberry;
    delete redberry;
    delete timer;
}

// show menu screen
void Menu(SDL_Window* window, SDL_Renderer* renderer, SDL_Surface* screen, SDL_Texture* scrtex, SDL_Surface* charset, SNAKE* snake, BLUEBERRY* blueberry, TIMER* timer, REDBERRY* redberry) {
    if (window && renderer) {
        bool quit = false;
        SDL_Event event;

        while (!quit) {
            // Obs uga zdarze 
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    quit = true;
                }
                else if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                        quit = true;
                    }
                    else if (event.key.keysym.scancode == SDL_SCANCODE_N) {
                        return;
                    }
                }
            }

            // screen reset
            SDL_FillRect(screen, NULL, BLACK);

            DrawRectangle(screen, 50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100, WHITE, BLACK);
            DrawString(screen, 100, 100, "SNAKE GAME", charset);
            DrawString(screen, 100, 200, "Press N to Start The New Game", charset);
            DrawString(screen, 100, 300, "Press ESC to Quit", charset);


            // update screen
            SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, scrtex, NULL, NULL);
            SDL_RenderPresent(renderer);
            SDL_Delay(16);
        }
    }
    CleanUpGame(screen, scrtex, snake, blueberry, timer, redberry);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    exit(0);
}

// initialize window
void initializeWindow(SDL_Window*& mainwin, SDL_Renderer*& renderer) {
    mainwin = SDL_CreateWindow("Etap 2 - Snake 2024/2025", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(mainwin, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

// init game parameters
void initializeGame(SDL_Window* mainwin, SDL_Renderer* renderer, SDL_Surface*& screen, SDL_Texture*& scrtex, SDL_Surface*& charset,
    SNAKE*& snake, BLUEBERRY*& blueberry, TIMER*& timer, REDBERRY*& redberry) {

    screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);

    scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

    charset = SDL_LoadBMP(CHARSET_PATH);
    SDL_SetColorKey(charset, true, BLACK);

    snake = InitSnake();

    blueberry = InitBlueberry(snake);

    redberry = InitRedberry(blueberry, snake);

    timer = InitTimer();

}

// show your score after game over
void ShowScore(SDL_Surface* screen, SDL_Surface* charset, SNAKE* snake, SDL_Renderer* renderer, SDL_Texture* scrtex, TIMER* timer, bool* running) {
    char scoreText[64];
    SDL_FillRect(screen, NULL, BLACK);
    DrawString(screen, SCREEN_WIDTH / 2 - strlen("GAME OVER!") * 4, SCREEN_HEIGHT / 2 - 50, "GAME OVER!", charset);
    sprintf(scoreText, "Your score: %d", snake->points);
    DrawString(screen, SCREEN_WIDTH / 2 - strlen(scoreText) * 4, SCREEN_HEIGHT / 2 - 25, scoreText, charset);
    sprintf(scoreText, "Snake length: %d", snake->length);
    DrawString(screen, SCREEN_WIDTH / 2 - strlen(scoreText) * 4, SCREEN_HEIGHT / 2, scoreText, charset);
    sprintf(scoreText, "Time: %.1lf s", timer->gameTime);
    DrawString(screen, SCREEN_WIDTH / 2 - strlen(scoreText) * 4, SCREEN_HEIGHT / 2 + 25, scoreText, charset);
    SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, scrtex, NULL, NULL);
    SDL_RenderPresent(renderer);
    int startTime = SDL_GetTicks();
    bool quit = false;
    while (!quit && (SDL_GetTicks() - startTime < 5000)) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
                *running = false;
            }
        }
        SDL_Delay(16);
    }
}


//------------------------------------------------------------------------
//----------------------------- MAIN LOOP FUNCTION -----------------------
//------------------------------------------------------------------------

void MainLoop(SDL_Window* window, SDL_Renderer* renderer, SDL_Surface* screen, SDL_Texture* scrtex, SDL_Surface* charset,
    SNAKE* snake, BLUEBERRY* blueberry, TIMER* timer, REDBERRY* redberry, bool* running) {
    if (window && renderer) {
        bool quit = false;
        SDL_Event event;
        timer->lastTimeUpdate = SDL_GetTicks();

        while (!quit && !CheckCollisions(snake)) {
            // Obs uga zdarze 
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    quit = true;
                    *running = false;
                    return;
                }
                else if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                        quit = true;
                    }
                    else if (event.key.keysym.scancode == SDL_SCANCODE_S) {
                        saveGame(snake, timer, blueberry, redberry);
                    }
                    else if (event.key.keysym.scancode == SDL_SCANCODE_L) {
                        loadGame(snake, timer, blueberry, redberry);
                    }
                }
                SnakeKeys(snake, event);
            }

            // screen reset
            SDL_FillRect(screen, NULL, BLACK);

            // update timer
            UpdateTimer(timer, snake, redberry, blueberry, screen);

            // show board and ststs
            DrawGameArea(screen);
            DrawStats(screen, timer, charset, snake);

            // show and update snake
            MoveSnake(screen, snake, timer);

            // show and update blueberry and check if it was eaten
            UpdateBlueberry(screen, blueberry, snake);

            // show and update redberry and check if it was eaten
            UpdateRedberry(screen, redberry, blueberry, snake, timer);



            // update screen
            SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, scrtex, NULL, NULL);
            SDL_RenderPresent(renderer);
            SDL_Delay(16);
        }
        // show score after game over
        ShowScore(screen, charset, snake, renderer, scrtex, timer, running);
    }
}

//------------------------------------------------------------------------
//------------------------------ MAIN FUNCTION ---------------------------
//------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("SDL_Init error: %s\n", SDL_GetError());
        return 1;
    }
    srand(time(NULL));
    SDL_Window* mainwin = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Surface* screen = nullptr;
    SDL_Surface* charset = nullptr;
    SDL_Texture* scrtex = nullptr;
    SNAKE* snake = nullptr;
    BLUEBERRY* blueberry = nullptr;
    REDBERRY* redberry = nullptr;
    TIMER* timer = nullptr;
    bool running = true;
    initializeWindow(mainwin, renderer);
    while (running) {
        initializeGame(mainwin, renderer, screen, scrtex, charset, snake, blueberry, timer, redberry);

        Menu(mainwin, renderer, screen, scrtex, charset, snake, blueberry, timer, redberry);

        MainLoop(mainwin, renderer, screen, scrtex, charset, snake, blueberry, timer, redberry, &running);

        CleanUpGame(screen, scrtex, snake, blueberry, timer, redberry);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(mainwin);
    SDL_Quit();
    exit(0);
    return 0;
}
