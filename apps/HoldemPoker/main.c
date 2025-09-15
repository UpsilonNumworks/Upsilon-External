#define SMALL_SCALE 0.90
#define RAISE_AMOUNT 10
#define MAX_STR_LEN 10

#define BLACK_AMOUNT 500
#define RED_AMOUNT 100
#define GREEN_AMOUNT 50
#define BLUE_AMOUNT 10



#include "./inc/libraries.h"
#include "./inc/peripherals.h"
#include "./inc/render.h"
#include "./inc/math_functions.h"
#include "./inc/score.h"

#include "./inc/textures/C-atlas.h"
#include "./inc/textures/D-atlas.h"
#include "./inc/textures/H-atlas.h"
#include "./inc/textures/S-atlas.h"

#include "./inc/textures/card-back.h"
#include "./inc/textures/card-back-med.h"
#include "./inc/textures/card-deck.h"
#include "./inc/textures/card-flip.h"

#include "./inc/textures/blue-tokens.h"
#include "./inc/textures/red-tokens.h"
#include "./inc/textures/green-tokens.h"
#include "./inc/textures/black-tokens.h"


int randint(int a, int b) {
    return rand() % (b - a + 1) + a;
}

int sumArray(int* array, int size) {
    int sum = 0;
    for (int i = 0; i < size; i++) {
        sum += array[i];
    }
    return sum;
}

bool isKeydown(uint64_t key) {
    return extapp_scanKeyboard() & key;
}

void drawCard(int x, int y, obm_t img, int index) {
    if (index < 0 || index >= (img.height / CARD_HEIGHT)) {
        return; 
    }
    drawImageCropped(x, y, img, 0, index * CARD_HEIGHT, img.width, CARD_HEIGHT);
}

void drawCardRotated(int x, int y, obm_t img, int index ,double angle) {
    if (index < 0 || index >= (img.height / CARD_HEIGHT)) {
        return; 
    }
    drawImageCroppedRotated(x, y, img, 0, index * CARD_HEIGHT, img.width, CARD_HEIGHT, angle);
}


void drawCardScaled(int x, int y, obm_t img, int index, float scale) {
    if (index < 0 || index >= (img.height / CARD_HEIGHT)) {
        return; 
    }
    drawStretchedImageCropped(x, y, img.width * scale, CARD_HEIGHT * scale, img, 0, index * CARD_HEIGHT, img.width, CARD_HEIGHT);
}

void drawBettingPot(int bettingPot) {
    char potStr[20];
    snprintf(potStr, sizeof(potStr), "Pot: $%d", bettingPot);
    extapp_drawTextLarge(potStr, SCREEN_WIDTH / 2 - 50, 30, 0x0000, 0xFFFF, false);
}

double getHandAngle(int index) {
    if (index == 0) {
        return -CARD_INCLINE; 
    } else if (index == 1) {
        return CARD_INCLINE;  
    }
    return 0.0; 
}

static inline card_t indexToCard(int idx) {
    card_t c;
    c.rank = idx % 13;
    c.color = idx / 13;
    return c;
}

void showHand(card_t playerHand[2]) {
    int x = SCREEN_WIDTH / 2;
    int y = SCREEN_HEIGHT - CARD_HEIGHT * 0.6;
    int offset = 5;
    for (int i = 0; i < 2; i++) {
        double angle = getHandAngle(i);
        switch (playerHand[i].color) {
            case 0:
                drawCardRotated(x + (i - 1) * (CARD_WIDTH + offset), y, H_atlas, playerHand[i].rank, angle);
                break;
            case 1:
                drawCardRotated(x + (i - 1) * (CARD_WIDTH + offset), y, D_atlas, playerHand[i].rank, angle);
                break;
            case 2:
                drawCardRotated(x + (i - 1) * (CARD_WIDTH + offset), y, S_atlas, playerHand[i].rank, angle);
                break;
            case 3:
                drawCardRotated(x + (i - 1) * (CARD_WIDTH + offset), y, C_atlas, playerHand[i].rank, angle);
                break;
            default:
                drawCardRotated(x + (i - 1) * (CARD_WIDTH + offset), y, H_atlas, playerHand[i].rank, angle);
                break;
        }
    }
}

void bet(player_t* player, int* bettingPot, int amount) { 
    int placed = (amount > player->money) ? player->money : amount;
    *bettingPot += placed;
    player->money -= placed;
    player->moneyBet += placed;
}


