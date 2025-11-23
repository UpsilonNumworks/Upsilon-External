#ifndef UI_H
#define UI_H

#include "chess_defs.h"
#include <stdbool.h>

typedef struct {
    const char* text;
    const char* sub_text;
} MenuItem;
void showScoreBar(int white_advantage, int black_advantage);
void draw_board(char board[BOARD_SIZE][BOARD_SIZE], int sel_row, int sel_col, bool show_moves, Move* possible_moves, int check_row, int check_col);
void draw_square(char board[BOARD_SIZE][BOARD_SIZE], int row, int col, bool selected, bool is_possible_move, bool is_check);
void draw_cursor_move(char board[BOARD_SIZE][BOARD_SIZE], int old_row, int old_col, int new_row, int new_col, Move* possible_moves, int check_row, int check_col);
void draw_info_panel(char board[BOARD_SIZE][BOARD_SIZE], int cursor_row, int cursor_col, bool piece_selected, int selected_row, int selected_col, bool white_turn, bool is_check, const char* last_move_str);

// Generic menu function that handles the loop and drawing
// Returns the selected index, or -1 if Back/Home was pressed
int show_menu(const char* title, const MenuItem* items, int count, int initial_selection);

#endif
