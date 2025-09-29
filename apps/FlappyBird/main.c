



#include "./inc/libraries.h"
#include "./inc/peripherals.h"
#include "./inc/render.h"
#include "./inc/sprite_buffer.h"
#include "./inc/math_functions.h"

#include "./inc/textures/yellowbird-downflap.h"
#include "./inc/textures/yellowbird-midflap.h"
#include "./inc/textures/yellowbird-upflap.h"
#include "./inc/textures/background.h"
#include "./inc/textures/pipe.h"
#include "./inc/textures/base.h"
#include "./inc/textures/gameover.h"
#include "./inc/textures/scoreboard.h"
#include "./inc/textures/title.h"
#include "./inc/textures/pause.h"
#include "./inc/textures/start-menu.h"

#include "./inc/textures/0-img.h"
#include "./inc/textures/1-img.h"
#include "./inc/textures/2-img.h"
#include "./inc/textures/3-img.h"
#include "./inc/textures/4-img.h"
#include "./inc/textures/5-img.h"
#include "./inc/textures/6-img.h"
#include "./inc/textures/7-img.h"
#include "./inc/textures/8-img.h"
#include "./inc/textures/9-img.h"

#include "./inc/textures/0-img-small.h"
#include "./inc/textures/1-img-small.h"
#include "./inc/textures/2-img-small.h"
#include "./inc/textures/3-img-small.h"
#include "./inc/textures/4-img-small.h"
#include "./inc/textures/5-img-small.h"
#include "./inc/textures/6-img-small.h"
#include "./inc/textures/7-img-small.h"
#include "./inc/textures/8-img-small.h"
#include "./inc/textures/9-img-small.h"


#define PIPE_WIDTH 39
#define PIPE_GAP 80
#define PIPE_SPEED 2
#define MAX_PIPES 3

typedef struct Pipe {
    int x;
    int gap_y;
    bool active;
} pipe_t;

int initialized = 0;
double prev_t = 0.0;
double x, y;
double vx, vy;
int last_pressed = 0;

typedef struct Player {
    int x_i;
    int y_i;
    int velX_i;
    int velY_i;

    int x;
    int y;
    int velX;
    int velY;
    double angle;
    obm_t sprites[3];
} player_t;


const int g = 10;
sprite_buffer_t* spriteBuffer = NULL;


pipe_t pipes[MAX_PIPES];


int randint(int a, int b) {
    return rand() % (b - a + 1) + a;
}

void initPipes() {
    for (int i = 0; i < MAX_PIPES; i++) {
        pipes[i].x = SCREEN_WIDTH + i * (SCREEN_WIDTH / MAX_PIPES);
        pipes[i].gap_y = randint(30, SCREEN_HEIGHT - PIPE_GAP - 30 - 40);
        pipes[i].active = true;
    }
}

void updatePipes(int* score, player_t* player) {
    for (int i = 0; i < MAX_PIPES; i++) {
        pipes[i].x -= PIPE_SPEED;
        if (pipes[i].x < -PIPE_WIDTH) {
            pipes[i].x = SCREEN_WIDTH;
            pipes[i].gap_y = randint(30, SCREEN_HEIGHT - PIPE_GAP - 30 - 40);
            pipes[i].active = true;
        }
        if (pipes[i].x + PIPE_WIDTH <= player->x && pipes[i].active) {
            (*score)++;
            pipes[i].active = false;
        }
    }
}

void drawPipes() {
    for (int i = 0; i < MAX_PIPES; i++) {
        // Draw top pipe
        pushSprite(&spriteBuffer, &pipe, pipes[i].x, -pipe.height + pipes[i].gap_y, M_PI);
        // Draw bottom pipe
        pushSprite(&spriteBuffer, &pipe, pipes[i].x, pipes[i].gap_y + PIPE_GAP, 0);
    }
}

