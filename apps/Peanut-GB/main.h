#ifndef MAIN_H
#define MAIN_H

#include <stdbool.h>
#include <stdint.h>


#define PEANUT_GB_HIGH_LCD_ACCURACY 0
#define PEANUT_GB_12_COLOUR 0

#define MAX_SCRIPTSTORE_SIZE 8192

#define SAVE_COOLDOWN 120

#define NW_LCD_WIDTH 320
#define NW_LCD_HEIGHT 240

#define DUMMY_ROM 0
#define DUMMY_ROM_NAME Tetris

// Sleep to maintain a maximum framerate (dynamic, can catch up to 1 full frame
// duration of lag from previous frames)
#define ENABLE_FRAME_LIMITER 1

// Minimum average frame duration (on original Game Boy: 59.73 FPS = 16.74 ms,
// but our timer has only 1 ms accuracy)
#define TARGET_FRAME_DURATION 16

// Automatically skip frames if the emulator speed is too low to maintain the
// target frame duration, require frame limiter, usually safe to enable, didn't
// cause any visible issues on my testing, but break perhaps some animations
// (the emulator itself can't render some effects anyway)
#define AUTOMATIC_FRAME_SKIPPING 1

// Number of samples used to compute the average MSpF, minimum 1, should be a
// multiple of maximum frameskip (currently 4, as 1 of 4 frames are displayed
// when lagging, allowing a 15 FPS rendering). Higher values allow more precise
// benchmark with MSPF_DEBUG=1
#define MSPF_WINDOW 4

// Switch MSPF to us instead of ms, plus some debugging info about frame
// skipping and time budget. Increase MSPF_WINDOW to get more precise results
// (original speed data is in ms, but with a larger sample size, it's possible
// to achieve us precision on average)
#define MSPF_DEBUG 0


struct priv_t {
    // Pointer to allocated memory holding GB file.
    const uint8_t *rom;
    // Pointer to allocated memory holding save file.
    uint8_t *cart_ram;

    // Weather to show MSpF
    bool MSpFfCounter;

    #if ENABLE_FRAME_LIMITER
    // We use a "smart" frame limiter: for each frame, we add
    // `frame duration - target frame time` to our budget. If the frame was faster
    // than target, we sleep for (simplified version without taking the case where
    // time budget > target frame time - last frame duration):
    // target frame time - last frame duration - time budget
    // This way, we will keep an average frame duration consistant.
    uint32_t timeBudget;

    // Current frame for 15 FPS frame skipping, emulator can't handle it
    // internally
    uint8_t currentFrame;
    #endif
};

extern const uint16_t * palette;
#endif

