#ifndef HOLDEM_SCORE_H_
#define HOLDEM_SCORE_H_

#include "libraries.h"

typedef struct Card {
    int rank;
    int color; 
} card_t;

typedef struct Player {
    int money;
    card_t hand[2];
    int moneyBet;
    bool folded;
} player_t;


int* countOccurences(int hand[7]);
int getWinner(player_t* players, card_t communityCards[5], int* winningScore, int nbPlayers);

#endif