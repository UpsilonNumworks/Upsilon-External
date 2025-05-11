#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "extapp_api.h"
#include "inc/peripherals.h"

#define WORLD_WIDTH 16
#define WORLD_HEIGHT 16
#define SCREEN_WIDTH 322
#define SCREEN_HEIGHT 242
#define M_PI 3.14159265359


typedef struct Player {
    double x;
    double y;
    double angle;
    double fov;
} player_t;

int worldMap[WORLD_HEIGHT][WORLD_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 1},
    {1, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 2, 0, 2, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 2, 0, 2, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 2, 0, 2, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 0, 0, 0, 1},
    {1, 0, 0, 3, 0, 0, 0, 0, 0, 0, 4, 4, 0, 0, 0, 1},
    {1, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 3, 0, 0, 2, 0, 2, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};


int getCell(int x, int y) {
    if (0 <= x && x < WORLD_WIDTH && 0 <= y && y < WORLD_HEIGHT) {
        return worldMap[y][x];
    }
    return -1;
}


void setCell(int x, int y, int value) {
    if (0 <= x && x < WORLD_WIDTH && 0 <= y && y < WORLD_HEIGHT) {
        worldMap[y][x] = value;
    }
}

double sqrt(double x) {
    double result = 1;
    double term = 1;

    for (int i = 0; i < 10; i++) {
        term = (term + x / term) / 2;
        result = term;
    }

    return result;
}

double fmod(double x, double y) {
    return x - y * (int)(x / y);
}

double cos(double angle) {
    double result = 1;
    double term = 1;
    double angleSquared = angle * angle;
    int sign = -1;

    for (int i = 2; i <= 20; i += 2) {
        term *= angleSquared / (i * (i - 1));
        result += sign * term;
        sign = -sign;
    }

    return result;
}

double sin(double angle) {
    double result = angle;
    double term = angle;
    double angleSquared = angle * angle;
    int sign = -1;

    for (int i = 3; i <= 21; i += 2) {
        term *= angleSquared / (i * (i - 1));
        result += sign * term;
        sign = -sign;
    }

    return result;
}

double tan(double angle) {
    return sin(angle) / cos(angle);
}

double fabs(double x) {
    if (x < 0) {
        return -x;
    }
    return x;
}

double acos(double x) {
    if (x < -1 || x > 1) {
        return 0; // Error case
    }
    double result = 0;
    double term = x;
    double angle = 0;

    for (int i = 1; i <= 20; i++) {
        term *= (x * x) / ((2 * i - 1) * (2 * i));
        angle += term / (2 * i + 1);
    }

    result = M_PI / 2 - angle;
    return result;
}

int castRay(double rayAngle, int *hitX, int *hitY, int *hitDistance, int *extSide, double playerX, double playerY) {
    double rayDirX = cos(rayAngle);
    double rayDirY = sin(rayAngle);


    int mapX = (int)playerX;
    int mapY = (int)playerY;

    double sideDistX, sideDistY;
    double deltaDistX = fabs(1 / rayDirX);
    double deltaDistY = fabs(1 / rayDirY);
    double perpWallDist;

    int stepX, stepY;
    int hit = 0;
    int side;

    if (rayDirX < 0) {
        stepX = -1;
        sideDistX = (playerX - mapX) * deltaDistX;
    } else {
        stepX = 1;
        sideDistX = (mapX + 1.0 - playerX) * deltaDistX;
    }
    if (rayDirY < 0) {
        stepY = -1;
        sideDistY = (playerY - mapY) * deltaDistY;
    } else {
        stepY = 1;
        sideDistY = (mapY + 1.0 - playerY) * deltaDistY;
    }

    while (!hit) {
        if (sideDistX < sideDistY) {
            sideDistX += deltaDistX;
            mapX += stepX;
            side = 0;
        } else {
            sideDistY += deltaDistY;
            mapY += stepY;
            side = 1;
        }

        if (getCell(mapX, mapY) > 0) {
            hit = 1;
        }
    }

    if (side == 0) {
        perpWallDist = (mapX - playerX + (1 - stepX) / 2) / rayDirX;
    } else {
        perpWallDist = (mapY - playerY + (1 - stepY) / 2) / rayDirY;
    }
    if (mapX < 0 || mapX >= WORLD_WIDTH || mapY < 0 || mapY >= WORLD_HEIGHT || perpWallDist * 10 > 1000) {
        *hitDistance = 0; // No hit
        return 0;
    }
    *hitX = mapX;
    *hitY = mapY;
    *hitDistance = (int)(perpWallDist * 10);
    *extSide = side;
    return 1;
}



int slope(int height1, int height2, int t, int n) {
    return height1 + (height2 - height1) * (t / n);
}

int clamp(int color) {
    if (color < 0) {
        return 0;
    }
    if (color > 255) {
        return 255;
    }
    return color;
}
double lerp(double start, double end, double t) {
    return start * (1.0 - t) + end * t;
}
uint16_t getColor(int cell) {
    switch (cell) {
        case 1: return 0xFFFF; // White
        case 2: return 0xF800; // Red
        case 3: return 0x07E0; // Green
        case 4: return 0x001F; // Blue
        default: return 0x0000; // Black
    }
}


