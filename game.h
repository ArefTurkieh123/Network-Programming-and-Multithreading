#ifndef GAME_H
#define GAME_H

#include "board.h"

typedef struct {
    Board board;
    int current_player;
    int scoreA;
    int scoreB;
    int claimed_boxes;
} Game;

void init_game(Game *game);
void run_game(Game *game, int sock);
void switch_player(Game *game);
int is_game_over(const Game *game);
void declare_winner(const Game *game);

#endif