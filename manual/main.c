#define TOTAL_STEPS 52
#define SDL_MAIN_HANDLED

#include <time.h> 
#include <math.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <types.h>
#include <logic.h>

int main() {
    // Initialize random seed
    srand(time(NULL));

    // Initialize SDL subsystems
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

    SDL_Surface* icon = IMG_Load("assets/icon.png");
    if (icon) {
    	SDL_SetWindowIcon(window, icon);
    	SDL_FreeSurface(icon);
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    SDL_FreeSurface(imageSurface);

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
    	SDL_Log("IMG_Init error: %s", IMG_GetError());
    	return 1;
    }

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

    init_game();

    int running = 1;
    SDL_Event e;

    int greenIndex = 0;
    int redIndex = 0;
    int blueIndex = 0;
    int yellowIndex = 0;

    // Now draw all letters, including '?'
    int numLetters = 11;

    LetterDraw questionMark;
    place_random_question_mark(&questionMark, letters, numLetters);

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


            	    // --- Optional: Cycle through tokens of same color ---
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

        for (int i = 0; i < numLetters; i++) {
            draw_letter(renderer, font, letters[i].letter, letters[i].x, letters[i].y, black);
        }

        // Draw '?' separately
        draw_letter(renderer, font, questionMark.letter, questionMark.x, questionMark.y, black);

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