void drawPlayerMoney(player_t player, int x, int y) {
    char moneyStr[20];
    snprintf(moneyStr, sizeof(moneyStr), "Money: $%d", player.money);
    extapp_drawTextLarge(moneyStr, x, y, 0x0000, 0xFFFF, false);
}

void drawPlayerActionMenu(int action, int mode, int currentBet) {
        int n;
        char** actions;
        int displacement[3] = {70, 70, 70};
        if (mode == 1) {
            n = 2;
            actions = malloc(n * sizeof(char*));
            assert(actions != NULL);
            actions[0] = "Small Blind";
            actions[1] = "Fold";
            displacement[0] = 120;
        } else if (mode == 2) {
            n = 2;
            actions = malloc(n * sizeof(char*));
            assert(actions != NULL);
            
            actions[0] = "Big Blind";
            actions[1] = "Fold";
            displacement[0] = 100;
            
        } else {
            n = 3;
            actions = malloc(n * sizeof(char*));
            assert(actions != NULL);
            if (currentBet == 0) {
                actions[0] = "Check";
            } else {
                actions[0] = "Call ";
            }
            actions[1] = "Raise";
            actions[2] = "Fold";
            
        }
    int x = (SCREEN_WIDTH - sumArray(displacement, n)) / 2;
    int y = 5;
    for (int i = 0; i < n; i++) {
        uint16_t color = (i == action) ? 0xF800 : 0x0000; // Highlight selected action in red
        extapp_drawTextLarge(actions[i], x + sumArray(displacement, i), y, color, 0xFFFF, false);
    }
    free(actions);
}




void drawToken(obm_t tokenImg, int x, int y, int index) {
    drawImageCropped(x, y, tokenImg, 0,  + index * TOK_HEIGHT, tokenImg.width, TOK_HEIGHT);

}


void drawRaiseMenu(int raiseAmount) {
    srand(1553288);
    char raiseStr[32];
    snprintf(raiseStr, sizeof(raiseStr), "$%d  ", raiseAmount);

    int x = 0;
    int y = SCREEN_HEIGHT - 20;
    int colX = x;

    extapp_drawTextLarge(raiseStr, x, y, 0x0000, 0xFFFF, false);
    int tokens[4]; 

    tokens[0] = raiseAmount / BLACK_AMOUNT; // Black tokens
    raiseAmount %= BLACK_AMOUNT;
    tokens[1] = raiseAmount / GREEN_AMOUNT; // Green tokens
    raiseAmount %= GREEN_AMOUNT;
    tokens[2] = raiseAmount / RED_AMOUNT;   // Red tokens
    raiseAmount %= RED_AMOUNT;
    tokens[3] = raiseAmount / BLUE_AMOUNT;  // Blue tokens
    raiseAmount %= BLUE_AMOUNT;

    int colMax[3] = {10, 4, 9};
    int maxAmount = sumArray(colMax, 3);
    int col = 0;
    int drawnTokens = 0;
    int tokenY = y - 20;
    int tokenYOg = tokenY;
    int delta = black_tokens.width + 2;

    for (int i = 0; i < 5; i++) {
        obm_t tokenImg;
        switch (i) {
            case 0:
                tokenImg = black_tokens;
                break;
            case 1:
                tokenImg = green_tokens;   
                break;
            case 2:
                tokenImg = red_tokens;
                break;
            case 3: 
                tokenImg = blue_tokens;
                break;
            default:
                tokenImg = blue_tokens;
                break;

        }
        if (i < 4 ) {
            for (int j = 0; j < tokens[i]; j++) {
                
                drawToken(tokenImg, colX + randint(-1,1), tokenY, j % 4);
                tokenY -= TOK_HEIGHT;
                drawnTokens++;
                if (drawnTokens == colMax[col]) {
                    drawnTokens = 0;
                    col++;
                    colX += delta;
                    tokenY = tokenYOg;
                }
                if (col >= 3) {
                    return;
                }
            }
        } else {
            for (int j = 0; j < maxAmount - sumArray(tokens, 4); j++) {
                extapp_pushRectUniform(colX - 1, tokenY, tokenImg.width + 2, TOK_HEIGHT, 0xFFFF);
                tokenY -= TOK_HEIGHT;
                drawnTokens++;
                if (drawnTokens == colMax[col]) {
                    drawnTokens = 0;
                    col++;
                    colX += delta;
                    tokenY = tokenYOg;
                }
                if (col >= 3) {
                    return;
                }
            }
        }
    }
    
    
    
}