void castRays(player_t* player,int origX, int origY, int screenWidth, int screenHeight, int nRay) {
    double playerAngle = player->angle;
    double playerX = player->x;
    double playerY = player->y;
    double fov = player->fov;
    double halfFov = fov / 2;
    double rayStep = fov / nRay;
    double t = fov / nRay;
    int rectWidth = screenWidth / nRay;
    int hitX, hitY, hitDistance, side;
    int previousX, previousY, previousColor, previousSide, previousNbRect1, previousNbRect2, previousColumnHeight1, previousColumnHeight2;
    double columnHeight;
    for (int nRect = 0; nRect < nRay; nRect++) {
        double rayAngle = playerAngle - halfFov + nRect * rayStep;
        
            
        if (castRay(rayAngle, &hitX, &hitY, &hitDistance, &side, playerX, playerY)) {
            double correctedDistance = hitDistance * cos(rayAngle - playerAngle);
            if (correctedDistance != 0) {
                columnHeight = screenHeight * 10. / correctedDistance;
            } else {
                columnHeight = screenHeight;
            }
            
            uint16_t color = getColor(getCell(hitX, hitY));
            
            double shade = (side == 1)? 0.9 - correctedDistance / 100 : 1 - correctedDistance / 100;
            int16_t red = (color >> 11) & 0x1F;
            uint16_t green = (color >> 5) & 0x3F;
            uint16_t blue = color & 0x1F; 
            red = clamp(red * shade);
            green = clamp(green *shade);
            blue = clamp(blue * shade);

            color = (red << 11) | (green << 5) | blue;

            if (nRect == 0) {
                previousColor = color;
                previousNbRect1 = nRect;
                previousColumnHeight1 = columnHeight;
                previousSide = side;
            }
            else if (hitX != previousX || hitY != previousY || side != previousSide || nRect == nRay - 1) {

                // Interpolation between in a cell
                int n = previousNbRect2 - previousNbRect1;
                for (int i = 0; i <= n; i++) {
                    double t = (double)i / n;
                    int height = (int)lerp(previousColumnHeight1, previousColumnHeight2, t);
                    
                    if (screenHeight > height) {
                        extapp_pushRectUniform(origX + (previousNbRect1 + i) * rectWidth, origY, rectWidth, 
                                            (screenHeight - height) / 2, 0x0000);
                        extapp_pushRectUniform(origX + (previousNbRect1 + i) * rectWidth, 
                                            origY + (screenHeight + height) / 2, rectWidth,
                                            (screenHeight - height) / 2, 0x4208);
                        extapp_pushRectUniform(origX + (previousNbRect1 + i) * rectWidth, 
                                        origY + screenHeight / 2 - height / 2,
                                        rectWidth, height, previousColor);
                    } else {
                        extapp_pushRectUniform(origX + (previousNbRect1 + i) * rectWidth, 
                                        origY,
                                        rectWidth, screenHeight, previousColor);
                    }
                    
                   
                    
                }
                previousColor = color;
                previousNbRect1 = nRect;
                previousColumnHeight1 = columnHeight;
                previousSide = side;
            } 
            previousX = hitX;
            previousY = hitY;
            previousNbRect2 = nRect;
            previousColumnHeight2 = columnHeight;
            
            
        } 
        else {
            extapp_pushRectUniform((nRect) * rectWidth, 0, rectWidth, 
                            (screenHeight) / 2, 0x0000);
            extapp_pushRectUniform((nRect) * rectWidth, 
                            (screenHeight) / 2, rectWidth,
                            (screenHeight ) / 2, 0x4208);
        }
    }
}


bool isKeydown(uint64_t key) {
    return extapp_scanKeyboard() & key;
}
void control(uint64_t up, uint64_t down, uint64_t left, uint64_t right, player_t* player) {
    if (isKeydown(left)) {
        player->angle = fmod(player->angle - 0.1, 2 * M_PI);
    }
    if (isKeydown(right)) {
        player->angle = fmod(player->angle + 0.1, 2 * M_PI);
    }
    if (isKeydown(up)) {
        double newX = player->x + cos(player->angle) * 0.1;
        double newY = player->y + sin(player->angle) * 0.1;
        if (getCell((int)newX, (int)newY) == 0) {
            player->x = newX;
            player->y = newY;
        }
    }
    if (isKeydown(down)) {
        double newX = player->x - cos(player->angle) * 0.1;
        double newY = player->y - sin(player->angle) * 0.1;
        if (getCell((int)newX, (int)newY) == 0) {
            player->x = newX;
            player->y = newY;
        }
    }
}


void extapp_main(void) {
    double fov = M_PI / 3;

    double playerAngle1 = 0;
    double playerX1 = 1.5;
    double playerY1 = 1.5;

    double playerAngle2 = 0;
    double playerX2 = 1.5;
    double playerY2 = 1.5;
    player_t player1 = {playerX1, playerY1, playerAngle1, fov};
    player_t player2 = {playerX2, playerY2, playerAngle2, fov};

    waitForKeyReleased();
    extapp_pushRectUniform(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0x0000);
   
    while (true) {
        if (extapp_isKeydown(KEY_CTRL_EXIT)) {
            break;
        }
        castRays(&player1, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 320 / 2);

        control(SCANCODE_Up, SCANCODE_Down, SCANCODE_Left, SCANCODE_Right, &player1);

    }
}
