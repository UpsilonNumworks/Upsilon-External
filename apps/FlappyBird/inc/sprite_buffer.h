#ifndef FLAPPY_SPRITE_BUFFER_H_
#define FLAPPY_SPRITE_BUFFER_H_

#include "libraries.h"
#include "render.h"
#include "math_functions.h"

typedef struct SpriteBuffer {
    obm_t* sprite;
    int x;
    int y;
    double angle;
    double c_a;
    double s_a;
    struct SpriteBuffer* next;
} sprite_buffer_t;

void pushSprite(sprite_buffer_t** head, obm_t* sprite, int x, int y, double angle);
void displaySpriteBuffer(sprite_buffer_t** head);
#endif