int selectPlayerAction(player_t* player, int* raiseAmount, int* action, int mode, int currentBet) { 
    bool returnPressed = false;
    *action = 0;
    *raiseAmount = 0;
    bool buttonPressed = false;
    
    int minMenu, maxMenu;

    switch (mode) {
        case 0: 
            minMenu = 0;
            maxMenu = 2;
            break;
        case 1:
            minMenu = 0;
            maxMenu = 1;
            break;
        case 2:
            minMenu = 0;
            maxMenu = 1;
            break;
        default:
            minMenu = 0;
            maxMenu = 2;
            break;
    }

    while (!returnPressed) {
        drawPlayerActionMenu(*action, mode, currentBet);
        drawRaiseMenu(*raiseAmount + currentBet);
        if (isKeydown(SCANCODE_Back)) {
            return -1; // Exit Code
        }
        if (isKeydown(SCANCODE_Left) && *action > minMenu && !buttonPressed) {
            (*action)--;
            buttonPressed = true;
        }
        else if (isKeydown(SCANCODE_Right) && *action < maxMenu && !buttonPressed) {
            (*action)++;
            buttonPressed = true;
        }
        if (isKeydown(SCANCODE_OK)) {
            returnPressed = true;
            buttonPressed = true;
        }
        if (!isKeydown(SCANCODE_Left) && !isKeydown(SCANCODE_Right)) {
            buttonPressed = false;
        }
    }
    while(isKeydown(SCANCODE_OK)) {continue;}; // Wait for OK release

    switch (mode) {
        case 0:
            break;
        case 1: 
            if (*action == 1) {
                *action = 2; // Fold
            }
            break;
        case 2: 
            if (*action == 1) {
                *action = 2; // Fold
            }
            break;
        
        default:
          break;
    }

    if (*action == 1) { // Raise
        int msSleep = 200;
        int consecutivePresses = 0;
        returnPressed = false;
        while (!returnPressed) {
            drawRaiseMenu(*raiseAmount + currentBet);
            
            if (isKeydown(SCANCODE_Back)) {
                return -1; // Exit Code
            }
            if (isKeydown(SCANCODE_Up) && *raiseAmount < player->money - player->moneyBet) {
                *raiseAmount += RAISE_AMOUNT;
                if (*raiseAmount > player->money - player->moneyBet) { *raiseAmount = player->money - player->moneyBet; }
                if (consecutivePresses > 0) {
                    consecutivePresses++;
                } else {
                    consecutivePresses = 1;
                }
            } else if (isKeydown(SCANCODE_Down) && *raiseAmount > 0) {
                *raiseAmount -= RAISE_AMOUNT;
                if (*raiseAmount < 0)  { *raiseAmount = 0; }
                if (consecutivePresses < 0) {
                    consecutivePresses--;
                } else {
                    consecutivePresses = -1;
                }
                
            } else {
                consecutivePresses = 0;
            }
            if (isKeydown(SCANCODE_OK)) {
                returnPressed = true;
            }
            int realMsSleep = msSleep - (abs(consecutivePresses) * 50);
            extapp_msleep(realMsSleep > 0 ? realMsSleep : 0);
        }
        if (*raiseAmount == 0) { *action = 0; } // Call
        while(isKeydown(SCANCODE_OK)) {continue;};
    }
    
}


void showPlayer(player_t player, int playerIndex, int nbPlayers) {
    char playerLabel[20];
    snprintf(playerLabel, sizeof(playerLabel), "P %d/%d", playerIndex + 1 , nbPlayers);
    extapp_drawTextLarge(playerLabel, SCREEN_WIDTH - 50, SCREEN_HEIGHT - 20, 0x0000, 0xFFFF, false);
    showHand(player.hand);
}

