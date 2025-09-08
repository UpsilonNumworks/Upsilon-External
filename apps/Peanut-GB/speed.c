// Avoid compiling Peanut-GB as it's already compiled in main.c
#include <stdbool.h>
#define PEANUT_GB_HEADER_ONLY
#include <extapp_api.h>

#include <stdio.h>

#include "speed.h"

#include "main.h"
#include "peanut_gb.h"

#define NW_LCD_WIDTH 320
#define NW_LCD_HEIGHT 240

// Manage time budget and sleep if too slow
inline void frame_limiter(struct gb_s * gb, uint16_t MSpF) {
    struct priv_t * priv = gb->direct.priv;

    #if ENABLE_FRAME_LIMITER
    uint32_t differenceToTarget = abs(TARGET_FRAME_DURATION - MSpF);
    if (TARGET_FRAME_DURATION - MSpF > 0) {
        // Frame was faster than target, so let's slow down if we have time to
        // catch up

        if (priv->timeBudget >= differenceToTarget) {
            // We were too slow at previous frames so we have to catch up
            priv->timeBudget -= differenceToTarget;
        } else if (priv->timeBudget > 0) {
            // We can catch up everything on one frame, so let's sleep a bit less
            // than what we would have done if we weren't late
            uint32_t time_to_sleep = differenceToTarget - priv->timeBudget;
            extapp_msleep(time_to_sleep);
            priv->timeBudget = 0;
        } else {
            // We don't have time to catch up, so we just sleep until we get to 16ms/f
            extapp_msleep(differenceToTarget);
        }
    } else {
        // Frame was slower than target, so we need to catch up.
        priv->timeBudget += differenceToTarget;

        if (priv->timeBudget >= TARGET_FRAME_DURATION) {
            priv->timeBudget = TARGET_FRAME_DURATION;
        }
    }
    #endif
}

inline void frame_skipper(struct gb_s * gb) {
    struct priv_t * priv = gb->direct.priv;
    if (priv->timeBudget == 0) {
        // Disable frame skipping as we are running faster than required
        gb->direct.frame_skip = 0;
        // gb->display.frame_skip_count = true;
        // gb->direct.interlace = 0;
    } else if (priv->timeBudget >= TARGET_FRAME_DURATION / 2) {
        // Switch to 30 FPS after half a frame of lag
        // Enable frame skipping in an attempt to speed up emulation
        gb->direct.frame_skip = 1;

        if (priv->timeBudget >= TARGET_FRAME_DURATION) {
            // If MSpF is still below threshold, switch to 1/4 (15 FPS) frame
            // skipping, instead of the emulator 1/2 (30 FPS) frame skipping.
            // It provide similar savings to interlacing, but less visible

            // TODO: Experiment with 1/3 skipping (20 FPS) to balance between 30
            // and 15 FPS.
            priv->currentFrame++;
            if (priv->currentFrame >= 4) {
                priv->currentFrame = 0;
            }

            gb->display.frame_skip_count = !priv->currentFrame;
            // Interlacing is not used as it's really ugly, especially with low
            // frame rates.
            // We can't improve much performance when we are at 15 FPS, as the
            // display overhead has already been reduced quite a lot (by a
            // factor 4), and we can't improve much more as:
            // 1. Display overhead is near-zero: on my calculator, the difference
            //    between no display at all (11.2 ms/f) and 60 FPS fullscreen
            //    display (21.2 ms/f) is 10 ms, so 15 FPS fullscreen would be
            //    2.5 ms of overhead. The game itself is using too much CPU
            // 2. Reducing more aggressively would really hurt UX (to get a
            //    real improvement, we would need to divide by 2 the framerate,
            //    so 7.5 FPS)
            // 3. Subpixel rendering used for antialiased fullscreen display
            //    doesn't work with antialiasing (to avoid reading directly from
            //    the slow framebuffer)
            // gb->direct.interlace = 1;
        }
    }
}

static inline void update_MSpF(struct priv_t * priv, uint16_t MSpF) {
    // Last frames durations
    static uint16_t historyMSpF[MSPF_WINDOW];

    // Index of last written value in historyMSpF
    static uint8_t currentIndexMSpf = 0;

    currentIndexMSpf++;
    if (currentIndexMSpf >= MSPF_WINDOW) {
        currentIndexMSpf = 0;
    }
    historyMSpF[currentIndexMSpf] = MSpF;

    if (priv->MSpFfCounter) {
        // We need to average the MSpF as skipped frames are faster
        // TODO: Compare with real moving average algorithms
        // Compute the average over the array
        uint32_t total = 0;
        for (uint32_t index = 0; index < MSPF_WINDOW; index++) {
            total += historyMSpF[index];
        }

        // Allocate a buffer on the stack to store the displayed string
        char buffer[100];

        #if MSPF_DEBUG
        uint32_t average = total * 1000 / MSPF_WINDOW;
        sprintf(buffer, "%d us/f, %d, %d  ", average, priv->currentFrame, priv->timeBudget);
        #else
        uint32_t average = total / MSPF_WINDOW;
        sprintf(buffer, "%d ms/f", average);
        #endif

        extapp_drawTextSmall(buffer, 2, NW_LCD_HEIGHT - 10, 65535, 0, false);
    }
}

// Handle everything timing-related in the emulation, like frame limiter,
// frame skipping, interlacing...
inline void speed_handler(struct gb_s * gb, uint16_t MSpF) {
    struct priv_t * priv = gb->direct.priv;

    update_MSpF(priv, MSpF);

    #if ENABLE_FRAME_LIMITER
    frame_limiter(gb, MSpF);
    #endif

    #if AUTOMATIC_FRAME_SKIPPING
    frame_skipper(gb);
    #endif
}