bool checkPipeCollision(player_t* player, int baseY) {

    int px = player->x, py = player->y;
    if (py + 20 >= baseY) { return true; }
    for (int i = 0; i < MAX_PIPES; i++) {
        if (!pipes[i].active) continue;
        if (px + 20 > pipes[i].x && px + 4 < pipes[i].x + PIPE_WIDTH) {
            if (py + 4 < pipes[i].gap_y || py + 20 > pipes[i].gap_y + PIPE_GAP) {
                return true;
            }
        }
    }
    return false;
}



obm_t* getNumberImg(char a) {
    switch (a) {
        case '0':
            return &img_0;
        case '1':
            return &img_1;
        case '2':
            return &img_2;
        case '3':
            return &img_3;
        case '4':
            return &img_4;
        case '5':
            return &img_5;
        case '6':
            return &img_6;
        case '7':
            return &img_7;
        case '8':
            return &img_8;
        case '9':
            return &img_9;
        default:
            return NULL;
    }
}

void drawChar(char a, int x, int y) {
    pushSprite(&spriteBuffer, getNumberImg(a), x, y, 0);
}

void drawInt(int n, int x, int y) {

    int delta = 2;

    char nStr[32];
    snprintf(nStr, sizeof(nStr), "%d", n);
    int totalWidth = 0;
    for (int i = 0; nStr[i] != '\0'; i++) {
        totalWidth += getNumberImg(nStr[i])->width + delta;
    }
    int sum = 0;
    for (int i = 0; nStr[i] != '\0'; i++) {
        drawChar(nStr[i], x + sum - totalWidth / 2, y);
        sum += getNumberImg(nStr[i])->width + delta;
    }


}

obm_t* getNumberImgSmall(char a) {
    switch (a) {
        case '0':
            return &img_0_small;
        case '1':
            return &img_1_small;
        case '2':
            return &img_2_small;
        case '3':
            return &img_3_small;
        case '4':
            return &img_4_small;
        case '5':
            return &img_5_small;
        case '6':
            return &img_6_small;
        case '7':
            return &img_7_small;
        case '8':
            return &img_8_small;
        case '9':
            return &img_9_small;
        default:
            return NULL;
    }
}

void drawCharSmall(char a, int x, int y) {
    pushSprite(&spriteBuffer, getNumberImgSmall(a), x, y, 0);
}