void showCommunityCards( card_t communityCards[5], card_t previousCommunityCards[5]) {
    int delta = 5;
    int x_middle = SCREEN_WIDTH / 2;
    int totalWidth = (CARD_WIDTH + delta) * SMALL_SCALE + 5 + 5 * (CARD_WIDTH * SMALL_SCALE + 5);
    int x = x_middle - (totalWidth / 2);
    int y = 50;


    drawStretchedImageCropped(x, y, (CARD_WIDTH + delta) * SMALL_SCALE, (CARD_HEIGHT + delta) * SMALL_SCALE, card_deck, 0, 0, card_deck.width, card_deck.height);
    for (int i = 0; i < 5; i++) {
        
        int x_i = x + (i + 1) * (CARD_WIDTH * SMALL_SCALE + 5) + delta;
        if (communityCards[i].rank != -1) {
            if (previousCommunityCards[i].rank == -1) {
                drawStretchedImageCropped(x_i, y + delta, CARD_WIDTH * SMALL_SCALE, CARD_HEIGHT * SMALL_SCALE, card_flip, 0, 0, card_flip.width, card_flip.height);
                extapp_msleep(300);
            }
            switch (communityCards[i].color) {
                case 0: 
                    drawCardScaled(x_i, y + delta, H_atlas, communityCards[i].rank, SMALL_SCALE);
                    break;
                case 1: 
                    drawCardScaled(x_i, y + delta, D_atlas, communityCards[i].rank, SMALL_SCALE);
                    break;
                case 2: 
                    drawCardScaled(x_i, y + delta, S_atlas, communityCards[i].rank, SMALL_SCALE);
                    break;
                case 3: 
                    drawCardScaled(x_i, y + delta, C_atlas, communityCards[i].rank, SMALL_SCALE);
                    break;
            }
        }
        else {
            drawStretchedImageCropped(x_i, y + delta, CARD_WIDTH * SMALL_SCALE, CARD_HEIGHT * SMALL_SCALE, card_back, 0, 0, card_back.width, card_back.height);
        }
        
    }
}
void showCommunityCardsWithoutAnimation( card_t communityCards[5]) {
    int delta = 5;
    int x_middle = SCREEN_WIDTH / 2;
    int totalWidth = (CARD_WIDTH + delta) * SMALL_SCALE + 5 + 5 * (CARD_WIDTH * SMALL_SCALE + 5);
    int x = x_middle - (totalWidth / 2);
    int y = 50;


    drawStretchedImageCropped(x, y, (CARD_WIDTH + delta) * SMALL_SCALE, (CARD_HEIGHT + delta) * SMALL_SCALE, card_deck, 0, 0, card_deck.width, card_deck.height);
    for (int i = 0; i < 5; i++) {
        
        int x_i = x + (i + 1) * (CARD_WIDTH * SMALL_SCALE + 5) + delta;
        if (communityCards[i].rank != -1) {
            switch (communityCards[i].color) {
                case 0: 
                    drawCardScaled(x_i, y + delta, H_atlas, communityCards[i].rank, SMALL_SCALE);
                    break;
                case 1: 
                    drawCardScaled(x_i, y + delta, D_atlas, communityCards[i].rank, SMALL_SCALE);
                    break;
                case 2: 
                    drawCardScaled(x_i, y + delta, S_atlas, communityCards[i].rank, SMALL_SCALE);
                    break;
                case 3: 
                    drawCardScaled(x_i, y + delta, C_atlas, communityCards[i].rank, SMALL_SCALE);
                    break;
            }
        }
        else {
            drawStretchedImageCropped(x_i, y + delta, CARD_WIDTH * SMALL_SCALE, CARD_HEIGHT * SMALL_SCALE, card_back, 0, 0, card_back.width, card_back.height);
        }
        
    }
}


void foldAnimation(int originX, int originY) {

    extapp_pushRectUniform(SCREEN_WIDTH / 2 - (CARD_WIDTH * 2.4) / 2, SCREEN_HEIGHT - CARD_HEIGHT * 0.7, CARD_WIDTH * 2.4, CARD_HEIGHT * 0.7, 0xFFFF);
    const int offset = 5;
    const int frames = 7;
    int totalWidth = card_back_med.width * 2 + offset;
    int startX[2] = {
        originX - (totalWidth - card_back_med.width),
        originX
    };
    int startY = originY;
    int dx[2] = { -40, 40 };
    int dy = -60;
    int x, y;
    double angle;
    int prev_x[2];
    int prev_y[2];
    double prev_angle[2];


    for (int t = 0; t < frames; t++) {
        double p = (double)t / (double)(frames - 1);

    
        for (int i = 0; i < 2; i++) {
            double p_accel = p * p * p;

            if (t == 0) {
            x = startX[i] + (int)(dx[i] * p_accel);
            y = startY + (int)(dy * p_accel);
            angle = ((i == 0) ? -1.0 : 1.0) * (0.6 * p_accel);
            drawImageRotated(x, y, card_back_med, angle);

            } else {                
            x = startX[i] + (int)(dx[i] * p_accel);
            y = startY + (int)(dy * p_accel);
            angle = ((i == 0) ? -1.0 : 1.0) * (0.6 * p_accel);
            drawImageRotatedAndRemovePrevious(x, y, card_back_med, angle, prev_x[i], prev_y[i], card_back_med, prev_angle[i]);

            }
            prev_x[i] = x;
            prev_y[i] = y;
            prev_angle[i] = angle;
        }
        
        if (t == frames - 1) {
          
            int clearWidth = card_back_med.width * 1.55;
            int clearHeight = card_back_med.height * 1.4;
            extapp_pushRectUniform(
                startX[0] + (int)(dx[0] * p) - (clearWidth - card_back_med.width) / 2,
                startY + (int)(dy * p) - (clearHeight - card_back_med.height) / 2,
                clearWidth,
                clearHeight,
                0xFFFF
            );
            extapp_pushRectUniform(
                startX[1] + (int)(dx[1] * p) - (clearWidth - card_back_med.width) / 2,
                startY + (int)(dy * p) - (clearHeight - card_back_med.height) / 2,
                clearWidth,
                clearHeight,
                0xFFFF
            );
        }
    }

}

