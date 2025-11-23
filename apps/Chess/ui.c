#include "ui.h"

#include <stdio.h>
#include <string.h>

#include "board.h"
#include "extapp_api.h"

// Include assets
#include "assets/bishop.h"
#include "assets/bishop1.h"
#include "assets/king.h"
#include "assets/king1.h"
#include "assets/knight.h"
#include "assets/knight1.h"
#include "assets/pawn.h"
#include "assets/pawn1.h"
#include "assets/queen.h"
#include "assets/queen1.h"
#include "assets/rook.h"
#include "assets/rook1.h"

void draw_piece_to_buffer(uint16_t* buffer, int buffer_width, int buffer_height, char piece)
{
    const uint16_t* piece_data = NULL;
    int width = 0, height = 0;

    switch (piece) {
    case WHITE_P:
        piece_data = pawn_data;
        width = PAWN_WIDTH;
        height = PAWN_HEIGHT;
        break;
    case BLACK_P:
        piece_data = pawn1_data;
        width = PAWN1_WIDTH;
        height = PAWN1_HEIGHT;
        break;
    case WHITE_R:
        piece_data = rook_data;
        width = ROOK_WIDTH;
        height = ROOK_HEIGHT;
        break;
    case BLACK_R:
        piece_data = rook1_data;
        width = ROOK1_WIDTH;
        height = ROOK1_HEIGHT;
        break;
    case WHITE_N:
        piece_data = knight_data;
        width = KNIGHT_WIDTH;
        height = KNIGHT_HEIGHT;
        break;
    case BLACK_N:
        piece_data = knight1_data;
        width = KNIGHT1_WIDTH;
        height = KNIGHT1_HEIGHT;
        break;
    case WHITE_B:
        piece_data = bishop_data;
        width = BISHOP_WIDTH;
        height = BISHOP_HEIGHT;
        break;
    case BLACK_B:
        piece_data = bishop1_data;
        width = BISHOP1_WIDTH;
        height = BISHOP1_HEIGHT;
        break;
    case WHITE_Q:
        piece_data = queen_data;
        width = QUEEN_WIDTH;
        height = QUEEN_HEIGHT;
        break;
    case BLACK_Q:
        piece_data = queen1_data;
        width = QUEEN1_WIDTH;
        height = QUEEN1_HEIGHT;
        break;
    case WHITE_K:
        piece_data = king_data;
        width = KING_WIDTH;
        height = KING_HEIGHT;
        break;
    case BLACK_K:
        piece_data = king1_data;
        width = KING1_WIDTH;
        height = KING1_HEIGHT;
        break;
    default:
        return;
    }

    if (piece_data == NULL)
        return;

    int scaled_width = (int)(width * PIECE_SCALE);
    int scaled_height = (int)(height * PIECE_SCALE);
    int offset_x = (buffer_width - scaled_width) / 2;
    int offset_y = (buffer_height - scaled_height) / 2;

    for (int py = 0; py < scaled_height; py++) {
        for (int px = 0; px < scaled_width; px++) {
            int orig_x = (int)(px / PIECE_SCALE);
            int orig_y = (int)(py / PIECE_SCALE);
            if (orig_x >= width)
                orig_x = width - 1;
            if (orig_y >= height)
                orig_y = height - 1;

            uint16_t color = piece_data[orig_y * width + orig_x];
            if (color != COLOR_TRANSPARENT) {
                int buf_x = offset_x + px;
                int buf_y = offset_y + py;
                if (buf_x >= 0 && buf_x < buffer_width && buf_y >= 0 && buf_y < buffer_height) {
                    buffer[buf_y * buffer_width + buf_x] = color;
                }
            }
        }
    }
}

void draw_move_indicator_to_buffer(uint16_t* buffer, int buffer_width, int buffer_height)
{
    int center_x = buffer_width / 2;
    int center_y = buffer_height / 2;
    int radius = buffer_width / 6;

    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            if (dx * dx + dy * dy <= radius * radius) {
                int x = center_x + dx;
                int y = center_y + dy;
                if (x >= 0 && x < buffer_width && y >= 0 && y < buffer_height) {
                    uint16_t existing_pixel = buffer[y * buffer_width + x];
                    uint16_t r = (existing_pixel >> 11) & 0x1F;
                    uint16_t g = (existing_pixel >> 5) & 0x3F;
                    uint16_t b = existing_pixel & 0x1F;

                    // Apply rgba(0, 0, 0, 0.45) -> multiply by 0.55
                    r = (r * 55) / 100;
                    g = (g * 55) / 100;
                    b = (b * 55) / 100;

                    uint16_t blended = (r << 11) | (g << 5) | b;
                    buffer[y * buffer_width + x] = blended;
                }
            }
        }
    }
}

