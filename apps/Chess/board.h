#ifndef BOARD_H
#define BOARD_H

#include "chess_defs.h"

void init_board(char board[BOARD_SIZE][BOARD_SIZE]);
void copyBoard(char board[BOARD_SIZE][BOARD_SIZE], char boardCopy[BOARD_SIZE][BOARD_SIZE]);
void makeMove(char board[BOARD_SIZE][BOARD_SIZE], int x1, int y1, int x2, int y2);
int isWhite(char ch);
int isBlack(char ch);
char whichType(char p);

extern int is_move_king_white;
extern int is_move_king_black;
extern int is_move_right_rook_white;
extern int is_move_left_rook_white;
extern int is_move_right_rook_black;
extern int is_move_left_rook_black;

#endif
