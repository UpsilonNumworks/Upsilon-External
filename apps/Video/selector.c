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
  char * filenames[max_files];
  int selected_file = 0;

  waitForKeyReleased();

  extapp_pushRectUniform(0, 0, LCD_WIDTH, LCD_HEIGHT, SELECTOR_COLOR_BG);
  extapp_drawTextLarge("          Select a video         ", 0, 0, SELECTOR_COLOR_HEAD_FG, SELECTOR_COLOR_HEAD_BG, false);

  int nb = extapp_fileListWithExtension((const char **)filenames, max_files, extension_to_match, EXTAPP_FLASH_FILE_SYSTEM);

  char * extension;

  if (extension_to_match[0] != '\0') {
    for (int i = 0; i < nb; i++) {
      extension = strrchr(filenames[i], '.') + 1;

      if (strcmp(extension, extension_to_match) != 0) {
        filenames[i] = NULL;
      }
    }
    nb = remove(filenames, filenames + nb) - filenames;
  }

  if (nb == 0) {
    extapp_drawTextLarge("          No file found          ", 0, 120, SELECTOR_COLOR_FG, SELECTOR_COLOR_BG, false);
    extapp_msleep(10);
    waitForKeyPressed();
    return NULL;
  } else {
    drawfileList(filenames, nb, selected_file);
    for (;;) {
      extapp_msleep(10);
      uint64_t scancode = extapp_scanKeyboard();
      if (scancode & SCANCODE_Down) {
        selected_file = (selected_file + 1) % nb;
        drawfileList(filenames, nb, selected_file);
        waitForKeyReleasedTimeout(200);
      } else if (scancode & SCANCODE_Up) {
        selected_file = (selected_file - 1) % nb;
        if (selected_file < 0) {
          selected_file = nb + selected_file;
        }
        drawfileList(filenames, nb, selected_file);
        waitForKeyReleasedTimeout(200);
      } else if (scancode & (SCANCODE_OK | SCANCODE_EXE)) {
        break;
      } else if (scancode & SCANCODE_Back) {
        return NULL;
      }
    }
    return filenames[selected_file];
  }
}
