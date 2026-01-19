#include "minimax.h"
#include <stdio.h>
#include <stdlib.h>
#include "extapp_api.h"
#define INF 100000
#define VAL_P 10
#define VAL_N 30
#define VAL_B 35
#define VAL_R 50
#define VAL_Q 90
#define VAL_K 900

unsigned int boardConter = 1;

// Piece-Square Tables
const int knightScore[8][8] = {
    { 1, 1, 1, 1, 1, 1, 1, 1 }, { 1, 2, 2, 2, 2, 2, 2, 1 }, { 1, 2, 3, 3, 3, 3, 2, 1 }, { 1, 2, 3, 4, 4, 3, 2, 1 },
    { 1, 2, 3, 4, 4, 3, 2, 1 }, { 1, 2, 3, 3, 3, 3, 2, 1 }, { 1, 2, 2, 2, 2, 2, 2, 1 }, { 1, 1, 1, 1, 1, 1, 1, 1 }
};
const int bishopScore[8][8] = {
    { 4, 3, 2, 1, 1, 2, 3, 4 }, { 3, 4, 3, 2, 2, 3, 4, 3 }, { 2, 3, 4, 3, 3, 4, 3, 2 }, { 1, 2, 3, 4, 4, 3, 2, 1 },
    { 1, 2, 3, 4, 4, 3, 2, 1 }, { 2, 3, 4, 3, 3, 4, 3, 2 }, { 3, 4, 3, 2, 2, 3, 4, 3 }, { 4, 3, 2, 1, 1, 2, 3, 4 }
};
const int queenScore[8][8] = {
    { 1, 1, 1, 3, 1, 1, 1, 1 }, { 1, 2, 3, 3, 3, 1, 1, 1 }, { 1, 4, 3, 3, 3, 4, 2, 1 }, { 1, 2, 3, 3, 3, 2, 2, 1 },
    { 1, 2, 3, 3, 3, 2, 2, 1 }, { 1, 4, 3, 3, 3, 4, 2, 1 }, { 1, 2, 3, 3, 3, 1, 1, 1 }, { 1, 1, 1, 3, 1, 1, 1, 1 }
};
const int rookScore[8][8] = {
    { 4, 3, 4, 4, 4, 4, 3, 4 }, { 4, 4, 4, 4, 4, 4, 4, 4 }, { 1, 1, 2, 3, 3, 2, 1, 1 }, { 1, 2, 3, 4, 4, 3, 2, 1 },
    { 1, 2, 3, 4, 4, 3, 2, 1 }, { 1, 1, 2, 3, 3, 2, 1, 1 }, { 4, 4, 4, 4, 4, 4, 4, 4 }, { 4, 3, 4, 4, 4, 4, 3, 4 }
};
const int whitePawnScore[8][8] = {
    { 8, 8, 8, 8, 8, 8, 8, 8 }, { 8, 8, 8, 8, 8, 8, 8, 8 }, { 5, 6, 6, 7, 7, 6, 6, 5 }, { 2, 3, 3, 5, 5, 3, 3, 2 },
    { 1, 2, 3, 4, 4, 3, 2, 1 }, { 1, 2, 3, 3, 3, 3, 2, 1 }, { 1, 1, 1, 0, 0, 1, 1, 1 }, { 0, 0, 0, 0, 0, 0, 0, 0 }
};
const int blackPawnScore[8][8] = {
    { 0, 0, 0, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0, 0, 1, 1, 1 }, { 1, 2, 3, 3, 3, 3, 2, 1 }, { 1, 2, 3, 4, 4, 3, 2, 1 },
    { 2, 3, 3, 5, 5, 3, 3, 2 }, { 5, 6, 6, 7, 7, 6, 6, 5 }, { 8, 8, 8, 8, 8, 8, 8, 8 }, { 8, 8, 8, 8, 8, 8, 8, 8 }
};

