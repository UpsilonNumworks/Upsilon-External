#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "src/libspng/spng.h"
#include "extapp_api.h"

extern uint32_t _heap_size;
extern void *_heap_base;
extern void *_heap_ptr;


void int_to_hex(char* str, int value) {
    const char* hex_digits = "0123456789ABCDEF";
    for (int i = 0; i < 4; i++) {
        str[i * 2] = hex_digits[(value >> (8 * (3 - i) + 4)) & 0xF];
        str[i * 2 + 1] = hex_digits[(value >> (8 * (3 - i))) & 0xF];
    }
    str[8] = '\0';
}

void my_sprintf(char* str, const char* format, ...) {
    va_list args;
    va_start(args, format);

    for (const char *p = format; *p != '\0'; p++) {
        if (*p == '%') {
            p++;
            switch (*p) {
                case 'd': {
                    int i = va_arg(args, int);
                    char buffer[32];
                    itoa(i, buffer, 10);
                    strcat(str, buffer);
                    break;
                }
                case 'x': {
                    int i = va_arg(args, int);
                    char buffer[3];
                    int_to_hex(buffer, i);
                    strcat(str, buffer);
                    break;
                }
                case 's': {
                    char *s = va_arg(args, char*);
                    strcat(str, s);
                    break;
                }
                default:
                    break;
            }
        } else {
            size_t len = strlen(str);
            str[len] = *p;
            str[len + 1] = '\0';
        }
    }

    va_end(args);
}

void extapp_main() {

    chat extapp_palette[50];
    extapp_palette[0] = extapp_get_palette();

    char extapp_textBuffer[50];
    extapp_drawTextSmall(extapp_textBuffer, 100, 0, 0xFFFF, 0x0000, false);
    size_t len;
    const char *filename = "tux.png";
    const char *file_content = extapp_fileRead(filename, &len, EXTAPP_FLASH_FILE_SYSTEM);

    if (!file_content) {
        extapp_drawTextSmall("Failed to read file", 0, 0, 0xFFFF, 0x0000, false);
        return;
    }

    extapp_textBuffer[0] = '\0';
    extapp_drawTextSmall("File read successfully", 0, 0, 0xFFFF, 0x0000, false);
    my_sprintf(extapp_textBuffer, "File size: %d", len);
    extapp_drawTextSmall(extapp_textBuffer, 0, 20, 0xFFFF, 0x0000, false);
    extapp_msleep(2000);

    // Decode the PNG image to get the size and print it
    spng_ctx *ctx = spng_ctx_new(0);
    if (!ctx) {
        extapp_drawTextSmall("Failed to create spng context", 0, 40, 0xFFFF, 0x0000, false);
        return;
    }

    spng_set_png_buffer(ctx, file_content, len);
    if (spng_decoded_image_size(ctx, SPNG_FMT_RGB8, &len)) {
        extapp_drawTextSmall("Failed to get image size", 0, 40, 0xFFFF, 0x0000, false);
        return;
    }
    //Print the image size
    extapp_textBuffer[0] = '\0';
    my_sprintf(extapp_textBuffer, "Image size: %d", len);
    extapp_drawTextSmall(extapp_textBuffer, 0, 40, 0xFFFF, 0x0000, false);
    extapp_msleep(2000);

    // Blank extapp textBuffer to avoid any previous text
    extapp_textBuffer[0] = '\0';
    // Take the image length and print it
    struct spng_ihdr ihdr;
    if (spng_get_ihdr(ctx, &ihdr) != SPNG_OK) {
        printf("Failed to get image header\n");
        spng_ctx_free(ctx);
        return;
    }
    my_sprintf(extapp_textBuffer, "Image width and height: %dx%d", ihdr.width, ihdr.height);
    extapp_drawTextSmall(extapp_textBuffer, 0, 60, 0xFFFF, 0x0000, false);
    extapp_msleep(2000);

    // Print the first pixel of the image
    uint8_t *image = malloc(len);
    int heap_size = _heap_size;
    int heap_base = _heap_base;
    int heap_ptr = _heap_ptr;

    int ReamaingSize = (heap_size - (heap_ptr - heap_base));
    extapp_textBuffer[0] = '\0';
    my_sprintf(extapp_textBuffer, "ReamaingSize: %d", ReamaingSize);
    extapp_drawTextSmall(extapp_textBuffer, 0, 80, 0xFFFF, 0x0000, false);
    extapp_msleep(2000);

    if (spng_decode_image(ctx, image, len, SPNG_FMT_RGB8, 0)) {
        extapp_drawTextSmall("Failed to decode image", 0, 80, 0xFFFF, 0x0000, false);
        return;
    }

    //draw the image on the screen by printing each pixel
    for (int x = 0; x < ihdr.width; x++) {
        for (int y = 0; y < ihdr.height; y++) {
            uint8_t *pixel = image + (y * ihdr.width + x) * 3;
            // Convert the pixel to a rgb565 color
            uint16_t color = ((pixel[0] >> 3) << 11) | ((pixel[1] >> 2) << 5) | (pixel[2] >> 3);
            extapp_pushRectUniform(x, y, 1, 1, color);
        }
    }
    extapp_msleep(5000);
}