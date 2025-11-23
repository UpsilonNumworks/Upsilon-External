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

static unsigned char decompress_buffer[MAX_IMAGE_WIDTH * 2];

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

bool show_frame(struct jpeg_decompress_struct * info, const char * file, uint32_t file_size) {
    jpeg_mem_src(info, file, file_size);
    uint32_t bytes_to_add = info->src->bytes_in_buffer;

    if (jpeg_read_header(info, true) != JPEG_HEADER_OK) {
        init_display();
        extapp_drawTextLarge("Invalid file!", 0, 20 * 1, 0x0000, 0xFFFF, false);
        extapp_drawTextLarge("Press any key to exit", 0, 20 * 2, 0x0000, 0xFFFF, false);
        return false;
    }

    info->out_color_space = JCS_RGB565;

    jpeg_start_decompress(info);

    unsigned int imgWidth = info->output_width;
    unsigned int imgHeight = info->output_height;
    int numComponents = info->num_components;

    unsigned char *buffer_array[1] = {decompress_buffer};
    bool heap_allocated = false;
    if (imgWidth > MAX_IMAGE_WIDTH) {
        // If image is too big, fallback to heap allocation (using static memory
        // is "free" on Upsilon as it's a separate region)
        // Libjpeg already allocate some memory on the heap, so we should try to
        // avoid allocating on the heap (using static allocation, or stack
        // allocation for most use cases).
        buffer_array[0] = malloc(imgWidth * 2);
        heap_allocated = true;

        if (buffer_array[0] == 0) {
            init_display();
            extapp_drawTextLarge("Memory full", 0, 20 * 1, 0x0000, 0xFFFF, false);
            extapp_drawTextLarge("Press any key to exit", 0, 20 * 2, 0x0000, 0xFFFF, false);
            return false;
        }
    }


    while(info->output_scanline < info->output_height) {
        uint32_t read_lines = jpeg_read_scanlines(info, buffer_array, 1);

        // We skip displaying images out of screen, but need to fully parse them
        // to prevent jpeg_finish_decompress from crashing. If we display out of
        // screen, the screen will wrap.
        if (info->output_scanline <= LCD_HEIGHT) {
            extapp_pushRect(0, info->output_scanline - 1, imgWidth > LCD_WIDTH ? LCD_WIDTH : imgWidth, read_lines, (const uint16_t *)buffer_array[0]);
        }
    }

    jpeg_finish_decompress(info);

    if (heap_allocated) {
        free(buffer_array[0]);
    }

    return true;
}

bool read_file(const char * filename) {
    size_t file_len = 0;
    const char * filecontent = extapp_fileRead(filename, &file_len, EXTAPP_FLASH_FILE_SYSTEM);

    // Check if magic value is present at beginning of JPEG files.
    // This check is necessary to prevent freezing on invalid files
    if (*(uint32_t *)filecontent != 0xE0FFD8FF) {
        extapp_drawTextLarge("Invalid file!", 0, 20 * 1, 0x0000, 0xFFFF, false);
        extapp_drawTextLarge("Press any key to exit", 0, 20 * 2, 0x0000, 0xFFFF, false);

        return false;
    }

    uint64_t start = extapp_millis();

    // Tutorial at https://www.tspi.at/2020/03/20/libjpegexample.html#gsc.tab=0
    struct jpeg_decompress_struct info;
	struct jpeg_error_mgr err;

    info.err = jpeg_std_error(&err);
    jpeg_create_decompress(&info);

    int frame_count = 0;
    char * index = filecontent;
    while (index < (filecontent + file_len)) {
        if (!show_frame(&info, index, file_len + filecontent - index)) {
            return false;
        }
        frame_count += 1;

        // libjpeg already gives us the end of file, so we don't need to scan
        // the file. We still keep the scanning code commented in case it cause
        // problems, so it can be reverted (FFMpeg use scanning, so it's
        // probably useful in some cases)
        index = info.src->next_input_byte;

        // If index is invalid, use find_next_frame (can be invalid for example
        // when reading image is aborted, due to it being out of screen)
        if (*(uint32_t *)index != 0xE0FFD8FF) {
            index = (char *)find_next_frame((uint32_t *)index, (uint32_t *)(filecontent + file_len));
        }

        if (index == 0) {
            break;
        }
    }
    jpeg_destroy_decompress(&info);

    uint64_t end = extapp_millis();

    uint32_t duration = end-start;
    char buffer[100];
    sprintf(buffer, "%d ms, %d frames", duration, frame_count);
    extapp_drawTextSmall(buffer, 2, 240 - 10, 65535, 0, false);

    return true;
}

void extapp_main(void) {
    waitForKeyReleased();
    init_display();

    const char * filename = select_file("", 10);
    waitForKeyReleased();

    init_display();

    if (filename == NULL) {
        return;
    }

    read_file(filename);

    waitForKeyPressed();
    waitForKeyReleased();
}
void _fini (void) {};