void draw_square(char board[BOARD_SIZE][BOARD_SIZE], int row, int col, bool selected, bool is_possible_move, bool is_check)
{
    int x = BOARD_X + col * SQUARE_SIZE;
    int y = BOARD_Y + row * SQUARE_SIZE;

    uint16_t buffer[SQUARE_SIZE * SQUARE_SIZE];

    bool is_light = ((row + col) % 2 == 0);
    uint16_t bg_color = is_light ? COLOR_SQUARE_LIGHT : COLOR_SQUARE_DARK;
    uint16_t text_color = is_light ? COLOR_SQUARE_DARK : COLOR_SQUARE_LIGHT;

    if (is_check) {
        // Blend bg_color with COLOR_CHECK (alpha 0.8)
        int r_src = (COLOR_CHECK >> 11) & 0x1F;
        int g_src = (COLOR_CHECK >> 5) & 0x3F;
        int b_src = (COLOR_CHECK) & 0x1F;

        int r_dst = (bg_color >> 11) & 0x1F;
        int g_dst = (bg_color >> 5) & 0x3F;
        int b_dst = (bg_color) & 0x1F;

        int r = (r_src * 8 + r_dst * 2) / 10;
        int g = (g_src * 8 + g_dst * 2) / 10;
        int b = (b_src * 8 + b_dst * 2) / 10;

        bg_color = (r << 11) | (g << 5) | b;
    }

    // Fill background
    for (int i = 0; i < SQUARE_SIZE * SQUARE_SIZE; i++) {
        buffer[i] = bg_color;
    }

    // Draw selection
    if (selected) {
        for (int i = 0; i < SQUARE_SIZE * SQUARE_SIZE; i++) {
            buffer[i] = ((buffer[i] & COLOR_MASK_LSB) >> 1) + ((COLOR_SELECTION_OVERLAY & COLOR_MASK_LSB) >> 1);
        }
        bg_color = ((bg_color & COLOR_MASK_LSB) >> 1) + ((COLOR_SELECTION_OVERLAY & COLOR_MASK_LSB) >> 1);
    }

    // Draw piece
    int board_x = col;
    int board_y = 7 - row;
    if (board[board_x][board_y] != EMPTY) {
        draw_piece_to_buffer(buffer, SQUARE_SIZE, SQUARE_SIZE, board[board_x][board_y]);
    }

    // Draw move indicator
    if (is_possible_move) {
        draw_move_indicator_to_buffer(buffer, SQUARE_SIZE, SQUARE_SIZE);
    }

    // Push buffer
    extapp_pushRect(x, y, SQUARE_SIZE, SQUARE_SIZE, buffer);

    // Draw coordinates
    char buffer_text[2] = { 0, 0 };
    if (col == 0) {
        buffer_text[0] = '8' - row;
        extapp_drawTextSmall(buffer_text, x + 2, y + 2, text_color, bg_color, false);
    }

    if (row == 7) {
        buffer_text[0] = 'a' + col;
        extapp_drawTextSmall(buffer_text, x + SQUARE_SIZE - 9, y + SQUARE_SIZE - 12, text_color, bg_color, false);
    }
}

void draw_cursor_move(char board[BOARD_SIZE][BOARD_SIZE], int old_row, int old_col, int new_row, int new_col,
    Move* possible_moves, int check_row, int check_col)
{
    bool old_is_move = false;
    bool new_is_move = false;

    if (possible_moves != NULL) {
        Move* m = possible_moves;
        while (m != NULL) {
            int r = 7 - m->pos_end.y;
            int c = m->pos_end.x;
            if (r == old_row && c == old_col)
                old_is_move = true;
            if (r == new_row && c == new_col)
                new_is_move = true;
            m = m->next;
        }
    }

    bool old_is_check = (old_row == check_row && old_col == check_col);
    bool new_is_check = (new_row == check_row && new_col == check_col);

    draw_square(board, old_row, old_col, false, old_is_move, old_is_check);
    draw_square(board, new_row, new_col, true, new_is_move, new_is_check);
}