int get_piece_value(char piece, int x, int y)
{
    int base = 0;
    int pos_val = 0;
    int table_y = 7 - y;
    int table_x = x;

    switch (piece) {
    case WHITE_P:
        base = VAL_P;
        pos_val = whitePawnScore[table_y][table_x];
        break;
    case WHITE_N:
        base = VAL_N;
        pos_val = knightScore[table_y][table_x];
        break;
    case WHITE_B:
        base = VAL_B;
        pos_val = bishopScore[table_y][table_x];
        break;
    case WHITE_R:
        base = VAL_R;
        pos_val = rookScore[table_y][table_x];
        break;
    case WHITE_Q:
        base = VAL_Q;
        pos_val = queenScore[table_y][table_x];
        break;
    case WHITE_K:
        base = VAL_K;
        pos_val = 0;
        break;
    case BLACK_P:
        base = VAL_P;
        pos_val = blackPawnScore[table_y][table_x];
        break;
    case BLACK_N:
        base = VAL_N;
        pos_val = knightScore[table_y][table_x];
        break;
    case BLACK_B:
        base = VAL_B;
        pos_val = bishopScore[table_y][table_x];
        break;
    case BLACK_R:
        base = VAL_R;
        pos_val = rookScore[table_y][table_x];
        break;
    case BLACK_Q:
        base = VAL_Q;
        pos_val = queenScore[table_y][table_x];
        break;
    case BLACK_K:
        base = VAL_K;
        pos_val = 0;
        break;
    default:
        return 0;
    }
    return base + pos_val;
}



