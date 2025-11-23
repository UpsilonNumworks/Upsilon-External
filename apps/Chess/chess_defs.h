#ifndef CHESS_DEFS_H
#define CHESS_DEFS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define BOARD_SIZE 8

// Piece definitions (matching Chess-In-C-master)
#define WHITE_P 'm'
#define WHITE_B 'b'
#define WHITE_N 'n'
#define WHITE_R 'r'
#define WHITE_Q 'q'
#define WHITE_K 'k'

#define BLACK_P 'M'
#define BLACK_B 'B'
#define BLACK_N 'N'
#define BLACK_R 'R'
#define BLACK_Q 'Q'
#define BLACK_K 'K'

#define EMPTY ' '

#define WHITE 1
#define BLACK 0

#define COLOR_BLACK 0x0000 // #000000
#define COLOR_WHITE 0xFFFF // #FFFFFF
#define COLOR_RED 0xF800 // #FF0000
#define COLOR_UPSILON 0x7D19 // #7BA1CD
#define COLOR_TRANSPARENT 0xF81F
#define COLOR_SQUARE_LIGHT 0xEF7A // #EBECD0
#define COLOR_SQUARE_DARK 0x74AA // #739552
#define COLOR_MOVE_INDICATOR 0x7BEF

typedef struct Position {
    int x;
    int y;
} Pos;

typedef struct Move {
    char type;
    Pos pos_start;
    Pos pos_end;
    struct Move* next;
} Move;

#endif
