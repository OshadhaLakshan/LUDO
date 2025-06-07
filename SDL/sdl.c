#define TOTAL_STEPS 52

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

enum { RED = 0, GREEN = 1, BLUE = 2, YELLOW = 3 };

const int startIndex[4] = {
    0,   // Red starts at index 0
    13,  // Green starts 13 steps ahead
    26,  // Yellow
    39   // Blue
};

typedef struct {
    int x, y;
    SDL_Color color;
    int stepIndex;  // Index in universalPath
    int inPlay;     // 0 = in base, 1 = active
} Token;

SDL_Point universalPath[52] = {
    {345,  65},  // 0 Red start
    {345, 105},
    {345, 145},
    {345, 185},
    {345, 225},
    {385, 265},  // Red entry to center
    {425, 265},
    {465, 265},
    {505, 265},
    {545, 265},
    {585, 265},  // Right-most
    {585, 305},
    {585, 345},
    {545, 345},
    {505, 345},
    {465, 345},
    {425, 345},  // Bottom right corner
    {385, 345},
    {345, 385},
    {345, 425},
    {345, 465},
    {345, 505},
    {345, 545},  // Bottom-most
    {345, 585},
    {305, 585},
    {265, 585},
    {265, 545},
    {265, 505},
    {265, 465},
    {265, 425},  // Bottom left
    {265, 385},
    {225, 345},
    {185, 345},
    {145, 345},
    {105, 345},
    { 65, 345},
    { 25, 345},
    { 25, 305},
    { 25, 265},
    { 65, 265},
    {105, 265},
    {145, 265},
    {185, 265},
    {225, 265},
    {265, 225},
    {265, 185},   // Left-most
    {265, 145},
    {265, 105},
    {265,  65},
    {265,  25},
    {305,  25},
    {345,  25}  // Green center entry
};

// Declare the function before main
void draw_token(SDL_Renderer* renderer, Token token);
void move_token(Token* token, int direction);

