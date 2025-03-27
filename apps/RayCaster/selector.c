#include "inc/selector.h"
#include "inc/peripherals.h"
#include <extapp_api.h>
#include <string.h>

#define FILENAME_LENGTH_MAX 512


/**
 * drawfileList: draw the file list
 * @param filenames char **, the file list
 * @param nb int, the number of files in the list
 * @param selected_file int, the selected file to highlight
 */
static void drawfileList(char ** filenames, int nb, int selected_file) {
  // Initialize the name buffer
  char name_buffer[FILENAME_LENGTH_MAX];
  // Iterate through the file list
  for (int i = 0; i < nb; i++) {
    // Copy the filename into the buffer
    strncpy(name_buffer, filenames[i], FILENAME_LENGTH_MAX);
    // Force the string to terminate with \0 because the filename can be longer than the length of the buffer
    name_buffer[26] = '\0';
    // Draw the filename, highlighted if selected
    extapp_drawTextLarge(name_buffer, 0, (i + 1) * 20, selected_file == i ? SELECTOR_COLOR_FG_SELECT : SELECTOR_COLOR_FG, SELECTOR_COLOR_BG, false);
    // TODO: I don't understand it
    strncpy(name_buffer, filenames[i], FILENAME_LENGTH_MAX);
  }
}

static char ** remove(char ** first, char ** last) {
  char ** result = first;
  while (first != last) {
    if (!(*first == NULL)) {
      *result = *first;
      ++result;
    }
    ++first;
  }
  return result;
}

/**
 * select_file: display a file list to let the user chose a file
 * @param extension_to_match const char *, use an empty string to match any file extension
 * @param max_files int, the maximum number of files to display
 * @return const char *, the name of the selected file, or NULL if no file is selected
 */
const char * select_file(const char * extension_to_match, int max_files) {
  // The file list
  char * filenames[max_files];
  // The selected file index in the order of the file list
  int selected_file = 0;

  // Wait for the last key to be released
  waitForKeyReleased();

  // Clear the display
  extapp_pushRectUniform(0, 0, LCD_WIDTH, LCD_HEIGHT, SELECTOR_COLOR_BG);
  // Draw title bar
  extapp_drawTextLarge("          Select a file         ", 0, 0, SELECTOR_COLOR_HEAD_FG, SELECTOR_COLOR_HEAD_BG, false);

  // Get the list of files from the flash memory
  int nb = extapp_fileListWithExtension((const char **)filenames, max_files, "", EXTAPP_FLASH_FILE_SYSTEM);

  // The extension of the file
  char * extension;

  // If the extension to match isn't empty, filter the file list
  // Checking without strcomp, because it's also a string littery when it is empty
  if (extension_to_match[0] != '\0') {
    // Iterate through the file list
    for (int i = 0; i < nb; i++) {
      // Get the file extension by splitting his name by a dot
      extension = strrchr(filenames[i], '.');

      // Get if the file's extension doesn't match the requested extension
      if (strcmp(extension, extension_to_match) != 0) {
        // If the extension doesn't match, delete the file from the list
        filenames[i] = NULL;
      }
    }
    // Update the number of files
    nb = remove(filenames, filenames + nb) - filenames;
  }

  // If no files match, draw a no file found message
  if (nb == 0) {
    // Draw the text
    extapp_drawTextLarge("          No file found          ", 0, 120, SELECTOR_COLOR_FG, SELECTOR_COLOR_BG, false);
    // Sleep for 10 milliseconds
    extapp_msleep(10);
    // Wait for a key to be pressed
    waitForKeyPressed();
    // Return nothing
    return NULL;
  } else {
    // Draw the file list
    drawfileList(filenames, nb, selected_file);
    // Infinite loop
    for (;;) {
      // Sleep for 10 milliseconds
      extapp_msleep(10);
      // Scan the keyboard
      uint64_t scancode = extapp_scanKeyboard();
      // If the down arrow is pressed, select the next file
      if (scancode & SCANCODE_Down) {
        // Select the next file, and go to the first file if the file is out of range
        selected_file = (selected_file + 1) % nb;
        // Redraw the file list
        drawfileList(filenames, nb, selected_file);
        // Wait for the key to be released, or 200 milliseconds if the key sill pressed
        waitForKeyReleasedTimeout(200);
      } else if (scancode & SCANCODE_Up) {
        // Select the previous file
        selected_file = (selected_file - 1) % nb;
        // If the file is out of range, select the last file
        if (selected_file < 0) {
          selected_file = nb + selected_file;
        }
        // Redraw the file list
        drawfileList(filenames, nb, selected_file);
        // Wait for the key to be released, or 200 milliseconds if the key sill pressed
        waitForKeyReleasedTimeout(200);
      } else if (scancode & (SCANCODE_OK | SCANCODE_EXE)) {
        // If OK or EXE is pressed, exit the loop
        break;
      } else if (scancode & SCANCODE_Back) {
        // If back key is pressed, exit the menu and return nothing
        return NULL;
      }
    }
    // Redraw the selected file
    return filenames[selected_file];
  }
}
