#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include <jpeglib.h>
#include <jerror.h>

#include "extapp_api.h"

#include "inc/peripherals.h"
#include "inc/selector.h"

#define MAX_IMAGE_WIDTH 1920
#define MOVING_OFFSET 10
#define DEBUG 0

static unsigned char decompress_buffer[MAX_IMAGE_WIDTH * 2];

struct state_t {
    const char * filename;

    const char * filecontent;
    const char * index;
    const char * end_of_file;
    size_t file_len;

    int frame_count;
    uint64_t start;

    bool running;
    bool paused;
    bool loop;

    uint64_t last_scancode;

    int16_t y_offset;
    int16_t x_offset;
    unsigned int imgHeight;
    unsigned int imgWidth;
};

static struct state_t state = {
    .filename = 0,
    .filecontent = 0,
    .index = 0,
    .end_of_file = 0,
    .file_len = 0,

    .start = 0,
    .frame_count = 0,

    .running = true,
    .paused = false,
    .loop = false,

    .last_scancode = 0,

    .y_offset = 0,
    .x_offset = 0,
    .imgHeight = 0,
    .imgWidth = 0,
};


uint32_t * find_next_frame(uint32_t * current_frame, uint32_t * end_of_file) {
    uint32_t * address = current_frame + 1;
    #pragma unroll 100
    while (address < end_of_file) {
        // https://github.com/AdityaNG/mjpeg_format
        // M-JPEG use 0xFFD0 as start of image marker, followed by 0xFFE0.
        // I should try to investigate weather something more optimized is
        // possible, but FFMpeg seems to use a similar way: https://code.ffmpeg.org/FFmpeg/FFmpeg/src/branch/release/8.0/libavcodec/mjpeg_parser.c#L53
        // (their code could probably be optimized using a bitmask, but M-JPEG
        // decoding on modern hardware able to run FFMpeg won't probably need
        // that much performance)
        // if (*address == 0xFFD8FFE0) {
        // We are in little endian, so the value is reversed
        if (*address == 0xE0FFD8FF) {
            return address;
        }

        // Be sure to cast the pointer otherwise it will increment 4 by 4 and
        // missing 3/4 of the frames
        address = (void *)address + 1;
    }
    return 0;
}

void handle_keyboard() {
    uint64_t scancode = extapp_scanKeyboard();
    uint64_t filtered_scancode = scancode & (~state.last_scancode);
    if (filtered_scancode & (SCANCODE_Home | SCANCODE_OnOff | SCANCODE_Back)) {
        state.running = false;
    }

    if (filtered_scancode & (SCANCODE_OK | SCANCODE_EXE)) {
        state.paused = !state.paused;
    }

    if (scancode & (SCANCODE_Toolbox)) {
        if (filtered_scancode & (SCANCODE_Toolbox)) {
            state.loop = !state.loop;
        }
        if (state.loop) {
            extapp_drawTextSmall("Loop mode enabled", 2, 240 - 10, 0xFFFF, 0x0000, false);
        } else {
            extapp_drawTextSmall("Loop mode disabled", 2, 240 - 10, 0xFFFF, 0x0000, false);
        }
    }

    if (scancode & (SCANCODE_Left)) {
        int16_t wipe_origin = state.x_offset + state.imgWidth - MOVING_OFFSET;
        if ((wipe_origin > 0) || (wipe_origin < LCD_WIDTH)) {
            extapp_pushRectUniform(wipe_origin, 0, MOVING_OFFSET, 240, 0x0000);
        }
        state.x_offset -= MOVING_OFFSET;
    }

    if (scancode & (SCANCODE_Right)) {
        int16_t wipe_origin = state.x_offset;
        if ((wipe_origin > 0) || (wipe_origin < LCD_WIDTH)) {
            extapp_pushRectUniform(wipe_origin, 0, MOVING_OFFSET, 240, 0x0000);
        }
        state.x_offset += MOVING_OFFSET;
    }

    if (scancode & (SCANCODE_Up)) {
        int16_t wipe_origin = state.y_offset + state.imgHeight - MOVING_OFFSET;
        if ((wipe_origin > 0) || (wipe_origin < LCD_WIDTH)) {
            extapp_pushRectUniform(0, wipe_origin, 320, MOVING_OFFSET, 0x0000);
        }
        state.y_offset -= MOVING_OFFSET;
    }

    if (scancode & (SCANCODE_Down)) {
        int16_t wipe_origin = state.y_offset;
        if ((wipe_origin > 0) || (wipe_origin < LCD_WIDTH)) {
            extapp_pushRectUniform(0, wipe_origin, 320, MOVING_OFFSET, 0x0000);
        }
        state.y_offset += MOVING_OFFSET;
    }

    state.last_scancode = scancode;
}

