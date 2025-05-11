// #include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
// #include <stdio.h>
#include <string.h>
#include "extapp_api.h"

#include "inc/peripherals.h"
#include "inc/selector.h"
#include "lz4.h"


uint8_t select_mode() {
    extapp_drawTextLarge("Select mode:", 0, 20 * 1, 0x0000, 0xFFFF, false);
    extapp_drawTextLarge("1. Compress", 0, 20 * 2, 0x0000, 0xFFFF, false);
    extapp_drawTextLarge("2. Decompress", 0, 20 * 3, 0x0000, 0xFFFF, false);

    while (true) {
        uint64_t scancode = extapp_scanKeyboard();
        if (scancode & SCANCODE_One) {
            return 1;
        } else if (scancode & SCANCODE_Two) {
            return 2;
        } else if (scancode & (SCANCODE_Back | SCANCODE_Home | SCANCODE_OnOff)) {
            // Exit
            return 0;
        }

        extapp_msleep(10);
    }
}


void compress() {
    // Select file
    init_display();
    const char * filename = select_file("py", 100);
    waitForKeyReleased();
    init_display();
    if (filename == NULL) {
        return;
    }

    extapp_drawTextLarge("Compressing:", 0, 20 * 1, 0x0000, 0xFFFF, false);
    extapp_drawTextLarge(filename, 0, 20 * 2, 0x0000, 0xFFFF, false);

    size_t file_len = 0;
    // We need to do -1 on the content to get the whole file (with autoimport byte)
    const char * filecontent = extapp_fileRead(filename, &file_len, EXTAPP_RAM_FILE_SYSTEM) - 1;
    file_len++; // Increase file len as we modified our pointer

    // Compressed shouldn't be bigger than source
    char* output = malloc((size_t)file_len + 10);

    int compressed_size = LZ4_compress_default(filecontent, output, file_len, (size_t)file_len + 10);

    char* output_filename = malloc(strlen(filename) + 4);
    strcpy(output_filename, filename);
    strcat(output_filename, "lz4");


    if (compressed_size > 0) {
        if (extapp_fileExists(output_filename, EXTAPP_RAM_FILE_SYSTEM)) {
            extapp_drawTextLarge("Error: Output file already exists", 0, 20 * 4, 0x0000, 0xFFFF, false);
            extapp_drawTextLarge("Press any key to exit", 0, 20 * 5, 0x0000, 0xFFFF, false);
            waitForKeyPressed();
            waitForKeyReleased();

            return;
        }

        extapp_fileErase(filename, EXTAPP_RAM_FILE_SYSTEM);
        if (extapp_fileWrite(output_filename, output, compressed_size, EXTAPP_RAM_FILE_SYSTEM)) {
            extapp_drawTextLarge("Compression successful!", 0, 20 * 4, 0x0000, 0xFFFF, false);
        } else {
            extapp_drawTextLarge("Error: Couldn't write file", 0, 20 * 4, 0x0000, 0xFFFF, false);
            extapp_drawTextLarge("Press any key to continue", 0, 20 * 5, 0x0000, 0xFFFF, false);
            waitForKeyPressed();
            waitForKeyReleased();
        }
    } else {
        extapp_drawTextLarge("Error: Compression failed", 0, 20 * 4, 0x0000, 0xFFFF, false);
        extapp_drawTextLarge("Press any key to continue", 0, 20 * 5, 0x0000, 0xFFFF, false);
        waitForKeyPressed();
        waitForKeyReleased();
    }
}


void decompress() {
    // Select file
    init_display();
    const char * filename = select_file("pylz4", 100);
    waitForKeyReleased();
    init_display();
    if (filename == NULL) {
        return;
    }

    extapp_drawTextLarge("Decompressing:", 0, 20 * 1, 0x0000, 0xFFFF, false);
    extapp_drawTextLarge(filename, 0, 20 * 2, 0x0000, 0xFFFF, false);

    size_t file_len = 0;
    const char * filecontent = extapp_fileRead(filename, &file_len, EXTAPP_RAM_FILE_SYSTEM);

    // We allocate 60 KB of memory because we don't really need to care about
    // wasting memory
    char* output = malloc(60000);

    // int compressed_size = LZ4_compress_default(filecontent, output, file_len, (size_t)file_len + 10);
    int decompressed_size = LZ4_decompress_safe(filecontent, output, file_len, 60000);

    // Get the filename without the extension
    uint8_t len = strlen(filename);
    char* output_filename = malloc(len + 4);
    strcpy(output_filename, filename);
    // Remove lz4 extension
    output_filename[len - 3] = '\0';

    if (decompressed_size > 0) {
        if (extapp_fileExists(output_filename, EXTAPP_RAM_FILE_SYSTEM)) {
            extapp_drawTextLarge("Error: Output file already exists", 0, 20 * 4, 0x0000, 0xFFFF, false);
            extapp_drawTextLarge("Press any key to exit", 0, 20 * 5, 0x0000, 0xFFFF, false);
            waitForKeyPressed();
            waitForKeyReleased();

            return;
        }

        extapp_fileErase(filename, EXTAPP_RAM_FILE_SYSTEM);
        if (extapp_fileWrite(output_filename, output, decompressed_size, EXTAPP_RAM_FILE_SYSTEM)) {
            extapp_drawTextLarge("Decompression successful!", 0, 20 * 4, 0x0000, 0xFFFF, false);
        } else {
            extapp_drawTextLarge("Error: Couldn't write file", 0, 20 * 4, 0x0000, 0xFFFF, false);
            extapp_drawTextLarge("Press any key to continue", 0, 20 * 5, 0x0000, 0xFFFF, false);
            waitForKeyPressed();
            waitForKeyReleased();
        }
    } else {
        extapp_drawTextLarge("Error: Decompression failed", 0, 20 * 4, 0x0000, 0xFFFF, false);
        extapp_drawTextLarge("Press any key to continue", 0, 20 * 5, 0x0000, 0xFFFF, false);
        waitForKeyPressed();
        waitForKeyReleased();
    }
}


void extapp_main(void) {
    waitForKeyReleased();
    init_display();

    updateTitlebar("COMPRESSOR");

    // Select mode
    uint8_t mode = select_mode();
    waitForKeyReleased();
    if (mode == 0) {
        return;
    }

    // Do the action
    init_display();
    switch (mode) {
        case 1:
            compress();
            break;
        case 2:
            decompress();
            break;
        default:
            extapp_drawTextLarge("Error, unknown mode", 0, 20 * 1, 0x0000, 0xFFFF, false);
            extapp_drawTextLarge("Press any key to continue", 0, 20 * 2, 0x0000, 0xFFFF, false);

            waitForKeyPressed();
            waitForKeyReleased();
            break;
    }
}
