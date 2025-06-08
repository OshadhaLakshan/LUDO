#define TOTAL_STEPS 52

#define SDL_MAIN_HANDLED

#include <math.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

enum { RED = 0, GREEN = 1, BLUE = 2, YELLOW = 3 };

const int startIndex[4] = {
    0,   // Red starts at index 0
    39,  // Green
    13,  // Blue
    26   // Yellow
};

typedef struct {
    int x, y;                  	// Current position
    int baseX, baseY;          	// Original base position (for returning)
    SDL_Color color;
    int stepIndex;             	// Universal path index
    int homeStepIndex;         	// For final home path
    int inPlay;                	// 1 = active, 0 = in base
    int inHome;                	// 1 = in final home path
    int reachedHome;		// 1 = has reached home
} Token;

SDL_Point redHomePath[7] = {
    {305, 25},   // Entry to home
    {305, 65},
    {305, 105},
    {305, 145},
    {305, 185},
    {305, 225},
    {305, 265}   // Final home position
};

SDL_Point greenHomePath[7] = {
    {25, 305},   // Entry to home
    {65, 305},
    {105, 305},
    {145, 305},
    {185, 305},
    {225, 305},
    {265, 305}   // Final home position
};

SDL_Point blueHomePath[7] = {
    {585, 305},   // Entry to home
    {545, 305},
    {505, 305},
    {465, 305},
    {425, 305},
    {385, 305},
    {345, 305}   // Final home position
};

SDL_Point yellowHomePath[7] = {
    {305, 585},   // Entry to home
    {305, 545},
    {305, 505},
    {305, 465},
    {305, 425},
    {305, 385},
    {305, 345}   // Final home position
};

SDL_Point universalPath[52] = {
    {345,  65},  // Red start
    {345, 105},
    {345, 145},
    {345, 185},
    {345, 225},
    {385, 265},  
    {425, 265},
    {465, 265},
    {505, 265},
    {545, 265},
    {585, 265},  
    {585, 305}, // Blue Home entry
    {585, 345},
    {545, 345}, // Blue start
    {505, 345},
    {465, 345},
    {425, 345},  
    {385, 345},
    {345, 385},
    {345, 425},
    {345, 465},
    {345, 505},
    {345, 545},  
    {345, 585}, 
    {305, 585}, // Yellow Home entry
    {265, 585},
    {265, 545}, // Yellow start
    {265, 505},
    {265, 465},
    {265, 425},  
    {265, 385},
    {225, 345},
    {185, 345},
    {145, 345},
    {105, 345},
    { 65, 345},
    { 25, 345},
    { 25, 305}, // Green Home entry
    { 25, 265},
    { 65, 265}, // Green start
    {105, 265},
    {145, 265},
    {185, 265},
    {225, 265},
    {265, 225},
    {265, 185},   
    {265, 145},
    {265, 105},
    {265,  65},
    {265,  25},
    {305,  25}, // Red Home entry
    {345,  25}  
};

