#include <stdio.h>
#include <SDL_ttf.h>
#include <logic.h>
#include <types.h>
#include <stdlib.h>
#include <stdbool.h>

#define TOTAL_STEPS 52

void init_game() {
    printf("Game initialized!\n");
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

void place_random_question_mark(LetterDraw* questionMark,
                                const LetterDraw* letters,
                                int numLetters,
                                TTF_Font* font)
{
    const int maxAttempts = 100;
    int attempts = 0;

    int qmW = 0, qmH = 0;
    TTF_SizeText(font, "?", &qmW, &qmH);

    while (attempts++ < maxAttempts) {
        int idx = rand() % 52;
        SDL_Point cand = universalPath[idx];

        SDL_Rect qmRect = {cand.x, cand.y, qmW, qmH};

        // Check home paths
        bool inHome = false;
        for (int i = 0; i < 7 && !inHome; i++) {
            if ((cand.x == redHomePath[i].x && cand.y == redHomePath[i].y) ||
                (cand.x == greenHomePath[i].x && cand.y == greenHomePath[i].y) ||
                (cand.x == blueHomePath[i].x && cand.y == blueHomePath[i].y) ||
                (cand.x == yellowHomePath[i].x && cand.y == yellowHomePath[i].y)) {
                inHome = true;
            }
        }
        if (inHome) continue;

        // Check overlap with 'o' or 'x' or 'K' or 'P' or 'B'
        bool overlap = false;
        for (int i = 0; i < numLetters && !overlap; i++) {
            char ch = letters[i].letter;
            if (ch != 'o' && ch != 'x' && ch != 'K' && ch != 'P' && ch != 'B') continue;

            int lw = 0, lh = 0;
            char txt[2] = { ch, '\0' };
            TTF_SizeText(font, txt, &lw, &lh);

            SDL_Rect letRect = {letters[i].x, letters[i].y, lw, lh};
            if (SDL_HasIntersection(&qmRect, &letRect)) {
                overlap = true;
            }
        }
        if (overlap) continue;

        // Valid position found
        questionMark->letter = '?';
        questionMark->x = cand.x;
        questionMark->y = cand.y;
        printf("Placed ? at (%d, %d)\n", cand.x, cand.y);
        return;
    }

    // fallback
    questionMark->letter = '?';
    questionMark->x = universalPath[0].x;
    questionMark->y = universalPath[0].y;
    printf("Fallback: Placed ? at (%d, %d)\n", questionMark->x, questionMark->y);
}

void print_dice_ascii(int value) {
    const char* red = "\033[31m";       // Red
    const char* yellow = "\033[33m";    // Yellow
    const char* reset = "\033[0m";      // Reset

    const char* dice_faces[6] = {
        "+---------+\n"
        "|         |\n"
        "|    o    |\n"
        "|         |\n"
        "+---------+\n",

        "+---------+\n"
        "|  o      |\n"
        "|         |\n"
        "|      o  |\n"
        "+---------+\n",

        "+---------+\n"
        "|  o      |\n"
        "|    o    |\n"
        "|      o  |\n"
        "+---------+\n",

        "+---------+\n"
        "|  o   o  |\n"
        "|         |\n"
        "|  o   o  |\n"
        "+---------+\n",

        "+---------+\n"
        "|  o   o  |\n"
        "|    o    |\n"
        "|  o   o  |\n"
        "+---------+\n",

        "+---------+\n"
        "|  o   o  |\n"
        "|  o   o  |\n"
        "|  o   o  |\n"
        "+---------+\n"
    };

    if (value < 1 || value > 6) {
        printf("Invalid dice value: %d\n", value);
        return;
    }

    const char* face = dice_faces[value - 1];
    const char* ptr = face;

    while (*ptr) {
        if (*ptr == '+') {
            printf("%s+%s", red, reset);
        } else if (*ptr == '-') {
            printf("%s-%s", red, reset);
        } else if (*ptr == '|') {
            printf("%s|%s", red, reset);
        } else if (*ptr == 'o') {
            printf("%so%s", yellow, reset);
        } else {
            putchar(*ptr);
        }
        ptr++;
    }
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

          // If it's a blockade, move both tokens together
          if (token->isBlockade && token->partner != NULL) {
        	Token* partner = token->partner;

        	// Move both tokens recursively, but prevent infinite loops by breaking the link temporarily
        	token->partner = NULL;
        	partner->partner = NULL;

        	move_token(partner, direction, tokens);  // Move partner first
        	move_token(token, direction, tokens);    // Then move this token

        	// Restore blockade link
        	token->partner = partner;
        	partner->partner = token;

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
		    printf("Red Token has reached Home!\n");
                }
                // Yellow
                else if (token->color.r == 255 && token->color.g == 255 && token->color.b == 0 &&
                    token->x == yellowHomePath[7].x && token->y == yellowHomePath[7].y) {
                    token->reachedHome = 1;
		    printf("Yellow Token has reached Home!\n");
                }
                // Green
                else if (token->color.r == 0 && token->color.g == 255 && token->color.b == 0 &&
                    token->x == greenHomePath[7].x && token->y == greenHomePath[7].y) {
                    token->reachedHome = 1;
		    printf("Green Token has reached Home!\n");
                }
                // Blue
                else if (token->color.r == 0 && token->color.g == 0 && token->color.b == 255 &&
                    token->x == blueHomePath[7].x && token->y == blueHomePath[7].y) {
                    token->reachedHome = 1;
		    printf("Blue Token has reached Home!\n");
                }
            }

            break;

        default:
            // No action
            break;
    }

    for (int c = 0; c < 4; c++) {
        for (int t = 0; t < 4; t++) {
            Token* other = &tokens[c][t];

            if (other == token || !other->inPlay || other->inHome)
                continue;

            // 💥 Check collision
            if (other->x == token->x && other->y == token->y) {
                int isOpponent = (other->color.r != token->color.r ||
                                other->color.g != token->color.g ||
                                other->color.b != token->color.b);

                if (isOpponent) {
                    // 🎯 Opponent collision logic
                    if (other->isBlockade) {
                        if (token->isBlockade) {
                            // Blockade vs Blockade – both get sent to base
                            Token* p1 = other->partner;
                            other->x = other->baseX;
                            other->y = other->baseY;
                            other->stepIndex = 0;
                            other->inPlay = 0;
                            other->inHome = 0;
                            other->isBlockade = 0;
                            other->partner = NULL;

                            if (p1) {
                                p1->x = p1->baseX;
                                p1->y = p1->baseY;
                                p1->stepIndex = 0;
                                p1->inPlay = 0;
                                p1->inHome = 0;
                                p1->isBlockade = 0;
                                p1->partner = NULL;
                            }

                            printf("Opponent's blockade broken by your blockade!\n");
                        } else {
                            // Single token cannot capture blockade
                            printf("Cannot break a blockade with a single token!\n");
                            return; // Cancel the move or skip
                        }
                    } else {
                        // Single token gets captured
                        other->x = other->baseX;
                        other->y = other->baseY;
                        other->stepIndex = 0;
                        other->homeStepIndex = 0;
                        other->inPlay = 0;
                        other->inHome = 0;
                        other->isBlockade = 0;
                        other->partner = NULL;

                        printf("Token was sent back to Base!\n");
                    }

                } else {
                    // 🧱 Same-color – form blockade
                    if (!token->isBlockade && !other->isBlockade) {
                        token->isBlockade = 1;
                        other->isBlockade = 1;
                        token->partner = other;
                        other->partner = token;
                        printf("Two tokens formed a blockade!\n");
                    }
                }
            }
        }
    }
}
