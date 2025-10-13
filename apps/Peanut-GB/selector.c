#include "selector.h"
#include <extapp_api.h>
#include <string.h>

#define FILENAME_LENGHT_MAX 512

char *osd_newextension(char *string, char *ext) {
  int l=strlen(string);
  while(l && string[l]!='.') {
    l--;
  }
  if (l) string[l]=0;
  strcat(string, ext);
  return string;
}

static void waitForKeyReleasedTimeout(int timeout) {
  while(extapp_scanKeyboard() && timeout > 0) {
    extapp_msleep(10);
    timeout -= 10;
  }
}

static void waitForKeyReleased() {
  while(extapp_scanKeyboard()) {
    extapp_msleep(10);
  }
}

static void waitForKeyPressed() {
  while(!extapp_scanKeyboard()) {
    extapp_msleep(10);
  }
}

static void drawRomList(char **filenames, int nb, int selected_rom) {
  char name_buffer[FILENAME_LENGHT_MAX];
  for(int i = 0; i < nb; i++) {
    strncpy(name_buffer, filenames[i], FILENAME_LENGHT_MAX);
    name_buffer[26] = '\0';
    extapp_drawTextLarge(name_buffer, 0, (i+1)*20, selected_rom == i ? SELECTOR_COLOR_FG_SELECT : SELECTOR_COLOR_FG, SELECTOR_COLOR_BG, false);
    strncpy(name_buffer, filenames[i], FILENAME_LENGHT_MAX);
    if(extapp_fileExists(osd_newextension(name_buffer, ".gbs"), EXTAPP_RAM_FILE_SYSTEM)) {
      extapp_drawTextLarge("Saved", LCD_WIDTH - 50, (i+1)*20, selected_rom == i ? SELECTOR_COLOR_FG_SELECT : SELECTOR_COLOR_FG, SELECTOR_COLOR_BG, false);
    } else {
      extapp_drawTextLarge("     ", LCD_WIDTH - 50, (i+1)*20, selected_rom == i ? SELECTOR_COLOR_FG_SELECT : SELECTOR_COLOR_FG, SELECTOR_COLOR_BG, false);
    }
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

  int nb = extapp_fileListWithExtension((const char **)filenames, max_roms, "", EXTAPP_FLASH_FILE_SYSTEM);

  size_t len;
  for(int i = 0; i < nb; i++) {
    const char * data = extapp_fileRead(filenames[i], &len, EXTAPP_FLASH_FILE_SYSTEM);
    // Calculate header checksum to validate that the rom is indeed a gameboy rom.
    // Calculation goes as follow:
    // x=0:FOR i=0134h TO 014Ch:x=x-MEM[i]-1:NEXT
    // (See: https://gbdev.io/pandocs/#_014d-header-checksum)
    if(len < 0x0150) {
      filenames[i] = NULL;
    } else {
      uint8_t checksum = 0;
      for(uint16_t i = 0x0134; i < 0x014D; i++) {
        checksum += ~data[i];
      }

      if (checksum != data[0x014D]) {
        filenames[i] = NULL;
      }
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
      } else if(scancode & SCANCODE_Backspace) {
        strncpy(name_buffer, filenames[selected_rom], FILENAME_LENGHT_MAX);
        extapp_fileErase(osd_newextension(name_buffer, ".gbs"), EXTAPP_RAM_FILE_SYSTEM);
        drawRomList(filenames, nb, selected_rom);
        waitForKeyReleased();
      } else if(scancode & SCANCODE_Back) {
        return NULL;
      }
    }
    extapp_pushRectUniform(0, 0, LCD_WIDTH, LCD_HEIGHT, SELECTOR_COLOR_BG);
    return filenames[selected_rom];
  }
}
