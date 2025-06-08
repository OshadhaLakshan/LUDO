#include <stdio.h>
#include <SDL_ttf.h>
#include <logic.h>

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
