#ifndef GAME_H
#define GAME_H

#include <SDL_ttf.h>
#include <types.h>

void init_game();

// Declare the function before main
void draw_token(SDL_Renderer* renderer, Token token, int isSelected);
void move_token(Token* token, int direction, Token tokens[4][4]);
void draw_letter(SDL_Renderer* renderer, TTF_Font* font, char letter, int x, int y, SDL_Color color);
void place_random_question_mark(LetterDraw* questionMark, const LetterDraw* letters, int numLetters, TTF_Font* font);
void print_dice_ascii(int value);

#endif
