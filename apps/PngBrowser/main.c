#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "src/libspng/spng.h"
#include "extapp_api.h"

extern uint32_t _heap_size;
extern void *_heap_base;
extern void *_heap_ptr;

void my_sprintf(char* str, const char* format, ...) {
    str[0] = '\0'; // Clear the string

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
/*
This function is used to draw the background and the text on the screen.
params:
    Filename: the name of the file to be displayed on the screen.
*/
void draw_screen(char* Filename, char* Dimension, int ImageNumber, int ImageTotal, int ToolbarPalette, int BackgroundPalette, int TextPalette) {
    extapp_pushRectUniform(0, 18, 320, 222, BackgroundPalette);
    extapp_pushRectUniform(0, 0, 320, 18, ToolbarPalette);
    if (ImageTotal > 1) {
        //Draw the arrows if there is more that one image.
        extapp_pushRectUniform(311, 114, 1, 13, ToolbarPalette);
        extapp_pushRectUniform(312, 115, 1, 11, ToolbarPalette);
        extapp_pushRectUniform(313, 116, 1, 9, ToolbarPalette);
        extapp_pushRectUniform(314, 117, 1, 7, ToolbarPalette);
        extapp_pushRectUniform(315, 118, 1, 5, ToolbarPalette);
        extapp_pushRectUniform(316, 119, 1, 3, ToolbarPalette);
        extapp_pushRectUniform(317, 120, 1, 1, ToolbarPalette);

        extapp_pushRectUniform(10, 114, 1, 13, ToolbarPalette);
        extapp_pushRectUniform(9, 115, 1, 11, ToolbarPalette);
        extapp_pushRectUniform(8, 116, 1, 9, ToolbarPalette);
        extapp_pushRectUniform(7, 117, 1, 7, ToolbarPalette);
        extapp_pushRectUniform(6, 118, 1, 5, ToolbarPalette);
        extapp_pushRectUniform(5, 119, 1, 3, ToolbarPalette);
        extapp_pushRectUniform(4, 120, 1, 1, ToolbarPalette);
    }
    //Draw the text on the screen.
    int FilenameLength = extapp_drawTextSmall(Filename, 0, 0, TextPalette, ToolbarPalette, true);
    extapp_drawTextSmall(Filename, (320/2) - (FilenameLength/2), 3, TextPalette, ToolbarPalette, false);
    extapp_drawTextSmall(Dimension, 7, 3, TextPalette, ToolbarPalette, false);
    char ImageNumberBuffer[50];
    my_sprintf(ImageNumberBuffer, "%d/%d", ImageNumber, ImageTotal);
    int ImageNumberLength = extapp_drawTextSmall(ImageNumberBuffer, 0, 0, TextPalette, ToolbarPalette, true);
    extapp_drawTextSmall(ImageNumberBuffer, 320 - 7 - ImageNumberLength, 3, TextPalette, ToolbarPalette, false);
}

void extapp_main() {

    //Take the palette of the using the pull function.
    int ToolbarPalette;
    extapp_pullRect(0, 0, 1, 1, &ToolbarPalette);
    int BackgroundPalette;
    extapp_pullRect(0, 18, 1, 1, &BackgroundPalette);
    int TextPalette;
    extapp_pullRect(299, 7, 1, 1, &TextPalette);

    //Quit the app if the exam mode is enabled.
    if (extapp_inexammode()) {
        draw_screen("Exam mode is enabled", " ", 0, 0, ToolbarPalette, BackgroundPalette, TextPalette);
        extapp_drawTextLarge("Exam mode enabled", 80, 111, TextPalette, BackgroundPalette, false);
        extapp_drawTextLarge("App closing in 2 seconds.", 45, 131, TextPalette, BackgroundPalette, false);
        extapp_msleep(2000);
        return;
    }
    //list of the images
    const char* filenames[8];  // Define an array of const char* pointers
    int NumberOfImage = extapp_fileListWithExtension(filenames, 8, ".png", EXTAPP_FLASH_FILE_SYSTEM);

    if (NumberOfImage == 0) {
        draw_screen("No images found", " ", 0, 0, ToolbarPalette, BackgroundPalette, TextPalette);
        extapp_drawTextLarge("No images found", 80, 111, TextPalette, BackgroundPalette, false);
        extapp_drawTextLarge("App closing in 2 seconds.", 45, 131, TextPalette, BackgroundPalette, false);
        extapp_msleep(2000);
        return;
    }

    //Start a loop that will display the images.
    int Loop = 1;
    int ImageNum = 0;
    int ImageTotal = NumberOfImage;
    int Drawn = 1;
    int Zoom = 1;
    while (Loop == 1) {
        //Test if the user want to exit the app.
        if (extapp_isKeydown(5) || extapp_isKeydown(6) || extapp_isKeydown(7) || extapp_isKeydown(52)) {
            Loop = 0;
            break;
        }
        if (Drawn == 1) {
            int freeImage = 0;
            int freeCtx = 0;
            //Read the image from the file.
            size_t len;
            const char *file_content = extapp_fileRead(filenames[ImageNum], &len, EXTAPP_FLASH_FILE_SYSTEM);
            if (!file_content) {
                draw_screen(filenames[ImageNum], " ", ImageNum + 1, ImageTotal, ToolbarPalette, BackgroundPalette, TextPalette);
                extapp_drawTextLarge("Failed to read file", 80, 111, TextPalette, BackgroundPalette, false);
                Drawn = 0;
                goto Error;
            }
            //Decode the image.
            spng_ctx *ctx = spng_ctx_new(0);
            if (!ctx) {
                draw_screen(filenames[ImageNum], " ", ImageNum + 1, ImageTotal, ToolbarPalette, BackgroundPalette, TextPalette);
                extapp_drawTextLarge("Failed to create spng context", 80, 111, TextPalette, BackgroundPalette, false);
                Drawn = 0;
                goto Error;

            }
            spng_set_png_buffer(ctx, file_content, len);
            if (spng_decoded_image_size(ctx, SPNG_FMT_RGB8, &len)) {
                draw_screen(filenames[ImageNum], " ", ImageNum + 1, ImageTotal, ToolbarPalette, BackgroundPalette, TextPalette);
                extapp_drawTextLarge("Failed to get image size", 80, 111, TextPalette, BackgroundPalette, false);
                Drawn = 0;
                goto Error;
            }
            //Take the image size
            char DimensionBuffer[50];
            struct spng_ihdr ihdr;
            if (spng_get_ihdr(ctx, &ihdr)) {
                draw_screen(filenames[ImageNum], " ", ImageNum + 1, ImageTotal, ToolbarPalette, BackgroundPalette, TextPalette);
                extapp_drawTextLarge("Failed to get image size", 80, 111, TextPalette, BackgroundPalette, false);
                Drawn = 0;
                goto Error;
            }
            if (ihdr.width * ihdr.height > 8100) {
                draw_screen(filenames[ImageNum], " ", ImageNum + 1, ImageTotal, ToolbarPalette, BackgroundPalette, TextPalette);
                extapp_drawTextLarge("Image is too large", 80, 111, TextPalette, BackgroundPalette, false);
                Drawn = 0;
                goto Error;
            }
            my_sprintf(DimensionBuffer, "%dx%d", ihdr.width, ihdr.height);
            draw_screen(filenames[ImageNum], DimensionBuffer, ImageNum + 1, ImageTotal, ToolbarPalette, BackgroundPalette, TextPalette);

            //Print the image
            uint8_t *image = malloc(len);
            freeImage = 1;
            if (!image) {
                draw_screen(filenames[ImageNum], " ", ImageNum + 1, ImageTotal, ToolbarPalette, BackgroundPalette, TextPalette);
                extapp_drawTextLarge("Failed to allocate memory", 80, 111, TextPalette, BackgroundPalette, false);
                Drawn = 0;
                goto Error;
            }
            if (spng_decode_image(ctx, image, len, SPNG_FMT_RGB8, 0)) {
                draw_screen(filenames[ImageNum], " ", ImageNum + 1, ImageTotal, ToolbarPalette, BackgroundPalette, TextPalette);
                extapp_drawTextLarge("Failed to decode image", 80, 111, TextPalette, BackgroundPalette, false);
                Drawn = 0;
                goto Error;
            }
            freeCtx = 1;
            //Draw the image on the screen.

            if (Zoom == 1) {
                int OffsetX = 320 / 2 - ihdr.width / 2;
                int OffsetY = 220 / 2 - ihdr.height / 2 + 18;
                for (int x = 0; x < ihdr.width; x++) {
                    for (int y = 0; y < ihdr.height; y++) {
                        uint8_t *pixel = image + (y * ihdr.width + x) * 3;
                        // Convert the pixel to a rgb565 color
                        uint16_t color = ((pixel[0] >> 3) << 11) | ((pixel[1] >> 2) << 5) | (pixel[2] >> 3);
                        extapp_pushRectUniform(OffsetX + x, OffsetY + y, 1, 1, color);
                    }
                }
            } else {
                int OffsetX = 320 / 2 - ihdr.width;
                int OffsetY = 220 / 2 - ihdr.height + 18;
                for (int x = 0; x < ihdr.width; x++) {
                    for (int y = 0; y < ihdr.height; y++) {
                        uint8_t *pixel = image + (y * ihdr.width + x) * 3;
                        // Convert the pixel to a rgb565 color
                        uint16_t color = ((pixel[0] >> 3) << 11) | ((pixel[1] >> 2) << 5) | (pixel[2] >> 3);
                        extapp_pushRectUniform(OffsetX + x * 2, OffsetY + y * 2, 2, 2, color);
                    }
                }
            }
            //Goto here if there is a problem with the image.
            Error:
            Drawn = 0;
            if (freeImage) {
                free(image);
                freeImage = 0;
            }
            if (freeCtx) {
                spng_ctx_free(ctx);
                freeCtx = 0;
            }

        }

        //Test if the user want to change image.
        if (extapp_isKeydown(3)) {
            if (ImageTotal > 1) {
                ImageNum++;
                if (ImageNum >= ImageTotal) {
                    ImageNum = 0;
                }
                Drawn = 1;
                extapp_msleep(200);
            }
        }
        if (extapp_isKeydown(0)) {
            if (ImageTotal > 1) {
                ImageNum--;
                if (ImageNum < 0) {
                    ImageNum = ImageTotal - 1;
                }
                Drawn = 1;
                extapp_msleep(200);
            }
        }
        if (extapp_isKeydown(46)) {
            Zoom = 1;
            Drawn = 1;
            extapp_msleep(200);
        }
        if (extapp_isKeydown(45)) {
            Zoom = 0;
            Drawn = 1;
            extapp_msleep(200);
        }
    }
    return 0;
}