int main() {
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL_Init error: %s", SDL_GetError());
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        SDL_Log("IMG_Init error: %s", IMG_GetError());
        return 1;
    }

    SDL_Surface* imageSurface = IMG_Load("ludo.png");
    if (!imageSurface) {
        SDL_Log("IMG_Load error: %s", IMG_GetError());
        return 1;
    }

    int imgWidth = imageSurface->w;
    int imgHeight = imageSurface->h;

    SDL_Window* window = SDL_CreateWindow("Ludo Board",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        imgWidth, imgHeight, SDL_WINDOW_SHOWN);

    SDL_Surface* icon = IMG_Load("icon.png");  // Use PNG or BMP here
    if (icon) {
    	SDL_SetWindowIcon(window, icon);
    	SDL_FreeSurface(icon);
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    SDL_FreeSurface(imageSurface);

    int selectedColor = -1;  // -1 means no color selected
    int selectedIndex[4] = {0, 0, 0, 0};  // Which token of each color was placed last

    // Keep tokens outside the loop so their state is preserved
    Token tokens[4][4] = {
    	{   // Red tokens
            {445, 125, {105, 0, 0}},   // Red 1
            {485,  85, {105, 0, 0}},   // Red 2
            {525, 125, {105, 0, 0}},   // Red 3
            {485, 165, {105, 0, 0}}    // Red 4
    	},
    	{   // Green tokens
            {125, 165, {0, 105, 0}},   // Green 1
            { 85, 125, {0, 105, 0}},   // Green 2
            {125,  85, {0, 105, 0}},   // Green 3
            {165, 125, {0, 105, 0}}    // Green 4
    	},
    	{   // Blue tokens
            {485, 445, {0, 0, 105}},   // Blue 1
            {525, 485, {0, 0, 105}},   // Blue 2
            {485, 525, {0, 0, 105}},   // Blue 3
            {445, 485, {0, 0, 105}}    // Blue 4
    	},
    	{   // Yellow tokens
            {165, 485, {105, 105, 0}}, // Yellow 1
            {125, 525, {105, 105, 0}}, // Yellow 2
            { 85, 485, {105, 105, 0}}, // Yellow 3
            {125, 445, {105, 105, 0}}  // Yellow 4
    	}
    };


    int running = 1;
    SDL_Event e;

    int greenIndex = 0;
    int redIndex = 0;
    int blueIndex = 0;
    int yellowIndex = 0;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = 0;

            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    // --- Place Tokens ---
                    case SDLK_KP_7:  // Green
                        if (greenIndex < 4) {
                            tokens[GREEN][greenIndex].x = 65;
                            tokens[GREEN][greenIndex].y = 265;
                            tokens[GREEN][greenIndex].inPlay = 1;
                            tokens[GREEN][greenIndex].stepIndex = startIndex[GREEN];
                            selectedIndex[GREEN] = greenIndex;
                            greenIndex++;
                        }
                        break;

                    case SDLK_KP_9:  // Red
                            if (redIndex < 4) {
                            tokens[RED][redIndex].x = 345;
                            tokens[RED][redIndex].y = 65;
                            tokens[RED][redIndex].inPlay = 1;
                            tokens[RED][redIndex].stepIndex = startIndex[RED];
                            selectedIndex[RED] = redIndex;
                            redIndex++;
                        }
                        break;

                    case SDLK_KP_1:  // Yellow
                        if (yellowIndex < 4) {
                            tokens[YELLOW][yellowIndex].x = 265;
                            tokens[YELLOW][yellowIndex].y = 545;
                            tokens[YELLOW][yellowIndex].inPlay = 1;
                            tokens[YELLOW][yellowIndex].stepIndex = startIndex[YELLOW];
                            selectedIndex[YELLOW] = yellowIndex;
                            yellowIndex++;
                        }
                        break;

                    case SDLK_KP_3:  // Blue
                        if (blueIndex < 4) {
                            tokens[BLUE][blueIndex].x = 545;
                            tokens[BLUE][blueIndex].y = 345;
                            tokens[BLUE][blueIndex].inPlay = 1;
                            tokens[BLUE][blueIndex].stepIndex = startIndex[BLUE];
                            selectedIndex[BLUE] = blueIndex;
                            blueIndex++;
                        }
                        break;

                    // --- Select Token by Color ---
                    case SDLK_a: selectedColor = GREEN; break;
                    case SDLK_d: selectedColor = RED; break;
                    case SDLK_j: selectedColor = YELLOW; break;
                    case SDLK_l: selectedColor = BLUE; break;

                    // --- Move Token with Arrow Keys ---
                    case SDLK_RIGHT:
                        if (selectedColor != -1) {
                            move_token(&tokens[selectedColor][selectedIndex[selectedColor]], +1);
                        }
                        break;

                    case SDLK_LEFT:
                        if (selectedColor != -1) {
                            move_token(&tokens[selectedColor][selectedIndex[selectedColor]], -1);
                        }
                        break;
                }
            }
        }

        SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // Set white background
	SDL_RenderClear(renderer);


        SDL_Rect destRect = {0, 0, imgWidth, imgHeight};
        SDL_RenderCopy(renderer, texture, NULL, &destRect);

        for (int i = 0; i < 4; i++) {
	    for (int j = 0; j < 4; j++) {
            	draw_token(renderer, tokens[i][j]);
	    }
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}

// Function for generate Tokens
void draw_token(SDL_Renderer* renderer, Token token) {
    SDL_SetRenderDrawColor(renderer, token.color.r, token.color.g, token.color.b, 255);
    SDL_Rect rect = { token.x, token.y, 30, 30 };
    SDL_RenderFillRect(renderer, &rect);
}

// Function for token movement on PATH
void move_token(Token* token, int direction) {
    if (!token->inPlay) return;

    token->stepIndex = (token->stepIndex + direction + TOTAL_STEPS) % TOTAL_STEPS;

    SDL_Point pos = universalPath[token->stepIndex];
    token->x = pos.x;
    token->y = pos.y;
}




// 🔧 Step-by-Step Fix

// Download and Install MSYS2
// If not installed, download from:
// 👉 https://www.msys2.org

// Run the Correct Shell
// After installing MSYS2, launch:
// MSYS2 MinGW 64-bit (not just the regular MSYS shell).
// You can find this in the Start Menu under "MSYS2 64bit".

// Update MSYS2 (Optional but Recommended)
// Run these in the MSYS2 shell:
// pacman -Syu

// Then close the window and re-open the same shell, and run:
// pacman -Su

// Install SDL2 and SDL2_image
// pacman -S mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image

// Compile with icon, SDL2 and SDL2_image
// gcc sdl.c icon.res -o sdl.exe -lSDL2 -lSDL2_image
