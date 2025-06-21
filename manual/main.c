#define TOTAL_STEPS 52
#define SDL_MAIN_HANDLED

#include <time.h> 
#include <math.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <types.h>
#include <logic.h>
#include <stdbool.h>

int tokenStartX[4] = {345, 65, 545, 265};
int tokenStartY[4] = {65, 265, 345, 545};

int main() {
    // Initialize random seed
    time_t seed = time(NULL);
    srand(seed);
    printf("Seed: %d\n", seed % 100);

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

    SDL_Color black = {0, 0, 0, 255};

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
    place_random_question_mark(&questionMark, letters, numLetters, font);

    int currentPlayer = GREEN; // 0: Green, 1: Red, 2: Yellow, 3: Blue
    int diceRoll = 0;
    bool awaitingMove = false;
    bool allowAnotherTurn = false;

    while (running) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Draw background board
        SDL_Rect destRect = {0, 0, imgWidth, imgHeight};
        SDL_RenderCopy(renderer, texture, NULL, &destRect);

        // Draw letters and '?'
        for (int i = 0; i < numLetters; i++)
            draw_letter(renderer, font, letters[i].letter, letters[i].x, letters[i].y, black);

        draw_letter(renderer, font, questionMark.letter, questionMark.x + 10, questionMark.y, black);

        // Draw all tokens
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                draw_token(renderer, tokens[i][j], (i == selectedColor && j == selectedIndex[i]));

        // === AUTOMATE TURN START ===
        if (!awaitingMove) {
            diceRoll = (rand() % 6) + 1;
            printf("\n%s rolled a %d\n", colorNames[currentPlayer], diceRoll);
            print_dice_ascii(diceRoll);

            // Auto-select first in-play token or suggest base move
            bool tokenFound = false;
            for (int i = 0; i < 4; i++) {
                if (tokens[currentPlayer][i].inPlay && !tokens[currentPlayer][i].reachedHome) {
                    selectedIndex[currentPlayer] = i;
                    tokenFound = true;
                    break;
                }
            }
            selectedColor = currentPlayer;

            if (!tokenFound && diceRoll == 6) {
                // Find a token in base
                for (int i = 0; i < 4; i++) {
                    if (!tokens[currentPlayer][i].inPlay) {
                        selectedIndex[currentPlayer] = i;
                        break;
                    }
                }
            }

            awaitingMove = true;
        }

        // === INPUT HANDLING ===
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                running = 0;

            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {

                    case SDLK_TAB: {
                        // Cycle through in-play tokens
                        int count = 0;
                        int next = selectedIndex[selectedColor];
                        do {
                            next = (next + 1) % 4;
                            count++;
                        } while ((!tokens[selectedColor][next].inPlay || tokens[selectedColor][next].reachedHome) && count < 4);

                        if (tokens[selectedColor][next].inPlay && !tokens[selectedColor][next].reachedHome)
                            selectedIndex[selectedColor] = next;
                        break;
                    }

                    case SDLK_RETURN: {
                        Token* token = &tokens[selectedColor][selectedIndex[selectedColor]];

                        if (!token->inPlay && diceRoll == 6) {
                            // Enter from base
                            token->inPlay = 1;
                            token->stepIndex = startIndex[currentPlayer];
                            token->x = tokenStartX[currentPlayer];
                            token->y = tokenStartY[currentPlayer];
                            printf("%s token[%d] entered from base!\n", colorNames[currentPlayer], selectedIndex[selectedColor]);
                            allowAnotherTurn = true;

                        } else if (token->inPlay && !token->reachedHome) {
                            // Move token
                            int moveSteps = token->isBlockade ? diceRoll / 2 : diceRoll;
                            if (moveSteps == 0) moveSteps = 1;
                            move_token(token, moveSteps, tokens);

                            if (token->inHome && token->homeStepIndex == 6) {
                                token->reachedHome = 1;
                                printf("%s token[%d] reached home!\n", colorNames[currentPlayer], selectedIndex[selectedColor]);
                            }

                            allowAnotherTurn = (diceRoll == 6);
                        }

                        awaitingMove = false;

                        if (!allowAnotherTurn) {
                            // Rotate to next player
                            switch (currentPlayer) {
                                case GREEN: currentPlayer = RED; break;
                                case RED: currentPlayer = BLUE; break;
                                case BLUE: currentPlayer = YELLOW; break;
                                case YELLOW: currentPlayer = GREEN; break;
                            }
                        }

                        break;
                    }
                }
            }
        }

        SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // Set white background
	SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, texture, NULL, &destRect);

	SDL_Color black = {0, 0, 0, 255};  // Custom color

        for (int i = 0; i < numLetters; i++) {
            draw_letter(renderer, font, letters[i].letter, letters[i].x, letters[i].y, black);
        }

        // Draw '?' separately
        draw_letter(renderer, font, questionMark.letter, questionMark.x + 10, questionMark.y, black);

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