void dealCardsAnimation(int deckX, int deckY, int handCenterX, int handCenterY) {
    const int offset = 5;
    const int frames = 10;

    int totalWidth = card_back_med.width * 2 + offset;
    int targetX[2] = {
        handCenterX - (totalWidth - card_back_med.width),
        handCenterX
    };
    int targetY = handCenterY;

    for (int i = 0; i < 2; i++) {
        int prev_x = 0, prev_y = 0;
        double prev_angle = 0.0;

        for (int t = 0; t < frames; t++) {
            double p = (double)t / (double)(frames - 1);
            double p_accel = p * p * p;

            int x = deckX + (int)((targetX[i] - deckX) * p_accel);
            int y = deckY + (int)((targetY - deckY) * p_accel);
            double angle = ((i == 0) ? -CARD_INCLINE : CARD_INCLINE) * p_accel;

            if (t == 0) {
                drawImageRotated(x, y, card_back_med, angle);
            } else {
                drawImageRotatedAndRemovePrevious(
                    x, y, card_back_med, angle,
                    prev_x, prev_y, card_back_med, prev_angle
                );
            }

            prev_x = x;
            prev_y = y;
            prev_angle = angle;

            extapp_msleep(12);
        }

        int clearWidth = card_back_med.width * 1.55;
        int clearHeight = card_back_med.height * 1.4;
        extapp_pushRectUniform(
            targetX[i] - (clearWidth - card_back_med.width) / 2,
            targetY - (clearHeight - card_back_med.height) / 2,
            clearWidth,
            clearHeight,
            0xFFFF
        );
    }
}
void drawPassToPlayerBox(int playerIndex, card_t communityCards[5]) {
    char msg[32];
    snprintf(msg, sizeof(msg), "Pass to Player %d", playerIndex + 1);
    int boxWidth = 180;
    int boxHeight = 60;
    int x = (SCREEN_WIDTH - boxWidth) / 2;
    int y = (SCREEN_HEIGHT - boxHeight);
    extapp_pushRectUniform(x, y, boxWidth, boxHeight, 0xFFFF);
    extapp_drawTextLarge(msg, x + 10, y + 10, 0x0000, 0xFFFF, false);
    extapp_drawTextLarge("Press OK", x + 45, y + 35, 0x0000, 0xFFFF, false);
    while (!isKeydown(SCANCODE_OK)) { }
    while (isKeydown(SCANCODE_OK)) { }
    extapp_pushRectUniform(x, y, boxWidth, boxHeight, 0xFFFF);
    showCommunityCardsWithoutAnimation(communityCards);
}