// Declare the function before main
void draw_token(SDL_Renderer* renderer, Token token, int isSelected);
void move_token(Token* token, int direction, Token tokens[4][4]);
void draw_letter(SDL_Renderer* renderer, TTF_Font* font, char letter, int x, int y, SDL_Color color);

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

    SDL_Surface* imageSurface = IMG_Load("assets/ludo.png");
    if (!imageSurface) {
        SDL_Log("IMG_Load error: %s", IMG_GetError());
        return 1;
    }

    int imgWidth = imageSurface->w;
    int imgHeight = imageSurface->h;

    SDL_Window* window = SDL_CreateWindow("LUDO like UCSC - by Oshadha Lakshan",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        imgWidth, imgHeight, SDL_WINDOW_SHOWN);

    SDL_Surface* icon = IMG_Load("assets/icon.png");  // Use PNG or BMP here
    if (icon) {
    	SDL_SetWindowIcon(window, icon);
    	SDL_FreeSurface(icon);
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    SDL_FreeSurface(imageSurface);

    // Right after SDL and IMG_Init succeed
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
    	SDL_Log("IMG_Init error: %s", IMG_GetError());
    	return 1;
    }

    // ✅ Add this block
    if (TTF_Init() == -1) {
    	printf("TTF_Init failed: %s\n", TTF_GetError());
        return 1;
    }

    TTF_Font* font = TTF_OpenFont("assets/NunitoSans.ttf", 20);
    if (!font) {
    	printf("Failed to load font: %s\n", TTF_GetError());
    } else {
    	TTF_SetFontStyle(font, TTF_STYLE_BOLD);
    }

    int selectedColor = -1;  // -1 means no color selected
    int selectedIndex[4] = {0, 0, 0, 0};  // Which token of each color was placed last

    // Keep tokens outside the loop so their state is preserved
    Token tokens[4][4] = {
    	{   // Red tokens
            {445, 125, 445, 125, {255, 0, 0}, 0, 0, 0, 0}, // Red 1
            {485,  85, 485,  85, {255, 0, 0}, 0, 0, 0, 0}, // Red 2
            {525, 125, 525, 125, {255, 0, 0}, 0, 0, 0, 0}, // Red 3
            {485, 165, 485, 165, {255, 0, 0}, 0, 0, 0, 0}  // Red 4
    	},
    	{   // Green tokens
            {125, 165, 125, 165, {0, 255, 0}, 0, 0, 0, 0},
            { 85, 125,  85, 125, {0, 255, 0}, 0, 0, 0, 0},
            {125,  85, 125,  85, {0, 255, 0}, 0, 0, 0, 0},
            {165, 125, 165, 125, {0, 255, 0}, 0, 0, 0, 0}
    	},
    	{   // Blue tokens
            {485, 445, 485, 445, {0, 0, 255}, 0, 0, 0, 0},
            {525, 485, 525, 485, {0, 0, 255}, 0, 0, 0, 0},
            {485, 525, 485, 525, {0, 0, 255}, 0, 0, 0, 0},
            {445, 485, 445, 485, {0, 0, 255}, 0, 0, 0, 0}
    	},
    	{   // Yellow tokens
            {165, 485, 165, 485, {255, 255, 0}, 0, 0, 0, 0},
            {125, 525, 125, 525, {255, 255, 0}, 0, 0, 0, 0},
            { 85, 485,  85, 485, {255, 255, 0}, 0, 0, 0, 0},
            {125, 445, 125, 445, {255, 255, 0}, 0, 0, 0, 0}
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
                            Token *token = &tokens[selectedColor][selectedIndex[selectedColor]];
                            // Only move clockwise if token not at end
                            if (!token->reachedHome) {
                                move_token(token, +1, tokens);
                            }
                        }
                        break;

                    case SDLK_LEFT:
                        if (selectedColor != -1) {
                            Token *token = &tokens[selectedColor][selectedIndex[selectedColor]];
                            // Move anticlockwise only if NOT in home path and NOT reached end
                            if (!token->inHome && !token->reachedHome) {
                                move_token(token, -1, tokens);
                            }
                        }
                        break;


            	    // Optional: Cycle through tokens of same color
                    case SDLK_TAB:
                        if (selectedColor != -1) {
                            int count = 0;
                            int nextIndex = selectedIndex[selectedColor];

                            // Find the next in-play token
                            do {
                                nextIndex = (nextIndex + 1) % 4;
                                count++;
                            } while (!tokens[selectedColor][nextIndex].inPlay && count < 4);

                            if (tokens[selectedColor][nextIndex].inPlay) {
                                selectedIndex[selectedColor] = nextIndex;
                            }
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

	SDL_Color black = {0, 0, 0, 255};  // Custom color

	draw_letter(renderer, font, 'B', 155, 345, black);
	draw_letter(renderer, font, 'K', 355,  25, black);
	draw_letter(renderer, font, 'P', 395, 345, black);
	draw_letter(renderer, font, '?', 355, 505, black);

	draw_letter(renderer, font, 'o', 315,  25, black);
	draw_letter(renderer, font, 'o',  35, 305, black);
	draw_letter(renderer, font, 'o', 595, 305, black);
	draw_letter(renderer, font, 'o', 315, 585, black);

	draw_letter(renderer, font, 'x',  75, 265, black);
	draw_letter(renderer, font, 'x', 275, 545, black);
	draw_letter(renderer, font, 'x', 555, 345, black);
	draw_letter(renderer, font, 'x', 355,  65, black);

        for (int i = 0; i < 4; i++) {
	    for (int j = 0; j < 4; j++) {
            	draw_token(renderer, tokens[i][j], (i == selectedColor && j == selectedIndex[i]));

	    }
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}


// Helper function to draw filled circle
void draw_filled_circle(SDL_Renderer* renderer, int centerX, int centerY, int radius, SDL_Color fillColor, SDL_Color borderColor) {
    // Draw filled circle
    SDL_SetRenderDrawColor(renderer, fillColor.r, fillColor.g, fillColor.b, 255);
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx * dx + dy * dy) <= (radius - 2) * (radius - 2)) {
                SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
            }
        }
    }

    // Draw border
    SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, 255);
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            int distSq = dx * dx + dy * dy;
            if (distSq <= radius * radius && distSq >= (radius - 1.5) * (radius - 1.5)) {
                SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
            }
        }
    }
}

// Updated draw_token function
void draw_token(SDL_Renderer* renderer, Token token, int isSelected) {
    int radius = 15;
    int centerX = token.x + radius;
    int centerY = token.y + radius;

    SDL_Color borderColor = {0, 0, 0, 255};  // Black border

    // 1. Optional outer glow if selected
    if (isSelected) {
        SDL_Color glowColor = {255, 255, 255, 255};  // White outer glow
        draw_filled_circle(renderer, centerX, centerY, radius + 3, glowColor, glowColor);
    }

    // 2. Main token circle
    draw_filled_circle(renderer, centerX, centerY, radius, token.color, borderColor);

    // 3. Inner white circle for active selection
    if (isSelected) {
        SDL_Color innerWhite = {255, 255, 255, 255};
        draw_filled_circle(renderer, centerX, centerY, 7, innerWhite, innerWhite);  // 15px diameter ≈ radius 7
    }
}