bool show_frame(struct jpeg_decompress_struct * info, const char * file, uint32_t file_size) {
    jpeg_mem_src(info, file, file_size);
    uint32_t bytes_to_add = info->src->bytes_in_buffer;

    if (jpeg_read_header(info, true) != JPEG_HEADER_OK) {
        init_display();
        extapp_drawTextLarge("Invalid file!", 0, 20 * 1, 0xFFFF, 0x0000, false);
        extapp_drawTextLarge("Press any key to exit", 0, 20 * 2, 0xFFFF, 0x0000, false);
        return false;
    }

    info->out_color_space = JCS_RGB565;

    jpeg_start_decompress(info);

    state.imgWidth = info->output_width;
    state.imgHeight = info->output_height;
    int numComponents = info->num_components;

    unsigned char *buffer_array[1] = {decompress_buffer};
    bool heap_allocated = false;
    if (state.imgWidth > MAX_IMAGE_WIDTH) {
        // If image is too big, fallback to heap allocation (using static memory
        // is "free" on Upsilon as it's a separate region)
        // Libjpeg already allocate some memory on the heap, so we should try to
        // avoid allocating on the heap (using static allocation, or stack
        // allocation for most use cases).
        buffer_array[0] = malloc(state.imgWidth * 2);
        heap_allocated = true;

        if (buffer_array[0] == 0) {
            init_display();
            extapp_drawTextLarge("Memory full", 0, 20 * 1, 0xFFFF, 0x0000, false);
            extapp_drawTextLarge("Press any key to exit", 0, 20 * 2, 0xFFFF, 0x0000, false);
            return false;
        }
    }


    while(info->output_scanline < info->output_height) {
        uint32_t read_lines = jpeg_read_scanlines(info, buffer_array, 1);

        uint16_t normalized_time_width = state.imgWidth > LCD_WIDTH ? LCD_WIDTH : state.imgWidth;

        int16_t x = state.x_offset;
        uint16_t width = normalized_time_width - state.x_offset;

        uint32_t array_offset = 0;
        if (state.x_offset < 0) {
            if (-state.x_offset > state.imgWidth) {
                continue;
            }
            x = 0;
            array_offset = -state.x_offset;
            width = state.imgWidth + state.x_offset;
            if (width > LCD_WIDTH) {
                width = LCD_WIDTH;
            }
        }

        int16_t y = info->output_scanline - 1 + state.y_offset;

        // We skip displaying images out of screen, but need to fully parse them
        // to prevent jpeg_finish_decompress from crashing. If we display out of
        // screen, the screen will wrap.
        if (y > LCD_HEIGHT || x > LCD_WIDTH || y < 0) {
            continue;
        }

        // uint16_t height = read_lines;
        uint16_t height = 1;
        extapp_pushRect(x, y, width, height, (const uint16_t *)buffer_array[0] + array_offset);
    }

    jpeg_finish_decompress(info);

    if (heap_allocated) {
        free(buffer_array[0]);
    }

    return true;
}

bool read_file() {
    state.filecontent = extapp_fileRead(state.filename, &state.file_len, EXTAPP_FLASH_FILE_SYSTEM);
    state.end_of_file = state.filecontent + state.file_len;
    state.index = state.filecontent;

    // Check if magic value is present at beginning of JPEG files.
    // This check is necessary to prevent freezing on invalid files
    if (*(uint32_t *)state.index != 0xE0FFD8FF) {
        extapp_drawTextLarge("Invalid file!", 0, 20 * 1, 0xFFFF, 0x0000, false);
        extapp_drawTextLarge("Press any key to exit", 0, 20 * 2, 0xFFFF, 0x0000, false);

        return false;
    }

    state.start = extapp_millis();

    // Tutorial at https://www.tspi.at/2020/03/20/libjpegexample.html#gsc.tab=0
    struct jpeg_decompress_struct info;
	struct jpeg_error_mgr err;

    info.err = jpeg_std_error(&err);
    jpeg_create_decompress(&info);

    state.frame_count = 0;
    state.running = true;
    while ((state.index < state.end_of_file) && state.running) {
        if (!show_frame(&info,  state.index, state.end_of_file - state.index)) {
            return false;
        }

        if (!state.paused) {
            state.frame_count += 1;

            // libjpeg already gives us the end of file, so we don't need to scan
            // the file. We still keep the scanning code commented in case it cause
            // problems, so it can be reverted (FFMpeg use scanning, so it's
            // probably useful in some cases)
            state.index = info.src->next_input_byte;

            // If index is invalid, use find_next_frame (can be invalid for example
            // when reading image is aborted, due to it being out of screen)
            if (*(uint32_t *)state.index != 0xE0FFD8FF) {
                state.index = (char *)find_next_frame((uint32_t *)state.index, (uint32_t *)(state.end_of_file));
            }

            if (state.loop && (state.index == 0) || (state.index >= state.end_of_file)) {
                state.index = state.filecontent;
            }


            if (state.index == 0) {
                break;
            }
        }

        handle_keyboard();

    }
    jpeg_destroy_decompress(&info);

    uint32_t duration = extapp_millis()-state.start;

    #if DEBUG
    char buffer[100];
    sprintf(buffer, "%d ms, %d frames", duration, state.frame_count);
    extapp_drawTextSmall(buffer, 2, 240 - 10, 0xFFFF, 0x0000, false);
    #endif

    return true;
}

void extapp_main(void) {
    waitForKeyReleased();
    init_display();

    state.filename = select_file("", 10);
    state.last_scancode = extapp_scanKeyboard();

    init_display();

    if (state.filename == NULL) {
        return;
    }


    read_file();

    #if !DEBUG
    if (state.running) {
    #endif
        waitForKeyPressed();
        waitForKeyReleased();
    #if !DEBUG
    }
    #endif
}
void _fini (void) {};
