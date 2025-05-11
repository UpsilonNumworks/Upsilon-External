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
  char name_buffer[FILENAME_LENGTH_MAX];
  for (int i = 0; i < nb; i++) {
    strncpy(name_buffer, filenames[i], FILENAME_LENGTH_MAX);
    name_buffer[26] = '\0';
    extapp_drawTextLarge(name_buffer, 0, (i + 1) * 20, selected_file == i ? SELECTOR_COLOR_FG_SELECT : SELECTOR_COLOR_FG, SELECTOR_COLOR_BG, false);
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

  init_display();
  updateTitlebar("COMPRESSOR");
  // extapp_drawTextLarge("          Select a file         ", 0, 0, SELECTOR_COLOR_HEAD_FG, SELECTOR_COLOR_HEAD_BG, false);

  int nb = extapp_fileListWithExtension((const char **)filenames, max_files, extension_to_match, EXTAPP_RAM_FILE_SYSTEM);

  // If no files match, draw a no file found message
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
      // If the down arrow is pressed, select the next file
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
        // If OK or EXE is pressed, exit the loop
        break;
      } else if (scancode & SCANCODE_Back) {
        // If back key is pressed, exit the menu and return nothing
        return NULL;
      }
    }

    return filenames[selected_file];
  }
}