int bettingRound(player_t** players, int* bettingPot, int* currentBet, int smallBlind, int bigBlind, bool isPreFlop, card_t communityCards[5], card_t previousCommunityCards[5], int nbPlayers) {
    int action;
    int raiseAmount;
    showCommunityCards(communityCards, previousCommunityCards);
    if (isPreFlop) {
        action = 0;
        raiseAmount = 0;
        bet(&((*players)[0]), bettingPot, smallBlind);
        *currentBet = smallBlind;
        showPlayer((*players)[0], 0, nbPlayers);
        if (selectPlayerAction(&((*players)[0]), &raiseAmount, &action, 1, *currentBet) == -1) return -1;
        if (action == 2) {
            (*players)[0].folded = true;
            foldAnimation(SCREEN_WIDTH / 2, SCREEN_HEIGHT - CARD_HEIGHT * 0.6);
        }
        drawPassToPlayerBox(1, communityCards);
        extapp_pushRectUniform((SCREEN_WIDTH - 120 - 70) / 2, 0, 120 + 70, 25, 0xFFFF);
        action = 0;
        raiseAmount = 0;
        bet(&((*players)[1]), bettingPot, bigBlind);
        *currentBet = bigBlind;
        showPlayer((*players)[1], 1, nbPlayers);
        if (selectPlayerAction(&((*players)[1]), &raiseAmount, &action, 2, *currentBet) == -1) return -1;
        if (action == 2) {
            (*players)[1].folded = true;
            foldAnimation(SCREEN_WIDTH / 2, SCREEN_HEIGHT - CARD_HEIGHT * 0.6);
        }
        drawPassToPlayerBox(2, communityCards);
        extapp_pushRectUniform((SCREEN_WIDTH - 100 - 70) / 2, 0, 100 + 70, 25, 0xFFFF);
    
    }
    int startingPlayer = isPreFlop ? 2 : 0;
    bool bettingActive = true;
    int activePlayers = 0;
    for (int i = 0; i < nbPlayers; i++) {
        if (!(*players)[i].folded) {
            activePlayers++;
        }
    }
    
    while (bettingActive && activePlayers > 1) {
        bettingActive = false;
        for (int i = startingPlayer; i < nbPlayers; i++) {

            if ((*players)[i].folded || (*players)[i].money == 0 || (*players)[i].moneyBet == *currentBet) {
                continue;
            }

            int nextPlayer = i;
            for (int off = 1; off < nbPlayers; ++off) {
                int j = (i + off) % nbPlayers;
                if (!(*players)[j].folded) {
                    nextPlayer = j;
                    break;
                }
            }

            showPlayer((*players)[i], i, nbPlayers);
            action = 0;
            raiseAmount = 0;
            if (selectPlayerAction(&((*players)[i]), &raiseAmount, &action, 0, *currentBet) == -1) return -1;
            if (action == 0) { // Call
                int callAmount = *currentBet - (*players)[i].moneyBet;
                if (callAmount > 0 && (*players)[i].money >= callAmount) {
                    bet(&((*players)[i]), bettingPot, callAmount);
                } else if ((*players)[i].money < callAmount) {
                    bet(&((*players)[i]), bettingPot, (*players)[i].money);
                }
               
            } else if (action == 1) { // Raise
                int totalBet = *currentBet + raiseAmount - (*players)[i].moneyBet;
                if (totalBet > 0 && (*players)[i].money >= totalBet) {
                    bet(&((*players)[i]), bettingPot, totalBet);
                    *currentBet = (*players)[i].moneyBet;
                    bettingActive = true;
                } else {
                    int callAmount = *currentBet - (*players)[i].moneyBet;
                    if (callAmount > 0 && (*players)[i].money >= callAmount) {
                        bet(&((*players)[i]), bettingPot, callAmount);
                    }
                }
            } else { // Fold
                (*players)[i].folded = true;
                foldAnimation(SCREEN_WIDTH / 2, SCREEN_HEIGHT - CARD_HEIGHT * 0.6);
                activePlayers--;
            }
            
            if (activePlayers == 1) {
                for (int i = 0; i < nbPlayers; i++) {
                    if (!(*players)[i].folded) {
                        (*players)[i].money += *bettingPot;
                        return i; // Return the index of the winning player
                    }
                }
            }
            
            drawPassToPlayerBox(nextPlayer, communityCards);
        }
    }
    *currentBet = 0;
    return -2; // No errors
}

char* scoreToHand(int score) {
    if (score >= 10000) return "Royal Flush";
    if (score >= 9000) return "Straight Flush";
    if (score >= 8000) return "Four of a Kind";
    if (score >= 7000) return "Full House";
    if (score >= 6000) return "Flush";
    if (score >= 5000) return "Straight";
    if (score >= 4000) return "Three of a Kind";
    if (score >= 3000) return "Double Pair";
    if (score >= 2000) return "Pair";
    return "High Card";
}


