#ifndef TYPES_H
#define TYPES_H

#define TOTAL_STEPS 52

#include <SDL.h>

// Player colors
enum {
    RED = 0,
    GREEN = 1,
    BLUE = 2,
    YELLOW = 3
};

// Starting indices for each color in the universal path
static const int startIndex[4] = {
    0,   // Red
    39,  // Green
    13,  // Blue
    26   // Yellow
};

// Token structure
typedef struct Token {
    int x, y;                   // Current position on screen
    int baseX, baseY;           // Original base position
    SDL_Color color;		// Token color
    int stepIndex;              // Index in universal path
    int homeStepIndex;          // Index in home path
    int inPlay;                 // 1 = in play, 0 = in base
    int inHome;                 // 1 = in final home path
    int isBlockade;		// 1 = in a Blockade
    int reachedHome;            // 1 = reached home

    struct Token* partner; 	// Pointer to the other token in the blockade
} Token;

typedef struct {
    char letter;
    int x, y;
} LetterDraw;

// Declare global tokens array (defined elsewhere)
extern Token tokens[4][4];

extern LetterDraw letters[];

// Declare paths (read-only arrays)
extern const SDL_Point universalPath[TOTAL_STEPS];
extern const SDL_Point redHomePath[7];
extern const SDL_Point greenHomePath[7];
extern const SDL_Point blueHomePath[7];
extern const SDL_Point yellowHomePath[7];

// Starting step index for each color
extern const int startIndex[4];

#endif  // TYPES_H
