
#include "./inc/score.h"
#include <assert.h>



int* mallocZeros(int size) {
    int* arr = malloc(size * sizeof(int));
    assert(arr != NULL);
    for (int i = 0; i < size; i++) {
        arr[i] = 0;
    }
    return arr;
}
int* countOccurencesValues(card_t hand[7]) {
    int* count = mallocZeros(13);
    for (int i = 0; i < 7; i++) {
        count[hand[i].rank]++;
    }
    return count;
}

int* countOccurencesColors(card_t hand[7]) {
    int* count = mallocZeros(4);
    for (int i = 0; i < 7; i++) {
        count[hand[i].color]++;
    }
    return count;
}


int findStraight(int* occ) {
    for (int i = 12; i >= 4; i--) {
        if (occ[i] >= 1 && occ[i - 1] >= 1 && occ[i - 2] >= 1 && occ[i - 3] >= 1 && occ[i - 4] >= 1) {
            return i;
        }
    }
    return -1;
}

bool isStraightFLush(int index, card_t hand[7], int color) { 
    int sameColor = 0;
    for (int i = 0; i < 7; i++) {
        if (index - 3 <= hand[i].rank && hand[i].rank <= index) {
            if (hand[i].color != color) {
                return false;
            }
        }
    }
    return true;
}

int findHighestCardInFlush(int color, card_t hand[7]) {
    for (int i = 6; i >= 0; i--) {
        if (hand[i].color == color) {
            return hand[i].rank;
        }
    }
}

int max(int a, int b) {
    return (a < b) ? b : a;
}


int getHandScore(player_t player, card_t communityCards[5]) {
    
    card_t* fullHand = malloc(7 * sizeof(card_t));
    assert(fullHand != NULL);

    for (int i = 0; i < 2; i++) {
        fullHand[i] = player.hand[i];
    }
    for (int i = 2; i < 7; i++) {
        fullHand[i] = communityCards[i - 2];
    }
    int* occVal = countOccurencesValues(fullHand);
    int* occCol = countOccurencesColors(fullHand);

    int pairs = 0, threes = 0, fours = 0;
    int flush = 0;
    int straightFlush = 0;
    int straight = 0;
    int royalFlush = 0;
    
    int flushColor;
    int accompanyingCard = -1;

    int highCards[10] =  {0}; 
    for (int i = 0; i < 13; i++) {
        if (occVal[i] == 4) { 
            fours++;
            highCards[7] = i;

        }
        else if (occVal[i] == 3) { 
            threes++;
            highCards[3] = i;
        }
        else if (occVal[i] == 2) { 
            pairs++;
            highCards[2] = i;
            highCards[1] = i;
        }

        
        if (i <= 3 && occCol[i] == 5) {
            flush++;
            highCards[5] = findHighestCardInFlush(i, fullHand);
            flushColor = i;
        }


    }
    
    highCards[0] = max(player.hand[0].rank, player.hand[1].rank);
    int straightIndex = findStraight(occVal);
    if (straightIndex != -1) {
        straight++;
        highCards[4] = straightIndex;
        if (flush == 1 && isStraightFLush(straightIndex, fullHand, flushColor)) {
            straightFlush++;
            highCards[8] = straightIndex;
            if (straightIndex == 12) {
                royalFlush++;
                highCards[9] = straightIndex;
            }

        }
    }
    free(occVal);
    free(fullHand);
    free(occCol);

    if (royalFlush >= 1) return 10000 + highCards[9];
    if (straightFlush >= 1) return 9000 + highCards[8];
    if (fours >= 1) return 8000 + highCards[7]; 
    if (threes >= 1 && pairs >= 1) return 7000 + highCards[3];
    if (flush >= 1) return 6000 + highCards[5];
    if (straight >= 1) return 5000 + highCards[4];
    if (threes >= 1) return 4000 + highCards[3]; 
    if (pairs >= 2) return 3000 + highCards[2]; 
    if (pairs == 1) return 2000 + highCards[1]; 
    
    return 1000 + highCards[0];
}




int getWinner(player_t* players, card_t communityCards[5], int* winningScore, int nbPlayers) {
    
    int bestScore = -1;
    int winnerIndex = -1;
    for (int i = 0; i < nbPlayers; i++) {
        if (!players[i].folded) {
            int score = getHandScore(players[i], communityCards);
            if (score > bestScore) {
                bestScore = score;
                winnerIndex = i;
            }
        }
    }
    *winningScore = bestScore;
    return winnerIndex;
}
