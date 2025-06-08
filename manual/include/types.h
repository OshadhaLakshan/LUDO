#ifndef TYPES_H
#define TYPES_H

#include <SDL.h>  // or <SDL.h> depending on your setup

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
typedef struct {
    int x, y;                   // Current position on screen
    int baseX, baseY;           // Original base position
    SDL_Color color;
    int stepIndex;              // Index in universal path
    int homeStepIndex;          // Index in home path
    int inPlay;                 // 1 = in play, 0 = in base
    int inHome;                 // 1 = in final home path
    int reachedHome;            // 1 = reached home
} Token;

// Home paths for each color
static const SDL_Point redHomePath[7] = {
    {305, 25}, {305, 65}, {305, 105}, {305, 145},
    {305, 185}, {305, 225}, {305, 265}
};

static const SDL_Point greenHomePath[7] = {
    {25, 305}, {65, 305}, {105, 305}, {145, 305},
    {185, 305}, {225, 305}, {265, 305}
};

static const SDL_Point blueHomePath[7] = {
    {585, 305}, {545, 305}, {505, 305}, {465, 305},
    {425, 305}, {385, 305}, {345, 305}
};

static const SDL_Point yellowHomePath[7] = {
    {305, 585}, {305, 545}, {305, 505}, {305, 465},
    {305, 425}, {305, 385}, {305, 345}
};

// Universal path for all tokens
static const SDL_Point universalPath[52] = {
    {345,  65}, {345, 105}, {345, 145}, {345, 185}, {345, 225}, {385, 265},
    {425, 265}, {465, 265}, {505, 265}, {545, 265}, {585, 265}, {585, 305},
    {585, 345}, {545, 345}, {505, 345}, {465, 345}, {425, 345}, {385, 345},
    {345, 385}, {345, 425}, {345, 465}, {345, 505}, {345, 545}, {345, 585},
    {305, 585}, {265, 585}, {265, 545}, {265, 505}, {265, 465}, {265, 425},
    {265, 385}, {225, 345}, {185, 345}, {145, 345}, {105, 345}, { 65, 345},
    { 25, 345}, { 25, 305}, { 25, 265}, { 65, 265}, {105, 265}, {145, 265},
    {185, 265}, {225, 265}, {265, 225}, {265, 185}, {265, 145}, {265, 105},
    {265,  65}, {265,  25}, {305,  25}, {345,  25}
};

#endif  // TYPES_H
