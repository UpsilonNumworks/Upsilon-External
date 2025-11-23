#include "minimax.h"
#include <stdio.h>
#include <stdlib.h>

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

int scoring(int color, char board[BOARD_SIZE][BOARD_SIZE])
{
    int score_w = 0;
    int score_b = 0;
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            char p = board[x][y];
            if (p == EMPTY)
                continue;
            if (p >= 'a' && p <= 'z') { // White
                score_w += get_piece_value(p, x, y);
            } else { // Black
                score_b += get_piece_value(p, x, y);
            }
        }
    }
    return (color == WHITE) ? (score_w - score_b) : (score_b - score_w);
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
