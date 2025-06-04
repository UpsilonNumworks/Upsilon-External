#include "inc/selector.h"
#include "inc/peripherals.h"
#include <extapp_api.h>
#include <string.h>

#define FILENAME_LENGHT_MAX 512

static void drawRomList(char **filenames, int nb, int selected_rom) {
  char name_buffer[FILENAME_LENGHT_MAX];
  for(int i = 0; i < nb; i++) {
    strncpy(name_buffer, filenames[i], FILENAME_LENGHT_MAX);
    name_buffer[26] = '\0';
    extapp_drawTextLarge(name_buffer, 0, (i+1)*20, selected_rom == i ? SELECTOR_COLOR_FG_SELECT : SELECTOR_COLOR_FG, SELECTOR_COLOR_BG, false);
    strncpy(name_buffer, filenames[i], FILENAME_LENGHT_MAX);
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

const char * select_rom() {
  const int max_roms = 50;
  char * filenames[max_roms];
  int selected_rom = 0;
  char name_buffer[FILENAME_LENGHT_MAX];

  waitForKeyReleased();

  extapp_pushRectUniform(0, 0, LCD_WIDTH, LCD_HEIGHT, SELECTOR_COLOR_BG);
  extapp_drawTextLarge("          Select a ROM          ", 0, 0, SELECTOR_COLOR_HEAD_FG, SELECTOR_COLOR_HEAD_BG, false);

  int nb = extapp_fileListWithExtension(filenames, max_roms, "", EXTAPP_FLASH_FILE_SYSTEM);

  size_t len;
  char * extension;
  for(int i = 0; i < nb; i++) {
    extension = strrchr(filenames[i], '.');

    if (strcmp(extension, ".ch8") != 0) {
      filenames[i] = NULL;
    }
  }
  nb = remove(filenames, filenames + nb) - filenames;

  if(nb == 0) {
    extapp_drawTextLarge("          No ROM found          ", 0, 120, SELECTOR_COLOR_FG, SELECTOR_COLOR_BG, false);
    extapp_msleep(10);
    waitForKeyPressed();
    return NULL;
  } else {
    drawRomList(filenames, nb, selected_rom);
    for(;;) {
      extapp_msleep(10);
      uint64_t scancode = extapp_scanKeyboard();
      if(scancode & SCANCODE_Down) {
        selected_rom = (selected_rom + 1) % nb;
        drawRomList(filenames, nb, selected_rom);
        waitForKeyReleasedTimeout(200);
      } else if(scancode & SCANCODE_Up) {
        selected_rom = (selected_rom - 1) % nb;
        if(selected_rom < 0) {
          selected_rom = nb + selected_rom;
        }
        drawRomList(filenames, nb, selected_rom);
        waitForKeyReleasedTimeout(200);
      } else if(scancode & (SCANCODE_OK | SCANCODE_EXE))  {
        break;
      } else if(scancode & SCANCODE_Back) {
        return NULL;
      }
    }
    extapp_pushRectUniform(0, 0, LCD_WIDTH, LCD_HEIGHT, SELECTOR_COLOR_BG);
    return filenames[selected_rom];
  }
}