void colorAdvantage(char board[BOARD_SIZE][BOARD_SIZE], int* whiteAdvantage, int* blackAdvantage)
{
    // Prototypes (defined elsewhere in file)
    extern Move* getMoves(char board[BOARD_SIZE][BOARD_SIZE], int c);
    extern void freeMoves(Move* m);
    extern Move* cloneMove(Move* m); // already used elsewhere
    extern int countMoves(Move* moves);

    // Weights (tune as needed)
    const int W_MATERIAL      = 100;
    const int W_PST           = 1;
    const int W_MOBILITY      = 2;
    const int W_DOUBLED_PAWN  = -6;
    const int W_ISOLATED_PAWN = -8;
    const int W_PASSED_PAWN   = 12;
    const int W_KING_PAWN_SHELL = 3;
    const int W_OPEN_KING_FILE = -10;
    const int W_ENDGAME_KING_CENTRAL = 3;

    int material_w = 0, material_b = 0;
    int pst_w = 0, pst_b = 0;

    int pawnFileCountWhite[8] = {0};
    int pawnFileCountBlack[8] = {0};

    int king_wx = -1, king_wy = -1;
    int king_bx = -1, king_by = -1;

    int nonKingMaterial = 0;

    // Accumulate material + piece-square + pawn file counts
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            char p = board[x][y];
            if (p == EMPTY) continue;
            int val = 0;
            switch (p) {
                case 'p': val = VAL_P; break;
                case 'n': val = VAL_N; break;
                case 'b': val = VAL_B; break;
                case 'r': val = VAL_R; break;
                case 'q': val = VAL_Q; break;
                case 'k': val = VAL_K; break;
                case 'P': val = VAL_P; break;
                case 'N': val = VAL_N; break;
                case 'B': val = VAL_B; break;
                case 'R': val = VAL_R; break;
                case 'Q': val = VAL_Q; break;
                case 'K': val = VAL_K; break;
                default:  val = 0; break;
            }
            int gp = get_piece_value(p, x, y); // includes PST part
            if (p >= 'a' && p <= 'z') { // white
                material_w += val;
                pst_w += (gp - val) * W_PST;
                if (p == 'p') pawnFileCountWhite[x]++;
                if (p == 'k') { king_wx = x; king_wy = y; }
            } else { // black
                material_b += val;
                pst_b += (gp - val) * W_PST;
                if (p == 'P') pawnFileCountBlack[x]++;
                if (p == 'K') { king_bx = x; king_by = y; }
            }
            if (p != 'k' && p != 'K') nonKingMaterial += val;
        }
    }

    // Pawn structure: doubled + isolated + passed
    int pawnStruct_w = 0, pawnStruct_b = 0;
    for (int file = 0; file < 8; file++) {
        if (pawnFileCountWhite[file] > 1)
            pawnStruct_w += W_DOUBLED_PAWN * (pawnFileCountWhite[file] - 1);
        if (pawnFileCountBlack[file] > 1)
            pawnStruct_b += W_DOUBLED_PAWN * (pawnFileCountBlack[file] - 1);
    }

    // Helpers for isolated / passed
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            char p = board[x][y];
            if (p == 'p') {
                // Isolated
                int hasNeighbor =
                    (x > 0 && pawnFileCountWhite[x-1] > 0) ||
                    (x < 7 && pawnFileCountWhite[x+1] > 0);
                if (!hasNeighbor) pawnStruct_w += W_ISOLATED_PAWN;

                // Passed: no black pawn ahead on same/adjacent files
                int passed = 1;
                for (int fx = (x>0?x-1:x); fx <= (x<7?x+1:x); fx++) {
                    for (int ry = y+1; ry < 8; ry++) {
                        if (board[fx][ry] == 'P') { passed = 0; break; }
                    }
                    if (!passed) break;
                }
                if (passed) pawnStruct_w += W_PASSED_PAWN * y;
            } else if (p == 'P') {
                int hasNeighbor =
                    (x > 0 && pawnFileCountBlack[x-1] > 0) ||
                    (x < 7 && pawnFileCountBlack[x+1] > 0);
                if (!hasNeighbor) pawnStruct_b += W_ISOLATED_PAWN;

                int passed = 1;
                for (int fx = (x>0?x-1:x); fx <= (x<7?x+1:x); fx++) {
                    for (int ry = y-1; ry >= 0; ry--) {
                        if (board[fx][ry] == 'p') { passed = 0; break; }
                    }
                    if (!passed) break;
                }
                if (passed) pawnStruct_b += W_PASSED_PAWN * (7 - y);
            }
        }
    }

    // Mobility
    int mobility_w = 0, mobility_b = 0;
    {
        Move* mw = getMoves(board, WHITE);
        Move* mb = getMoves(board, BLACK);
        if (mw && mw->type != 'l' && mw->type != 't')
            mobility_w = countMoves(mw) * W_MOBILITY;
        if (mb && mb->type != 'l' && mb->type != 't')
            mobility_b = countMoves(mb) * W_MOBILITY;
        freeMoves(mw);
        freeMoves(mb);
    }

    // King safety (basic pawn shield + open file penalty)
    int kingSafety_w = 0, kingSafety_b = 0;
    if (king_wx != -1) {
        // Pawns in front (higher y)
        for (int dx = -1; dx <= 1; dx++) {
            int fx = king_wx + dx;
            int fy = king_wy + 1;
            if (fx >= 0 && fx < 8 && fy < 8 && board[fx][fy] == 'p')
                kingSafety_w += W_KING_PAWN_SHELL;
        }
        // Open file penalty: if no pawn on king file
        int hasPawn = 0;
        for (int ry = 0; ry < 8; ry++)
            if (board[king_wx][ry] == 'p') { hasPawn = 1; break; }
        if (!hasPawn) kingSafety_w += W_OPEN_KING_FILE;
    }
    if (king_bx != -1) {
        for (int dx = -1; dx <= 1; dx++) {
            int fx = king_bx + dx;
            int fy = king_by - 1;
            if (fx >= 0 && fx < 8 && fy >= 0 && board[fx][fy] == 'P')
                kingSafety_b += W_KING_PAWN_SHELL;
        }
        int hasPawn = 0;
        for (int ry = 0; ry < 8; ry++)
            if (board[king_bx][ry] == 'P') { hasPawn = 1; break; }
        if (!hasPawn) kingSafety_b += W_OPEN_KING_FILE;
    }

    // Endgame king centralization
    int endgame = (nonKingMaterial <= (VAL_R + VAL_B + VAL_N)); // simple threshold
    int endgame_w = 0, endgame_b = 0;
    if (endgame) {
        if (king_wx != -1)
            endgame_w += W_ENDGAME_KING_CENTRAL * (7 - (abs(king_wx - 3) + abs(king_wy - 3)));
        if (king_bx != -1)
            endgame_b += W_ENDGAME_KING_CENTRAL * (7 - (abs(king_bx - 3) + abs(king_by - 3)));
    }

    int total_w =
        material_w * W_MATERIAL +
        pst_w +
        mobility_w +
        pawnStruct_w +
        kingSafety_w +
        endgame_w;

    int total_b =
        material_b * W_MATERIAL +
        pst_b +
        mobility_b +
        pawnStruct_b +
        kingSafety_b +
        endgame_b;
    
    *whiteAdvantage = total_w;
    *blackAdvantage = total_b;
}

