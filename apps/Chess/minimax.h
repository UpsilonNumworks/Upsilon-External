#ifndef MINIMAX_H
#define MINIMAX_H

#include "board.h"
#include "chess_defs.h"
#include "moves.h"

#define valBest 700

Move* minimaxManager(char board[BOARD_SIZE][BOARD_SIZE], Move* moves, int color, unsigned int depth, char score);

#endif