void draw_letter(SDL_Renderer* renderer, TTF_Font* font, char letter, int x, int y, SDL_Color color) {
    // Convert letter to string
    char text[2] = { letter, '\0' };

    // Render text surface
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
    if (!surface) {
        printf("Text surface error: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        printf("Text texture error: %s\n", SDL_GetError());
        return;
    }

    SDL_Rect dstRect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);
    SDL_DestroyTexture(texture);
}


// Function to move Tokens
void move_token(Token* token, int direction, Token tokens[4][4]) {
    if (!token->inPlay) return;

    if (token->inHome) {
        token->homeStepIndex += direction;

        // Clamp to final position
        if (token->homeStepIndex >= 7) token->homeStepIndex = 6;
        if (token->homeStepIndex < 0) token->homeStepIndex = 0;

        SDL_Point pos;

        // Choose correct home path based on token color
        if (token->color.r == 255 && token->color.g == 0 && token->color.b == 0) {
            pos = redHomePath[token->homeStepIndex];
        } else if (token->color.r == 0 && token->color.g == 255 && token->color.b == 0) {
            pos = greenHomePath[token->homeStepIndex];
        } else if (token->color.r == 0 && token->color.g == 0 && token->color.b == 255) {
            pos = blueHomePath[token->homeStepIndex];
        } else if (token->color.r == 255 && token->color.g == 255 && token->color.b == 0) {
            pos = yellowHomePath[token->homeStepIndex];
        }

        token->x = pos.x;
        token->y = pos.y;
        return;
    }

    // Move on the universal path
    token->stepIndex = (token->stepIndex + direction + TOTAL_STEPS) % TOTAL_STEPS;

    SDL_Point pos = universalPath[token->stepIndex];
    token->x = pos.x;
    token->y = pos.y;

    // Switch to home path at the correct position
    switch (token->color.r) {
        case 255: // Red or Yellow
            if (token->color.g == 0 && token->color.b == 0 &&
                token->x == 305 && token->y == 25) {
                // Red
                token->inHome = 1;
                token->homeStepIndex = 0;
                token->x = redHomePath[0].x;
                token->y = redHomePath[0].y;
            } else if (token->color.g == 255 && token->color.b == 0 &&
                    token->x == 305 && token->y == 585) {
                // Yellow
                token->inHome = 1;
                token->homeStepIndex = 0;
                token->x = yellowHomePath[0].x;
                token->y = yellowHomePath[0].y;
            }
            break;

        case 0:
            if (token->color.g == 255 && token->color.b == 0 &&
                token->x == 25 && token->y == 305) {
                // Green
                token->inHome = 1;
                token->homeStepIndex = 0;
                token->x = greenHomePath[0].x;
                token->y = greenHomePath[0].y;
            } else if (token->color.g == 0 && token->color.b == 255 &&
                    token->x == 585 && token->y == 305) {
                // Blue
                token->inHome = 1;
                token->homeStepIndex = 0;
                token->x = blueHomePath[0].x;
                token->y = blueHomePath[0].y;
            }

            // Check if token is in home path and reached last home position (index 7)
            if (token->inHome) {
                // Red
                if (token->color.r == 255 && token->color.g == 0 && token->color.b == 0 &&
                    token->x == redHomePath[7].x && token->y == redHomePath[7].y) {
                    token->reachedHome = 1;
                }
                // Yellow
                else if (token->color.r == 255 && token->color.g == 255 && token->color.b == 0 &&
                    token->x == yellowHomePath[7].x && token->y == yellowHomePath[7].y) {
                    token->reachedHome = 1;
                }
                // Green
                else if (token->color.r == 0 && token->color.g == 255 && token->color.b == 0 &&
                    token->x == greenHomePath[7].x && token->y == greenHomePath[7].y) {
                    token->reachedHome = 1;
                }
                // Blue
                else if (token->color.r == 0 && token->color.g == 0 && token->color.b == 255 &&
                    token->x == blueHomePath[7].x && token->y == blueHomePath[7].y) {
                    token->reachedHome = 1;
                }
            }

            break;

        default:
            // No action
            break;
    }

    // Check for collisions with other tokens
    for (int c = 0; c < 4; c++) {
    	for (int t = 0; t < 4; t++) {
            Token* other = &tokens[c][t];

            // Don't compare to itself or inactive tokens
            if (other == token || !other->inPlay || other->inHome) continue;

            // If another token is at the same location and is from a different color
            if (other->x == token->x && other->y == token->y &&
               (other->color.r != token->color.r ||
             	other->color.g != token->color.g ||
             	other->color.b != token->color.b)) {

            	// Send the other token back to base
            	other->x = other->baseX;
            	other->y = other->baseY;
            	other->stepIndex = 0;
            	other->homeStepIndex = 0;
            	other->inPlay = 0;
            	other->inHome = 0;
            }
    	}
    }
}

