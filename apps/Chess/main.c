#include "board.h"
#include "chess_defs.h"
#include "extapp_api.h"
#include "minimax.h"
#include "moves.h"
#include "ui.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    MODE_PVP,
    MODE_PVAI
} GameMode;
GameMode game_mode = MODE_PVP;
int ai_difficulty = 1; // 0=Easy, 1=Normal, 2=Hard
char board[BOARD_SIZE][BOARD_SIZE];
bool white_turn = true;
Move* current_possible_moves = NULL;

void waitForKeyPressed()
{
    while (!extapp_scanKeyboard()) {
        extapp_msleep(10);
    }
}

void waitForKeyReleased()
{
    while (extapp_scanKeyboard()) {
        extapp_msleep(10);
    }
}

void extapp_main()
{
    waitForKeyReleased();

    while (true) {
        int cursor_row = 4;
        int cursor_col = 4;
        bool piece_selected = false;
        int selected_row = -1;
        int selected_col = -1;

        // Menu Loop
        int menu_option = 0;
        bool menu_active = true;

        srand(extapp_millis()); // Seed random number generator

        while (menu_active) {
            const char* diff_text = "Normal";
            if (ai_difficulty == 0)
                diff_text = "Easy";
            else if (ai_difficulty == 2)
                diff_text = "Hard";

            MenuItem main_items[] = {
                { "Player vs Player", NULL },
                { "Player vs AI", NULL },
                { "AI difficulty", diff_text }
            };

            int choice = show_menu("CHESS", main_items, 3, menu_option);

            if (choice == -1) {
                return;
            }

            menu_option = choice;

            if (choice == 0) {
                game_mode = MODE_PVP;
                menu_active = false;
            } else if (choice == 1) {
                game_mode = MODE_PVAI;
                menu_active = false;
            } else if (choice == 2) {
                MenuItem diff_items[] = {
                    { "Easy", NULL },
                    { "Normal", NULL },
                    { "Hard", NULL }
                };
                int diff = show_menu("CHESS", diff_items, 3, ai_difficulty);
                if (diff != -1) {
                    ai_difficulty = diff;
                }
            }
        }

        init_board(board);
        white_turn = true;
        char last_move_str[16] = "";

        waitForKeyReleased();

        extapp_pushRectUniform(0, 0, LCD_WIDTH, LCD_HEIGHT, COLOR_WHITE);

        int check_row = -1;
        int check_col = -1;
        if (isCheck(board, white_turn ? WHITE : BLACK) == 1) {
            Pos kPos = kingPosition(board, white_turn ? WHITE : BLACK);
            if (kPos.x != -1) {
                check_col = kPos.x;
                check_row = 7 - kPos.y;
            }
        }

        draw_board(board, cursor_row, cursor_col, false, NULL, check_row, check_col);
        draw_info_panel(board, cursor_row, cursor_col, false, -1, -1, white_turn, (check_row != -1), last_move_str);

        bool back_to_menu = false;
        uint64_t last_cursor_move_time = 0;
        int white_advantage = 0;
        int black_advantage = 0;
        while (true) {
            
            colorAdvantage(board, &white_advantage, &black_advantage);
            showScoreBar(white_advantage, black_advantage);
            
            // AI Turn
            if (game_mode == MODE_PVAI && !white_turn) {
                extapp_msleep(100); // Small delay

                // Generate moves for AI (Black)
                Move* ai_moves = getMoves(board, BLACK);
                if (ai_moves == NULL || ai_moves->type == 'l' || ai_moves->type == 't') {
                    // Game Over
                    if (ai_moves)
                        freeMoves(ai_moves);
                    extapp_drawTextLarge("GAME OVER", 100, 100, COLOR_RED, COLOR_WHITE, false);
                    waitForKeyReleased();
                    waitForKeyPressed();
                    back_to_menu = true;
                    break;
                }

                // Run Minimax avec difficulté variable
                // 0=Easy (depth 1), 1=Normal (depth 2), 2=Hard (depth 3)
                int depth =  ai_difficulty + 1;
                Move* best_move = minimaxManager(board, ai_moves, BLACK, depth, '\0');

                if (best_move != NULL) {
                    // Apply move
                    makeMove(board, best_move->pos_start.x, best_move->pos_start.y, best_move->pos_end.x, best_move->pos_end.y);
                    snprintf(last_move_str, sizeof(last_move_str), "%c%d -> %c%d", 'a' + best_move->pos_start.x, best_move->pos_start.y + 1, 'a' + best_move->pos_end.x, best_move->pos_end.y + 1);
                    // Handle promotion (auto queen for AI for now)
                    if (best_move->type != 'c' && best_move->type != 'a' && best_move->type != 'l' && best_move->type != 't') {
                        if (board[best_move->pos_end.x][best_move->pos_end.y] == BLACK_P && best_move->pos_end.y == 0) {
                            board[best_move->pos_end.x][best_move->pos_end.y] = BLACK_Q;
                        }
                    }

                    freeMoves(best_move);
                    white_turn = !white_turn;
                } else {
                    // No move found?
                }

                check_row = -1;
                check_col = -1;
                if (isCheck(board, white_turn ? WHITE : BLACK) == 1) {
                    Pos kPos = kingPosition(board, white_turn ? WHITE : BLACK);
                    if (kPos.x != -1) {
                        check_col = kPos.x;
                        check_row = 7 - kPos.y;
                    }
                }

                draw_board(board, cursor_row, cursor_col, false, NULL, check_row, check_col);
                draw_info_panel(board, cursor_row, cursor_col, false, -1, -1, white_turn, (check_row != -1), last_move_str);
                continue;
            }

            uint64_t keys = extapp_scanKeyboard();
            uint64_t current_time = extapp_millis();
            if (keys & SCANCODE_Home) {
                return;
            }
            if (keys & SCANCODE_Back) {
                break;
            }

            // OK key to select/move
            if (keys & SCANCODE_OK || keys & SCANCODE_EXE) {
                if (!piece_selected) {
                    // Select a piece
                    int bx = cursor_col;
                    int by = 7 - cursor_row;
                    char piece = board[bx][by];

                    if (piece != EMPTY) {
                        bool is_white_piece = isWhite(piece);
                        if ((white_turn && is_white_piece) || (!white_turn && !is_white_piece)) {
                            piece_selected = true;
                            selected_row = cursor_row;
                            selected_col = cursor_col;

                            // Calculate possible moves for this piece
                            if (current_possible_moves)
                                freeMoves(current_possible_moves);

                            Move* all_moves = getMoves(board, white_turn ? WHITE : BLACK);

                            // Filter moves starting from (bx, by)
                            Move* filtered_moves = NULL;
                            Move* m = all_moves;
                            while (m != NULL) {
                                if (m->pos_start.x == bx && m->pos_start.y == by) {
                                    Move* new_node = cloneMove(m);
                                    new_node->next = filtered_moves;
                                    filtered_moves = new_node;
                                }
                                m = m->next;
                            }
                            freeMoves(all_moves);
                            current_possible_moves = filtered_moves;

                            // Optimization: Only redraw the selected square and possible moves
                            // Redraw selected square
                            draw_square(board, cursor_row, cursor_col, true, false, (cursor_row == check_row && cursor_col == check_col));

                            // Redraw possible moves
                            Move* pm = current_possible_moves;
                            while (pm != NULL) {
                                int r = 7 - pm->pos_end.y;
                                int c = pm->pos_end.x;
                                if (r != cursor_row || c != cursor_col) {
                                    draw_square(board, r, c, false, true, (r == check_row && c == check_col));
                                }
                                pm = pm->next;
                            }

                            draw_info_panel(board, cursor_row, cursor_col, piece_selected, selected_row, selected_col, white_turn, (check_row != -1), last_move_str);
                            waitForKeyReleased();
                        }
                    }
                } else {
                    // Move or deselect
                    // Check if cursor pos is in possible moves
                    int target_x = cursor_col;
                    int target_y = 7 - cursor_row;

                    bool valid_move = false;
                    Move* m = current_possible_moves;
                    Move* selected_move = NULL;

                    while (m != NULL) {
                        if (m->pos_end.x == target_x && m->pos_end.y == target_y) {
                            valid_move = true;
                            selected_move = m;
                            break;
                        }
                        m = m->next;
                    }

                    if (valid_move) {
                        // Perform the move
                        makeMove(board, selected_move->pos_start.x, selected_move->pos_start.y, selected_move->pos_end.x, selected_move->pos_end.y);
                        snprintf(last_move_str, sizeof(last_move_str), "%c%d -> %c%d", 'a' + selected_move->pos_start.x, selected_move->pos_start.y + 1, 'a' + selected_move->pos_end.x, selected_move->pos_end.y + 1);

                        // Handle promotion (ask user? default to Queen for now)
                        if (board[target_x][target_y] == WHITE_P && target_y == 7) {
                            board[target_x][target_y] = WHITE_Q;
                        } else if (board[target_x][target_y] == BLACK_P && target_y == 0) {
                            board[target_x][target_y] = BLACK_Q;
                        }

                        white_turn = !white_turn; // Switch turn
                        piece_selected = false;
                        selected_row = -1;
                        selected_col = -1;
                        if (current_possible_moves) {
                            freeMoves(current_possible_moves);
                            current_possible_moves = NULL;
                        }

                        check_row = -1;
                        check_col = -1;
                        if (isCheck(board, white_turn ? WHITE : BLACK) == 1) {
                            Pos kPos = kingPosition(board, white_turn ? WHITE : BLACK);
                            if (kPos.x != -1) {
                                check_col = kPos.x;
                                check_row = 7 - kPos.y;
                            }
                        }

                        draw_board(board, cursor_row, cursor_col, false, NULL, check_row, check_col);
                        draw_info_panel(board, cursor_row, cursor_col, false, -1, -1, white_turn, (check_row != -1), last_move_str);
                    } else {
                        // Deselect
                        piece_selected = false;
                        selected_row = -1;
                        selected_col = -1;
                        if (current_possible_moves) {
                            // Optimization: Clear move indicators
                            Move* pm = current_possible_moves;
                            while (pm != NULL) {
                                int r = 7 - pm->pos_end.y;
                                int c = pm->pos_end.x;
                                if (r != cursor_row || c != cursor_col) {
                                    draw_square(board, r, c, false, false, (r == check_row && c == check_col));
                                }
                                pm = pm->next;
                            }
                            freeMoves(current_possible_moves);
                            current_possible_moves = NULL;
                        }
                        
                        // Redraw cursor square
                        draw_square(board, cursor_row, cursor_col, true, false, (cursor_row == check_row && cursor_col == check_col));

                        draw_info_panel(board, cursor_row, cursor_col, false, -1, -1, white_turn, (check_row != -1), last_move_str);
                    }
                    waitForKeyReleased();
                }
            }

            int new_row = cursor_row;
            int new_col = cursor_col;

            if (current_time - last_cursor_move_time > 150) {
                if (keys & SCANCODE_Up) {
                    new_row--;
                }
                if (keys & SCANCODE_Down) {
                    new_row++;
                }
                if (keys & SCANCODE_Left) {
                    new_col--;
                }
                if (keys & SCANCODE_Right) {
                    new_col++;
                }
            }

            if (new_row < 0)
                new_row = 0;
            if (new_row > 7)
                new_row = 7;
            if (new_col < 0)
                new_col = 0;
            if (new_col > 7)
                new_col = 7;

            if (new_row != cursor_row || new_col != cursor_col) {
                draw_cursor_move(board, cursor_row, cursor_col, new_row, new_col, current_possible_moves, check_row, check_col);
                cursor_row = new_row;
                cursor_col = new_col;
                draw_info_panel(board, cursor_row, cursor_col, piece_selected, selected_row, selected_col, white_turn, (check_row != -1), last_move_str);
                last_cursor_move_time = current_time;
            }

            extapp_msleep(10);
        }

        if (current_possible_moves) {
            freeMoves(current_possible_moves);
            current_possible_moves = NULL;
        }
    }

    return;
}