void draw_board(char board[BOARD_SIZE][BOARD_SIZE], int sel_row, int sel_col, bool show_moves, Move* possible_moves, int check_row, int check_col)
{
    bool moves_map[8][8];
    memset(moves_map, 0, sizeof(moves_map));

    if (show_moves && possible_moves != NULL) {
        Move* m = possible_moves;
        while (m != NULL) {
            // Map move coordinates (x,y) to screen coordinates (row,col)
            // x = col
            // y = 7 - row  => row = 7 - y
            int r = 7 - m->pos_end.y;
            int c = m->pos_end.x;
            if (r >= 0 && r < 8 && c >= 0 && c < 8) {
                moves_map[r][c] = true;
            }
            m = m->next;
        }
    }

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            bool is_check = (row == check_row && col == check_col);
            draw_square(board, row, col, (row == sel_row && col == sel_col), moves_map[row][col], is_check);
        }
    }
}

const char* get_piece_name(char piece)
{
    switch (piece) {
    case WHITE_P:
        return "Pawn W";
    case BLACK_P:
        return "Pawn B";
    case WHITE_R:
        return "Rook W";
    case BLACK_R:
        return "Rook B";
    case WHITE_N:
        return "Knight W";
    case BLACK_N:
        return "Knight B";
    case WHITE_B:
        return "Bishop W";
    case BLACK_B:
        return "Bishop B";
    case WHITE_Q:
        return "Queen W";
    case BLACK_Q:
        return "Queen B";
    case WHITE_K:
        return "King W";
    case BLACK_K:
        return "King B";
    default:
        return "";
    }
}

void draw_info_panel(char board[BOARD_SIZE][BOARD_SIZE], int cursor_row, int cursor_col, bool piece_selected,
    int selected_row, int selected_col, bool white_turn, bool is_check, const char* last_move_str)
{
    int x = INFO_PANEL_X;
    int y = 0;
    int width = INFO_PANEL_WIDTH;

    extapp_pushRectUniform(x, y, width, LCD_HEIGHT, COLOR_WHITE);

    int offset_y = 5;
    extapp_drawTextSmall("Turn:", x + 5, offset_y, COLOR_BLACK, COLOR_WHITE, false);
    offset_y += 12;
    if (white_turn) {
        extapp_drawTextSmall("White", x + 5, offset_y, COLOR_BLACK, COLOR_WHITE, false);
    } else {
        extapp_drawTextSmall("Black", x + 5, offset_y, COLOR_BLACK, COLOR_WHITE, false);
    }

    offset_y += 20;
    extapp_drawTextSmall("Position:", x + 5, offset_y, COLOR_BLACK, COLOR_WHITE, false);
    offset_y += 12;
    char pos[4];
    pos[0] = 'a' + cursor_col;
    pos[1] = '8' - cursor_row;
    pos[2] = '\0';
    extapp_drawTextSmall(pos, x + 5, offset_y, COLOR_BLACK, COLOR_WHITE, false);

    if (last_move_str && last_move_str[0] != '\0') {
        offset_y += 20;
        extapp_drawTextSmall("Last Move:", x + 5, offset_y, COLOR_BLACK, COLOR_WHITE, false);
        offset_y += 12;
        extapp_drawTextSmall(last_move_str, x + 5, offset_y, COLOR_BLACK, COLOR_WHITE, false);
    }

    if (piece_selected && selected_row >= 0 && selected_col >= 0) {
        offset_y += 20;
        extapp_drawTextSmall("Selected:", x + 5, offset_y, COLOR_BLACK, COLOR_WHITE, false);
        offset_y += 12;
        // Map selected_row/col to board coords
        int bx = selected_col;
        int by = 7 - selected_row;
        const char* piece_name = get_piece_name(board[bx][by]);
        extapp_drawTextSmall(piece_name, x + 5, offset_y, COLOR_BLACK, COLOR_WHITE, false);
    }
}