int scoring(int color, char board[BOARD_SIZE][BOARD_SIZE]) {
    int whiteAdvantage = 0;
    int blackAdvantage = 0;
    colorAdvantage(board, &whiteAdvantage, &blackAdvantage);
    int score = whiteAdvantage - blackAdvantage;
    return (color == WHITE) ? score : -score;
}

int countMoves(Move* moves)
{
    int counter = 0;
    if (moves == NULL || moves->type == 'l' || moves->type == 't')
        return 0;
    while (moves != NULL) {
        counter++;
        moves = moves->next;
    }
    return counter;
}

int isPromotionMin(Move* m, int color)
{
    if (color && m->type == 'm') {
        if (m->pos_end.y == 7)
            return 1;
    } else if (!color && m->type == 'M') {
        if (m->pos_end.y == 0)
            return 1;
    }
    return 0;
}

char whichPiece(int i, int color)
{
    if (color) {
        if (i == 0)
            return ('b');
        else if (i == 1)
            return ('n');
        else if (i == 2)
            return ('r');
        else if (i == 3)
            return ('q');
    } else {
        if (i == 0)
            return ('B');
        else if (i == 1)
            return ('N');
        else if (i == 2)
            return ('R');
        else if (i == 3)
            return ('Q');
    }
    return '\0';
}

Move* promotion(int color, Move* moves)
{
    Move* retList = NULL;
    Move* current = NULL;
    for (int i = 0; i < 4; i++) {
        if (retList == NULL) {
            retList = cloneMove(moves);
            if (retList == NULL)
                return NULL;
            retList->type = whichPiece(i, color);
            current = retList;
        } else {
            current->next = cloneMove(moves);
            if (current->next == NULL) {
                free(retList);
                return NULL;
            }
            current = current->next;
            current->type = whichPiece(i, color);
        }
    }
    return retList;
}

int negamax(int depth, char board[BOARD_SIZE][BOARD_SIZE], int color, int alpha, int beta)
{
    boardConter++;

    if (depth == 0) {
        return scoring(color, board);
    }

    Move* moves = getMoves(board, color);
    if (moves == NULL)
        return 0;

    if (moves->type == 'l') { // Checkmate
        freeMoves(moves);
        return -INF + (100 - depth);
    }
    if (moves->type == 't') { // Stalemate
        freeMoves(moves);
        return 0;
    }

    Move* m = moves;
    int bestVal = -INF;

    while (m != NULL) {
        char piece = board[m->pos_start.x][m->pos_start.y];
        char eatpiece = board[m->pos_end.x][m->pos_end.y];

        Move* movesToProcess = m;
        Move* nextMove = m->next;
        Move* promoList = NULL;

        if (isPromotionMin(m, color)) {
            promoList = promotion(color, m);
            movesToProcess = promoList;
        } else {
            m->next = NULL;
        }

        Move* subM = movesToProcess;
        while (subM != NULL) {
            if (subM->type == 'c') {
                makeMove(board, subM->pos_start.x, subM->pos_start.y, subM->pos_end.x, subM->pos_end.y);
                makeMove(board, 4, 7 * (1 - color), 4 - (2 * (left_right(subM->pos_start.x) - 2)), 7 * (1 - color));
            } else {
                makeMove(board, subM->pos_start.x, subM->pos_start.y, subM->pos_end.x, subM->pos_end.y);
                board[subM->pos_end.x][subM->pos_end.y] = subM->type;
            }

            int val = -negamax(depth - 1, board, 1 - color, -beta, -alpha);

            board[subM->pos_start.x][subM->pos_start.y] = piece;
            board[subM->pos_end.x][subM->pos_end.y] = eatpiece;
            if (subM->type == 'c') {
                board[4][7 * (1 - color)] = (color == WHITE) ? 'k' : 'K';
                board[(4) - (2 * (left_right(subM->pos_start.x) - 2))][7 * (1 - color)] = EMPTY;
            }

            if (val > bestVal) {
                bestVal = val;
            }
            if (bestVal > alpha) {
                alpha = bestVal;
            }
            if (alpha >= beta) {
                if (promoList) {
                    // Should free remaining promo moves if any, but we rely on freeMoves(moves) later?
                    // No, promoList is separate.
                    // But we are in a sub-loop.
                }
                break;
            }

            subM = subM->next;
        }

        if (promoList)
            freeMoves(promoList);
        else
            m->next = nextMove;

        if (alpha >= beta)
            break;

        m = nextMove;
    }

    freeMoves(moves);
    return bestVal;
}

