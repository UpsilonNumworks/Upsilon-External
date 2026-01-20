#include "board.h"
#include <string.h>

void init_board(char board[BOARD_SIZE][BOARD_SIZE])
{
    // Clear board
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            board[x][y] = EMPTY;
        }
    }

    // White pieces (y=0, y=1)
    board[0][0] = WHITE_R;
    board[1][0] = WHITE_N;
    board[2][0] = WHITE_B;
    board[3][0] = WHITE_Q;
    board[4][0] = WHITE_K;
    board[5][0] = WHITE_B;
    board[6][0] = WHITE_N;
    board[7][0] = WHITE_R;

    for (int x = 0; x < BOARD_SIZE; x++) {
        board[x][1] = WHITE_P;
    }

    // Black pieces (y=7, y=6)
    board[0][7] = BLACK_R;
    board[1][7] = BLACK_N;
    board[2][7] = BLACK_B;
    board[3][7] = BLACK_Q;
    board[4][7] = BLACK_K;
    board[5][7] = BLACK_B;
    board[6][7] = BLACK_N;
    board[7][7] = BLACK_R;

    for (int x = 0; x < BOARD_SIZE; x++) {
        board[x][6] = BLACK_P;
    }
}

void copyBoard(char board[BOARD_SIZE][BOARD_SIZE], char boardCopy[BOARD_SIZE][BOARD_SIZE])
{
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            boardCopy[x][y] = board[x][y];
        }
    }
}

void makeMove(char board[BOARD_SIZE][BOARD_SIZE], int x1, int y1, int x2, int y2)
{
    board[x2][y2] = board[x1][y1];
    board[x1][y1] = EMPTY;
}

int isWhite(char ch)
{
    switch (ch) {
    case WHITE_B:
    case WHITE_K:
    case WHITE_Q:
    case WHITE_N:
    case WHITE_R:
    case WHITE_P:
        return 1;
    default:
        return 0;
    }
}

int isBlack(char ch)
{
    switch (ch) {
    case BLACK_B:
    case BLACK_K:
    case BLACK_Q:
    case BLACK_N:
    case BLACK_R:
    case BLACK_P:
        return 1;
    default:
        return 0;
    }
}

char whichType(char p)
{
    return p; // Placeholder if needed, or just return the char
}

int is_move_king_white = 0;
int is_move_king_black = 0;
int is_move_right_rook_white = 0;
int is_move_left_rook_white = 0;
int is_move_right_rook_black = 0;
int is_move_left_rook_black = 0;