void drawIntSmall(int n, int x, int y) {
    int delta = 1;

    char nStr[32];
    snprintf(nStr, sizeof(nStr), "%d", n);
    int totalWidth = 0;
    for (int i = 0; nStr[i] != '\0'; i++) {
        totalWidth += getNumberImgSmall(nStr[i])->width + delta;
    }
    int sum = 0;
    for (int i = 0; nStr[i] != '\0'; i++) {
        drawCharSmall(nStr[i], x + sum - totalWidth / 2, y);
        sum += getNumberImgSmall(nStr[i])->width + delta;
    }

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

player_t* initPlayer(int x_i, int y_i, int velX_i, int velY_i, double angle, obm_t sprites[3]) {
    player_t* player = malloc(sizeof(player_t));
    assert(player != NULL);
    player->x = 0;
    player->y = 0;
    player->velX = 0;
    player->velY = 0;
    player->angle = 0.0;
    player->x_i = x_i;
    player->y_i = y_i;
    player->velX_i = velX_i;
    player->velY_i = velY_i;
    player->angle = angle;
    for (int i = 0; i < 3; i++) {
        player->sprites[i] = sprites[i];
    }
    
    return player;
}

void showSprite(player_t* player, int index) {
    pushSprite(&spriteBuffer, &player->sprites[index], player->x, player->y, player->angle);
}


void computeGravity(player_t* player, double time_step, int jump) {
    if (!player) return;

    

    if (!initialized) {
        initialized = 1;
        prev_t = time_step;
        x = (double)player->x_i;
        y = (double)player->y_i;
        vx = (double)player->velX_i;
        vy = (double)player->velY_i;
    }

    double dt = time_step - prev_t;
    if (dt < 0.0) dt = 0.0;
    prev_t = time_step;

    int pressed = 0;
    #ifdef SCANCODE_OK
    pressed |= isKeydown(SCANCODE_OK);
    #endif
    #ifdef SCANCODE_Up
    pressed |= isKeydown(SCANCODE_Up);
    #endif

    if (pressed && !last_pressed) {
        vy -= (double)jump;
    }
    last_pressed = pressed;

    vy += (double)g * dt;
    y += vy * dt;
    x += vx * dt;

    if (y < 0.0) { y = 0.0; vy = 0.0; }
    if (y > (double)(SCREEN_HEIGHT - 1)) { y = (double)(SCREEN_HEIGHT - 1); vy = 0.0; }
    if (x < 0.0) { x = 0.0; vx = 0.0; }
    if (x > (double)(SCREEN_WIDTH - 1)) { x = (double)(SCREEN_WIDTH - 1); vx = 0.0; }

    player->x = (int)x;
    player->y = (int)y;
    player->velX = (int)vx;
    player->velY = (int)vy;

    double ang = vy * 0.03;
    if (ang < -0.8) ang = -0.8;
    if (ang > 0.8) ang = 0.8;
    player->angle = ang;
}

void resetPlayer(player_t* player, int x_i, int y_i, int velX_i, int velY_i, double angle) {
    player->x = 0;
    player->y = 0;
    player->velX = 0;
    player->velY = 0;
    player->x_i = x_i;
    player->y_i = y_i;
    player->velX_i = velX_i;
    player->velY_i = velY_i;
    player->angle = angle;

    int initialized = 0;
    prev_t = 0.0;
    x = x_i;
    y = y_i;
    vx = velX_i;
    vy = velY_i;
    last_pressed = 0;

}

int strToInt(const char* str) {
    int result = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] < '0' || str[i] > '9') {
            break;
        }
        result = result * 10 + (str[i] - '0');
    }
    return result;
}
void saveScore(int score) {
    size_t file_len = 0;
    const char * filecontent = extapp_fileRead("flappy.dat", &file_len, EXTAPP_RAM_FILE_SYSTEM);

    if (strlen(filecontent) != 0) {
        int highscore = strToInt(filecontent);

        if (score > highscore) {
            char buffer[32];
            snprintf(buffer, sizeof(buffer), "%d", score);

            extapp_fileWrite("flappy.dat", buffer, strlen(buffer), EXTAPP_RAM_FILE_SYSTEM);
        }
    } else {
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%d", score);

        extapp_fileWrite("flappy.dat", buffer, strlen(buffer), EXTAPP_RAM_FILE_SYSTEM);
    }
}