Move* shuffleMoves(Move* head)
{
    if (!head || !head->next)
        return head;
    int count = countMoves(head);
    Move** array = (Move**)malloc(count * sizeof(Move*));
    if (!array)
        return head;
    Move* curr = head;
    for (int i = 0; i < count; i++) {
        array[i] = curr;
        curr = curr->next;
    }
    for (int i = count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Move* temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
    head = array[0];
    curr = head;
    for (int i = 1; i < count; i++) {
        curr->next = array[i];
        curr = curr->next;
    }
    curr->next = NULL;
    free(array);
    return head;
}

Move* minimaxManager(char board[BOARD_SIZE][BOARD_SIZE], Move* moves, int color, unsigned int depth, char score)
{
    moves = shuffleMoves(moves);
    boardConter = 1;

    int bestVal = -INF;
    Move* bestMove = NULL;
    int alpha = -INF;
    int beta = INF;

    Move* m = moves;
    while (m != NULL) {
        char piece = board[m->pos_start.x][m->pos_start.y];
        char eatpiece = board[m->pos_end.x][m->pos_end.y];

        Move* movesToProcess = m;
        Move* nextMove = m->next;
        Move* promoList = NULL;

        if (isPromotionMin(m, color)) {
            promoList = promotion(color, m);
            movesToProcess = promoList;
        } else {
            m->next = NULL;
        }

        Move* subM = movesToProcess;
        while (subM != NULL) {
            if (subM->type == 'c') {
                makeMove(board, subM->pos_start.x, subM->pos_start.y, subM->pos_end.x, subM->pos_end.y);
                makeMove(board, 4, 7 * (1 - color), 4 - (2 * (left_right(subM->pos_start.x) - 2)), 7 * (1 - color));
            } else {
                makeMove(board, subM->pos_start.x, subM->pos_start.y, subM->pos_end.x, subM->pos_end.y);
                board[subM->pos_end.x][subM->pos_end.y] = subM->type;
            }

            int val = -negamax(depth - 1, board, 1 - color, -beta, -alpha);

            board[subM->pos_start.x][subM->pos_start.y] = piece;
            board[subM->pos_end.x][subM->pos_end.y] = eatpiece;
            if (subM->type == 'c') {
                board[4][7 * (1 - color)] = (color == WHITE) ? 'k' : 'K';
                board[(4) - (2 * (left_right(subM->pos_start.x) - 2))][7 * (1 - color)] = EMPTY;
            }

            if (val > bestVal) {
                bestVal = val;
                if (bestMove)
                    freeMoves(bestMove);
                bestMove = cloneMove(subM);
                bestMove->next = NULL;
            }
            if (bestVal > alpha) {
                alpha = bestVal;
            }

            subM = subM->next;
        }

        if (promoList)
            freeMoves(promoList);
        else
            m->next = nextMove;

        m = nextMove;
    }

    freeMoves(moves);
    return bestMove;
}