int game(player_t** players, int* winningScore, int nbPlayers) {
    int smallBlind = 10;
    int bigBlind = smallBlind * 2;
    int bettingPot = 0;
    int currentBet = 0;
    card_t emptyCard = {-1, -1};
    card_t communityCards[5] = {emptyCard, emptyCard, emptyCard, emptyCard, emptyCard};
    card_t previousCommunityCards[5] = {emptyCard, emptyCard, emptyCard, emptyCard, emptyCard};
    int deck[52];
    int deckIndex = 0;
    int gamePhase = 0;
    int bettingResult;

    for (int i = 0; i < 52; i++) {
        deck[i] = i;
    }
    for (int i = 51; i > 0; i--) {
        int j = randint(0, i);
        int temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
    for (int i = 0; i < nbPlayers; i++) {
        (*players)[i].hand[0] = indexToCard(deck[deckIndex++]);
        (*players)[i].hand[1] = indexToCard(deck[deckIndex++]);
        (*players)[i].folded = false;
        (*players)[i].moneyBet = 0;
    }
    drawBettingPot(bettingPot);
    bettingResult = bettingRound(players, &bettingPot, &currentBet, smallBlind, bigBlind, true, communityCards, previousCommunityCards, nbPlayers);
    if (bettingResult != -2) return bettingResult;

    // Flop (3 cards)
    for (int i = 0; i < 3; i++) {
        communityCards[i] = indexToCard(deck[deckIndex++]);
    }
    gamePhase = 1;

    bettingResult = bettingRound(players, &bettingPot, &currentBet, smallBlind, bigBlind, false, communityCards, previousCommunityCards, nbPlayers);
    if (bettingResult != -2) return bettingResult;

    for (int i = 0; i < 5; i++) {
        previousCommunityCards[i] = communityCards[i];
    }
    // Turn (1 card)
    communityCards[3] = indexToCard(deck[deckIndex++]);
    gamePhase = 2;

    bettingResult = bettingRound(players, &bettingPot, &currentBet, smallBlind, bigBlind, false, communityCards, previousCommunityCards, nbPlayers);
    if (bettingResult != -2) return bettingResult;

    for (int i = 0; i < 5; i++) {
        previousCommunityCards[i] = communityCards[i];
    }
    // River (1 card)
    communityCards[4] = indexToCard(deck[deckIndex++]);
    gamePhase = 3;
    bettingResult = bettingRound(players, &bettingPot, &currentBet, smallBlind, bigBlind, false, communityCards, previousCommunityCards, nbPlayers);
    if (bettingResult != -2) return bettingResult;
    
    
    
    return getWinner(*players, communityCards, winningScore, nbPlayers);
}

player_t generateRandomPlayer(int money) {
    player_t p;
    p.money = money;
    p.hand[0].rank = randint(0, 12);
    p.hand[0].color = randint(0, 3);
    p.hand[1].rank = randint(0, 12);
    p.hand[1].color = randint(0, 3);
    p.moneyBet = 0;
    p.folded = false;
    return p;
}

double randdouble(double min, double max) {
    double range = (max - min); 
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

int menuGraphic() {
    srand(1553288);
    int nb = 5;
    int delta = 40;
    int x = SCREEN_WIDTH / 2 - delta * nb / 2;
    int y = SCREEN_HEIGHT / 2 - card_back_med.height / 2 + 20;
    for (int i = 0; i < nb; i++) {
        drawImageRotated(x + delta * i, y,card_back_med,  randdouble(-M_PI, M_PI));
    }
}

int menu(int* nbPlayers, int* startingMoney) {
    
    const int minPlayers = 2;
    const int maxPlayers = 8;
    const int minMoney = 100;
    const int maxMoney = 5000;
    const int moneyStep = 10;

    int n = (*nbPlayers >= minPlayers && *nbPlayers <= maxPlayers) ? *nbPlayers : 2;
    int m = (*startingMoney >= minMoney && *startingMoney <= maxMoney) ? *startingMoney : 1000;

    int selected = 0; 
    bool buttonPressed = false;

    waitForKeyReleased();
    bool first = true;
    while (true) {
        

        const char* title = "Hold'em Poker";
        int titleLen = 0; while (title[titleLen]) titleLen++;
        int titleX = SCREEN_WIDTH / 2 - (titleLen * 10) / 2;
        extapp_drawTextLarge(title, titleX, 8, 0x0000, 0xFFFF, false);


        char playersStr[32];
        snprintf(playersStr, sizeof(playersStr), "Players: %d", n);
        int playersLen = 0; while (playersStr[playersLen]) playersLen++;
        int playersX = SCREEN_WIDTH / 2 - (playersLen * 10) / 2;
        uint16_t playersColor = (selected == 0) ? 0xF800 : 0x0000;
        extapp_drawTextLarge(playersStr, playersX, 40, playersColor, 0xFFFF, false);


        char moneyStr[32];
        snprintf(moneyStr, sizeof(moneyStr), "Starting: $%d", m);
        int moneyLen = 0; while (moneyStr[moneyLen]) moneyLen++;
        int moneyX = SCREEN_WIDTH / 2 - (moneyLen * 10) / 2;
        uint16_t moneyColor = (selected == 1) ? 0xF800 : 0x0000;
        extapp_drawTextLarge(moneyStr, moneyX, 65, moneyColor, 0xFFFF, false);


        const char* hint = "OK to start";
        int hintLen = 0; while (hint[hintLen]) hintLen++;
        int hintX = SCREEN_WIDTH / 2 - (hintLen * 10) / 2;
        extapp_drawTextLarge(hint, hintX, SCREEN_HEIGHT - 18, 0x0000, 0xFFFF, false);

        drawRaiseMenu(m);


        if (isKeydown(SCANCODE_Back)) {
            return -1;
        }

        if (!buttonPressed && isKeydown(SCANCODE_Up)) {
            if (selected > 0) selected--;
            buttonPressed = true;
        } else if (!buttonPressed && isKeydown(SCANCODE_Down)) {
            if (selected < 1) selected++;
            buttonPressed = true;
        } else if (!buttonPressed && isKeydown(SCANCODE_Left)) {
            if (selected == 0 && n > minPlayers) n--;
            else if (selected == 1 && m > minMoney) {
                m -= moneyStep;
                if (m < minMoney) m = minMoney;
            }
            buttonPressed = true;
        } else if (!buttonPressed && isKeydown(SCANCODE_Right)) {
            if (selected == 0 && n < maxPlayers) n++;
            else if (selected == 1 && m < maxMoney) {
                m += moneyStep;
                if (m > maxMoney) m = maxMoney;
            }
            buttonPressed = true;
        } else if (isKeydown(SCANCODE_OK)) {
            while (isKeydown(SCANCODE_OK)) { }
            break;
        }

        if (!isKeydown(SCANCODE_Up) && !isKeydown(SCANCODE_Down) &&
            !isKeydown(SCANCODE_Left) && !isKeydown(SCANCODE_Right)) {
            buttonPressed = false;
        }

        extapp_msleep(16);
        if (first) {
            menuGraphic();
            first = false;
        }
    }

    *nbPlayers = n;
    *startingMoney = m;
    waitForKeyReleased();
    return 0;
}

void extapp_main(void) {
    while (true) {
        waitForKeyReleased();
        extapp_pushRectUniform(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0xFFFF);

        card_t demoHand[2] = {
            { .rank = randint(0, 12), .color = 0 },
            { .rank = randint(0, 12), .color = 3 }
        };


        int nbPlayers;
        int money;

        int menuRet = menu(&nbPlayers, &money);
        if (menuRet == -1) { return; }
        extapp_pushRectUniform(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0xFFFF);

        player_t* players = malloc(nbPlayers * sizeof(player_t));
        assert(players != NULL);
        for (int i = 0; i < nbPlayers; i++) {
            players[i] = generateRandomPlayer(money);
        }

        int winningScore;
        int winner = game(&players, &winningScore, nbPlayers);

        if (winner != -1) {
            char winnerMessage[40];
            snprintf(winnerMessage, sizeof(winnerMessage), "Player %d", winner + 1);
            int winnerMessageLen = 0;
            while (winnerMessage[winnerMessageLen] != '\0') { winnerMessageLen++; }
            int labelX = SCREEN_WIDTH / 2 - (winnerMessageLen * 10) / 2;
            int labelY = SCREEN_HEIGHT / 2 + 30;
            extapp_drawTextLarge(winnerMessage, labelX, labelY, 0x0000, 0xFFFF, false);

            snprintf(winnerMessage, sizeof(winnerMessage), "won with a %s", scoreToHand(winningScore));
            winnerMessageLen = 0;
            while (winnerMessage[winnerMessageLen] != '\0') { winnerMessageLen++; }
            labelX = SCREEN_WIDTH / 2 - (winnerMessageLen * 10) / 2;
            labelY = SCREEN_HEIGHT / 2 + 50;
            extapp_drawTextLarge(winnerMessage, labelX, labelY, 0x0000, 0xFFFF, false);

            showPlayer(players[winner], winner, nbPlayers);
        
            while(true) {
                if (isKeydown(SCANCODE_OK)) {
                    break;
                } else if (isKeydown(SCANCODE_Back)) {
                    return;
                }
            }
        }
        free(players);
    }
}
