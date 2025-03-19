#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "extapp_api.h"

#include "inc/peripherals.h"
#include "inc/selector.h"

void extapp_main(void) {

    // Wait for the key to be released before starting the application
    waitForKeyReleased();
    // Draw a white background
    init_display();
    // Draw hello world message
    extapp_drawTextLarge("Hello World !", 0, 20 * 1, 0x0000, 0xFFFF, false);
    extapp_drawTextLarge("Press any key to continue", 0, 20 * 2, 0x0000, 0xFFFF, false);

    // Just wait for a key to be pressed (defined in peripherals.c)
    waitForKeyPressed();
    waitForKeyReleased();

    // Select a file (The code is in selector.c)
    // The code is a bit more complicated, but you can see how to use the file system API to read a file
    const char * filename = select_file("", 50);
    // Wait for the key to be released
    waitForKeyReleased();

    // Clear the screen
    init_display();

    // If no file was selected, exit the application
    if (filename == NULL) {
        init_display();
        extapp_drawTextLarge("No file selected !", 0, 20 * 1, 0x0000, 0xFFFF, false);
        extapp_drawTextLarge("Press any key to exit", 0, 20 * 2, 0x0000, 0xFFFF, false);
        waitForKeyPressed();
        waitForKeyReleased();
        return;
    }
    // Show the selected file name
    extapp_drawTextLarge("You selected :", 0, 20 * 1, 0x0000, 0xFFFF, false);
    extapp_drawTextLarge(filename, 0, 20 * 2, 0x0000, 0xFFFF, false);


    extapp_drawTextLarge("Reading file...", 0, 20 * 4, 0x0000, 0xFFFF, false);
    // Initialize the file len variable that will be written by the file read function
    size_t file_len = 0;
    // Read the file into filecontent variable
    const char * filecontent = extapp_fileRead(filename, &file_len, EXTAPP_FLASH_FILE_SYSTEM);

    extapp_drawTextLarge("Press any key to show file", 0, 20 * 5, 0x0000, 0xFFFF, false);
    extapp_drawTextLarge("content", 0, 20 * 6, 0x0000, 0xFFFF, false);

    // Wait for a key to be pressed (defined in peripherals.c)
    waitForKeyPressed();
    waitForKeyReleased();

    // Clear the screen
    init_display();

    extapp_drawTextLarge("File content :", 0, 20 * 1, 0x0000, 0xFFFF, false);
    // It crashes on simulator, because Built-In app content is NULL
    if (filecontent != NULL) {
        extapp_drawTextLarge(filecontent, 0, 20 * 2, 0x0000, 0xFFFF, false);
    } else {
        extapp_drawTextLarge("File content is NULL", 0, 20 * 2, 0x0000, 0xFFFF, false);
    }

    extapp_drawTextLarge("Press any key to continue", 0, 20 * 3, 0x0000, 0xFFFF, false);
    // Wait for a key to be pressed
    waitForKeyPressed();
    waitForKeyReleased();

    // Clear the screen
    init_display();

    extapp_drawTextLarge("Writing file into ram filesystem...", 0, 20 * 1, 0x0000, 0xFFFF, false);

    // Initialize the filename to write to the ram filesystem
    const char * filename_to_write = "hello_world.py";
    // Initialize the file content to write to the ram filesystem, add three spaces at the beginning
    // because the Python app use the first character to store the auto importation status, and the
    // second and third characters to store the cursor position as an uint16_t
    const char * filecontent_to_write = " hello world";

    // Write the file into the ram filesystem
    extapp_fileWrite(filename_to_write, filecontent_to_write, strlen(filecontent_to_write) + 1, EXTAPP_RAM_FILE_SYSTEM);
    extapp_drawTextLarge("File written to ram filesystem", 0, 20 * 2, 0x0000, 0xFFFF, false);
    extapp_drawTextLarge("Press any key to exit", 0, 20 * 3, 0x0000, 0xFFFF, false);
    // Wait for a key to be pressed before exiting the application
    waitForKeyPressed();
    waitForKeyReleased();

    return;
}