static void draw_menu_triangle(int x, int y)
{
    for (int i = 0; i < 8; i++) {
        int y_off = (i * 5) / 8;
        int h = 10 - 2 * y_off;
        if (h > 0) {
            extapp_pushRectUniform(x + i, y + y_off, 1, h, COLOR_TRIANGLE);
        }
    }
}

/// @brief
/// @param index
/// @param text
/// @param selected
/// @param has_sub
/// @param sub_text
static void draw_menu_option(int index, const char* text, bool selected, bool has_sub, const char* sub_text, bool draw_borders)
{
    int x = MENU_MARGIN_SIDE;
    int y = MENU_MARGIN_TOP + index * (MENU_OPTION_HEIGHT - 1);
    int w = MENU_OPTION_WIDTH;
    int h = MENU_OPTION_HEIGHT;

    uint16_t bg_color = selected ? COLOR_OPTION_ACTIVE_BG : COLOR_OPTION_BG;

    // Fill background
    extapp_pushRectUniform(x + 1, y + 1, w - 2, h - 2, bg_color);

    // Draw borders
    if (draw_borders) {
        extapp_pushRectUniform(x, y, w, 1, COLOR_BORDER); // Top
        extapp_pushRectUniform(x, y + h - 1, w, 1, COLOR_BORDER); // Bottom
        extapp_pushRectUniform(x, y, 1, h, COLOR_BORDER); // Left
        extapp_pushRectUniform(x + w - 1, y, 1, h, COLOR_BORDER); // Right
    }

    // Text
    extapp_drawTextLarge(text, x + 11, y + 7, COLOR_BLACK, bg_color, false);

    // Triangle
    if (has_sub) {
        int triangle_x = x + w - 10 - 8;
        draw_menu_triangle(triangle_x, y + 12);

        if (sub_text != NULL) {
            int text_w = extapp_drawTextLarge(sub_text, 0, 0, 0, 0, true);
            int text_x = triangle_x + 8 - text_w;
            extapp_drawTextSmall(sub_text, text_x, y + 11, COLOR_SUBTEXT, bg_color, false);
        }
    }
}

int show_menu(const char* title, const MenuItem* items, int count, int initial_selection)
{
    int selected = initial_selection;
    if (selected < 0)
        selected = 0;
    if (selected >= count)
        selected = count - 1;

    // Wait for key release
    while (extapp_scanKeyboard()) {
        extapp_msleep(10);
    }

    // Initial Draw
    extapp_pushRectUniform(0, 0, LCD_WIDTH, LCD_HEIGHT, COLOR_MENU_BG);

    if (title) {
        extapp_pushRectUniform(0, 0, LCD_WIDTH, 18, COLOR_UPSILON);
        int t_width = strlen(title) * 7;
        extapp_drawTextSmall(title, (LCD_WIDTH - t_width) / 2, 3, COLOR_WHITE, COLOR_UPSILON, false);
    }

    for (int i = 0; i < count; i++) {
        draw_menu_option(i, items[i].text, i == selected, items[i].sub_text != NULL, items[i].sub_text, true);
    }

    uint64_t last_move_time = 0;
    while (true) {
        uint64_t keys = extapp_scanKeyboard();
        uint64_t current_time = extapp_millis();
        int old_selected = selected;
        bool selection_changed = false;

        if (current_time - last_move_time > 150) {
            if (keys & SCANCODE_Up) {
                if (selected > 0) {
                    selected--;
                    selection_changed = true;
                }
            } else if (keys & SCANCODE_Down) {
                if (selected < count - 1) {
                    selected++;
                    selection_changed = true;
                }
            }
        }

        if (keys & SCANCODE_OK || keys & SCANCODE_EXE) {
            return selected;
        } else if (keys & SCANCODE_Home || keys & SCANCODE_Back) {
            return -1;
        }

        if (selection_changed) {
            extapp_waitForVBlank();
            draw_menu_option(old_selected, items[old_selected].text, false, items[old_selected].sub_text != NULL, items[old_selected].sub_text, false);
            draw_menu_option(selected, items[selected].text, true, items[selected].sub_text != NULL, items[selected].sub_text, false);
            last_move_time = current_time;
        }

        extapp_msleep(10);
    }
}
