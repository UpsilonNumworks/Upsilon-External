#ifndef FLAPPY_TEXTURES_OBM_H_
#define FLAPPY_TEXTURES_OBM_H

#include <stdint.h>


// Define the OBMHeader structure
struct OBMHeader {
    uint16_t signature;
    uint16_t width;
    uint16_t height;
    const uint16_t* pixels;
};

typedef struct OBMHeader obm_t;


#endif
