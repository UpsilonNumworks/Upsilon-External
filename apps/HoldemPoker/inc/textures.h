#ifndef HOLDEM_TEXTURES_OBM_H_
#define HOLDEM_TEXTURES_OBM_H

#include <stdint.h>

#define CARD_HEIGHT 75
#define CARD_WIDTH 52
#define CARD_INCLINE 0

#define TOK_HEIGHT 5

// Define the OBMHeader structure
struct OBMHeader {
    uint16_t signature;
    uint16_t width;
    uint16_t height;
    const uint16_t* pixels;
};

typedef struct OBMHeader obm_t;


#endif
