#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "extapp_api.h"
#include "inc/peripherals.h"
#include "inc/selector.h"

#define WORLD_WIDTH 8
#define WORLD_HEIGHT 8
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 242
#define M_PI 3.14159265359

static int worldMap[WORLD_HEIGHT][WORLD_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 3, 0, 0, 0, 1},
    {1, 0, 0, 2, 0, 0, 0, 1},
    {1, 0, 1, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 4, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1}
};

double playerX = 1.5;
double playerY = 1.5;

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

int castRay(double rayAngle, int *hitX, int *hitY, int *hitDistance, int *extSide) {
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

bool isAdjacent(int x1, int y1, int x2, int y2) {
    return (abs(x1 - x2) <= 1 && abs(y1 - y2) <= 1);
}

int slope(int height1, int height2, int t, int n) {
    return height1 + (height2 - height1) * (t / n);
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

void extapp_main(void) {
    double playerAngle = 0;
    double fov = M_PI / 3;
    double halfFov = fov / 2;
    int nRay = 320;
    double rayStep = fov / nRay;
    double t = fov / nRay;
    int rectWidth = SCREEN_WIDTH / nRay;

    waitForKeyReleased();
    extapp_pushRectUniform(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0x0000);
    int hitX, hitY, hitDistance, side;
    int previousX, previousY, previousColor, previousSide, previousNbRect1, previousNbRect2, previousColumnHeight1, previousColumnHeight2;
    while (true) {
        if (extapp_isKeydown(KEY_CTRL_EXIT)) {
            break;
        }

        
        for (int nRect = 0; nRect < nRay; nRect++) {
            double rayAngle = playerAngle - halfFov + nRect * rayStep;
            
                
            if (castRay(rayAngle, &hitX, &hitY, &hitDistance, &side)) {
                double correctedDistance = hitDistance * cos(rayAngle - playerAngle);
                double columnHeight = SCREEN_HEIGHT * 10. / correctedDistance;
                uint16_t color = getColor(getCell(hitX, hitY));
                
                double shade = (side == 1)? 0.9 - correctedDistance / 100 : 1 - correctedDistance / 100;
                int16_t red = (color >> 11) & 0x1F;
                uint16_t green = (color >> 5) & 0x3F;
                uint16_t blue = color & 0x1F; 
                red = red * shade;
                green = green *shade;
                blue = blue * shade;

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
                        
                        if (SCREEN_HEIGHT > height) {
                            extapp_pushRectUniform((previousNbRect1 + i) * rectWidth, 0, rectWidth, 
                                                (SCREEN_HEIGHT - height) / 2, 0x0000);
                            extapp_pushRectUniform((previousNbRect1 + i) * rectWidth, 
                                                (SCREEN_HEIGHT + height) / 2, rectWidth,
                                                (SCREEN_HEIGHT - height) / 2, 0x0000);
                        }
                        
                       
                        extapp_pushRectUniform((previousNbRect1 + i) * rectWidth, 
                                            SCREEN_HEIGHT / 2 - height / 2,
                                            rectWidth, height, previousColor);
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
                extapp_pushRectUniform(nRect * rectWidth, 0, rectWidth, SCREEN_HEIGHT, 0x0000);
            }
        }

        if (extapp_isKeydown(KEY_CTRL_LEFT)) {
            playerAngle -= 0.1;
        }
        if (extapp_isKeydown(KEY_CTRL_RIGHT)) {
            playerAngle += 0.1;
        }
        if (extapp_isKeydown(KEY_CTRL_UP)) {
            double newX = playerX + cos(playerAngle) * 0.1;
            double newY = playerY + sin(playerAngle) * 0.1;
            if (getCell((int)newX, (int)newY) == 0) {
                playerX = newX;
                playerY = newY;
            }
        }
        if (extapp_isKeydown(KEY_CTRL_DOWN)) {
            double newX = playerX - cos(playerAngle) * 0.1;
            double newY = playerY - sin(playerAngle) * 0.1;
            if (getCell((int)newX, (int)newY) == 0) {
                playerX = newX;
                playerY = newY;
            }
        }
    }
}
