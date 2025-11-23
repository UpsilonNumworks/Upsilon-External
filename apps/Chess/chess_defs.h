#ifndef CHESS_DEFS_H
#define CHESS_DEFS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// Board dimensions
#define BOARD_SIZE 8
#define BOARD_X 0
#define BOARD_Y 0
#define SQUARE_SIZE (LCD_HEIGHT / 8)
#define PIECE_SCALE 0.7
#define INFO_PANEL_X (8 * SQUARE_SIZE)
#define INFO_PANEL_WIDTH (LCD_WIDTH - INFO_PANEL_X)

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

// Colors - Base
#define COLOR_BLACK 0x0000 // #000000
#define COLOR_WHITE 0xFFFF // #FFFFFF
#define COLOR_RED 0xF800 // #FF0000
#define COLOR_UPSILON 0x7D19 // #7BA1CD
#define COLOR_TRANSPARENT 0xF81F

// Colors - Board
#define COLOR_SQUARE_LIGHT 0xD6FD // #D0DDEC
#define COLOR_SQUARE_DARK 0x7D19 // #7BA1CD
#define COLOR_MOVE_INDICATOR 0x7BEF
#define COLOR_CHECK 0xEB0A // #EB6150
#define COLOR_SELECTION_OVERLAY 0xFFE0 // #FFFF00
#define COLOR_MASK_LSB 0xF7DE

// Colors - Menu
#define COLOR_MENU_BG 0xFFDF // #FFFAFF
#define COLOR_OPTION_BG 0xFFFF // #FFFEFF
#define COLOR_OPTION_ACTIVE_BG 0xE71C // #E6E2E6
#define COLOR_TRIANGLE 0xFD70 // #FFAD83
#define COLOR_BORDER 0xEF7D // #EEEEEE
#define COLOR_SUBTEXT 0x6B6D // #6A6D6A

// Menu layout
#define MENU_MARGIN_TOP 38
#define MENU_MARGIN_SIDE 15
#define MENU_OPTION_WIDTH (LCD_WIDTH - 2 * MENU_MARGIN_SIDE)
#define MENU_OPTION_HEIGHT 34

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