int getHighScore() {
    size_t file_len = 0;
    const char * filecontent = extapp_fileRead("flappy.dat", &file_len, EXTAPP_RAM_FILE_SYSTEM);

    if (strlen(filecontent) != 0) {
        return strToInt(filecontent);
    } else {
        return 0;
    }
}
void extapp_main(void) {
    srand(extapp_millis());
    waitForKeyReleased();
    extapp_pushRectUniform(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0xFFFF);

    obm_t sprites[3] = {yellowbird_downflap, yellowbird_midflap, yellowbird_upflap};
    player_t* player = initPlayer(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0, 0, 0.0, sprites);

    double t = 0.0;
    int jump = 0;
    bool okPressed = false;
    int playerIndex = 0;
    int backgroundX =0;
    int baseY = 200;
    initPipes();
    int score = 0;
    
    int y_anim = -20;
    int step = 2;
    
    while (!isKeydown(SCANCODE_OK) && !isKeydown(SCANCODE_Up)) {
        if (isKeydown(SCANCODE_Back)) { return; }
        pushSprite(&spriteBuffer, &background, 0, 0, 0);
        pushSprite(&spriteBuffer, &sprites[1], SCREEN_WIDTH / 2 - sprites[1].width / 2, SCREEN_HEIGHT / 2 - sprites[1].height / 2 + y_anim, 0);
        pushSprite(&spriteBuffer, &base, backgroundX, baseY, 0);
        pushSprite(&spriteBuffer, &base, backgroundX + base.width, baseY, 0);
        pushSprite(&spriteBuffer, &title, (SCREEN_WIDTH - title.width) / 2, 20, 0);

        pushSprite(&spriteBuffer, &start_menu, (SCREEN_WIDTH - start_menu.width) / 2, SCREEN_HEIGHT / 2  + 60, 0);
        displaySpriteBuffer(&spriteBuffer);
        y_anim += step;
        if (y_anim >= 20 || y_anim <= -20) step = -step;
     }
    while (!isKeydown(SCANCODE_Back)) {
        if (checkPipeCollision(player, baseY)) { 
            saveScore(score);
            while (!isKeydown(SCANCODE_OK) && !isKeydown(SCANCODE_Up)) {
                if (isKeydown(SCANCODE_Back)) {
                    return;
                }
                

                pushSprite(&spriteBuffer, &background, 0, 0, 0);
                showSprite(player, playerIndex);
                drawPipes();
                pushSprite(&spriteBuffer, &base, backgroundX, baseY, 0);
                pushSprite(&spriteBuffer, &base, backgroundX + base.width, baseY, 0);
                pushSprite(&spriteBuffer, &gameover, SCREEN_WIDTH / 2 - gameover.width / 2, 20, 0);
                pushSprite(&spriteBuffer, &scoreboard, SCREEN_WIDTH / 2 - scoreboard.width / 2, 90, 0);
                drawIntSmall(score,  SCREEN_WIDTH / 2 - scoreboard.width / 2 + 50, 90 + 40);
                drawIntSmall(getHighScore(),  SCREEN_WIDTH / 2 - scoreboard.width / 2 + 180, 90 + 40);
                displaySpriteBuffer(&spriteBuffer);


            } 
            resetPlayer(player, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0, 0, 0.0);
            
            score = 0;
            t = 0;
            jump = 0;
            backgroundX =0;
            initPipes();
            okPressed = false;

        }
        if (isKeydown(SCANCODE_Shift)) {
            waitForKeyReleased();
            
            int durBlink = 50;
            int pauseBlink = durBlink / 2;
            while (!isKeydown(SCANCODE_Shift)) {
                if (isKeydown(SCANCODE_Back)) { return; }
                pushSprite(&spriteBuffer, &background, 0, 0, 0);
                showSprite(player, playerIndex);
                drawPipes();
                pushSprite(&spriteBuffer, &base, backgroundX, baseY, 0);
                pushSprite(&spriteBuffer, &base, backgroundX + base.width, baseY, 0);
                drawInt(score, SCREEN_WIDTH / 2, 10);
                if (pauseBlink >= durBlink / 2) {
                    pushSprite(&spriteBuffer, &pause, SCREEN_WIDTH - pause.width - 4, 4, 0);
                }
                
                pauseBlink = (pauseBlink + 1) % (durBlink - 1);
                displaySpriteBuffer(&spriteBuffer);
            }
            waitForKeyReleased();
        }
        jump = 0;
        if ((isKeydown(SCANCODE_OK) || isKeydown(SCANCODE_Up)) && !okPressed) {
            jump = 50;
            okPressed = true;
        } else {
            okPressed = false;
            jump = 0;
        }



        computeGravity(player, t, jump);
        
        updatePipes(&score, player);
        pushSprite(&spriteBuffer, &background, 0, 0, 0);
        showSprite(player, playerIndex);
        drawPipes();
        pushSprite(&spriteBuffer, &base, backgroundX, baseY, 0);
        pushSprite(&spriteBuffer, &base, backgroundX + base.width, baseY, 0);
        drawInt(score, SCREEN_WIDTH / 2, 10);

        displaySpriteBuffer(&spriteBuffer);
        backgroundX -= PIPE_SPEED;
        if (backgroundX <= -base.width) {
            backgroundX = 0;
        }

        
        t += 0.2;
        playerIndex = (playerIndex + 1) % 3;
        
    }
    free(player);
}
