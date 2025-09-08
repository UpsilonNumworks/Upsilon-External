#include <extapp_api.h>

// #include <stdint.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "main.h"
#include "lz4.h"
#include "render.h"
#include "selector.h"
#include "speed.h"

#include "peanut_gb.h"

#ifndef DUMMY_ROM
#define DUMMY_ROM 0
#endif

#if DUMMY_ROM
#define _DUMMY_ROM_VAR(NAME)   NAME ## _GB
#define DUMMY_ROM_VAR(NAME)    _DUMMY_ROM_VAR(NAME)
#define _DUMMY_ROM_FILE(NAME)   #NAME ".gb"
#define DUMMY_ROM_FILE(NAME)    _DUMMY_ROM_FILE(NAME)

#include "out.h"
#endif

static bool running = false;


// Returns a byte from the ROM file at the given address.
uint8_t gb_rom_read(struct gb_s *gb, const uint_fast32_t addr) {
  const struct priv_t * const p = gb->direct.priv;
  return p->rom[addr];
}

// Returns a byte from the cartridge RAM at the given address.
uint8_t gb_cart_ram_read(struct gb_s *gb, const uint_fast32_t addr) {
  const struct priv_t * const p = gb->direct.priv;
  return p->cart_ram[addr];
}

// Writes a given byte to the cartridge RAM at the given address.
void gb_cart_ram_write(struct gb_s *gb, const uint_fast32_t addr, const uint8_t val) {
  const struct priv_t * const p = gb->direct.priv;
  p->cart_ram[addr] = val;
}

// Ignore all errors.
void gb_error(struct gb_s *gb, const enum gb_error_e gb_err, const uint16_t val) {

  const char* gb_err_str[4] = {
    "UNKNOWN",
    "INVALID OPCODE",
    "INVALID READ",
    "INVALID WRITE"
  };

  switch (gb_err) {
    case GB_INVALID_WRITE:
    case GB_INVALID_READ:
      return;
    default:
      running = false;
  }

  // TODO: Handle errors.
}
const uint16_t palette_peanut_GB[4] = {0x9DE1, 0x8D61, 0x3306, 0x09C1};
const uint16_t palette_original[4] = {0x8F80, 0x24CC, 0x4402, 0x0A40};
const uint16_t palette_gray[4] = {0xFFFF, 0xAD55, 0x52AA, 0x0000};
const uint16_t palette_gray_negative[4] = {0x0000, 0x52AA, 0xAD55, 0xFFFF};
const uint16_t * palette = palette_peanut_GB;

enum save_status_e {
  SAVE_READ_OK,
  SAVE_WRITE_OK,
  SAVE_READ_ERR,
  SAVE_WRITE_ERR,
  SAVE_COMPRESS_ERR,
  SAVE_NODISP
};
static enum save_status_e saveMessage = SAVE_NODISP;

char* read_save_file(const char* name, size_t size) {
  char* save_name = malloc(strlen(name) + 3);
  strcpy(save_name, name);
  osd_newextension(save_name, ".gbs");

  char* output = malloc(size);

  if (extapp_fileExists(save_name, EXTAPP_RAM_FILE_SYSTEM)) {
    size_t file_len = 0;
    const char* save_content = extapp_fileRead(save_name, &file_len, EXTAPP_RAM_FILE_SYSTEM);
    int error = LZ4_decompress_safe(save_content, output, file_len, size);

    // Handling corrupted save.
    if (error <= 0) {
      memset(output, 0xFF, size);
      extapp_fileErase(save_name, EXTAPP_RAM_FILE_SYSTEM);
      saveMessage = SAVE_READ_ERR;
    } else {
      saveMessage = SAVE_READ_OK;
    }
  } else {
    memset(output, 0xFF, size);
  }

  free(save_name);

  return output;
}

void write_save_file(const char* name, char* data, size_t size) {
  char* save_name = malloc(strlen(name) + 3);
  strcpy(save_name, name);
  osd_newextension(save_name, ".gbs");

  char* output = malloc((size_t) MAX_SCRIPTSTORE_SIZE);

  int compressed_size = LZ4_compress_default(data, output, size, MAX_SCRIPTSTORE_SIZE);

  if (compressed_size > 0) {
    if (extapp_fileWrite(save_name, output, compressed_size, EXTAPP_RAM_FILE_SYSTEM)) {
      saveMessage = SAVE_WRITE_OK;
    } else {
      saveMessage = SAVE_WRITE_ERR;
    }
  } else {
    saveMessage = SAVE_COMPRESS_ERR;
  }

  free(save_name);
  free(output);
}

static bool wasSavePressed = false;
static bool wasMSpFPressed = false;
static uint8_t saveCooldown = 0;

// Static allow allocating to the ITCM, where 16KB are available
static struct gb_s gb;

void extapp_main() {

  enum gb_init_error_e gb_ret;
  struct priv_t priv = {
    .rom = NULL,
    .cart_ram = NULL,
    .MSpFfCounter = false,
    #if ENABLE_FRAME_LIMITER
    .timeBudget = 0,
    .currentFrame = 0,
    #endif
  };
  enum gb_init_error_e ret;

  #if DUMMY_ROM
  priv.rom = DUMMY_ROM_VAR(DUMMY_ROM_NAME);
  const char * file_name = DUMMY_ROM_FILE(DUMMY_ROM_NAME);
  #else
  const char * file_name = select_rom();
  if (!file_name)
    return;

  size_t file_len = 0;
  priv.rom = (const uint8_t*) extapp_fileRead(file_name, &file_len, EXTAPP_FLASH_FILE_SYSTEM);
  #endif

  // Alloc internal RAM.
  gb.wram = malloc(WRAM_SIZE);
  // Other buffers are allocated staticly in the gb object declaration
  // gb.vram = malloc(VRAM_SIZE);
  // gb.hram_io = malloc(HRAM_IO_SIZE);
  // gb.oam = malloc(OAM_SIZE);

  gb_ret = gb_init(&gb, &gb_rom_read, &gb_cart_ram_read, &gb_cart_ram_write, &gb_error, &priv);

  // TODO: Handle init errors.
  switch(gb_ret) {
    case GB_INIT_NO_ERROR:
      break;
    default:
      return;
  }

  // Alloc and init save RAM.
  size_t save_size = gb_get_save_size(&gb);
  priv.cart_ram = read_save_file(file_name, save_size);
  saveCooldown = SAVE_COOLDOWN;

  // Init LCD
  gb_init_lcd(&gb, &lcd_draw_line_maximized_fast);

  extapp_pushRectUniform(0, 0, NW_LCD_WIDTH, NW_LCD_HEIGHT, 0);

  running = true;
  while(running) {
    uint64_t start = extapp_millis();
    uint64_t kb = extapp_scanKeyboard();

    gb.direct.joypad_bits.a = (kb & SCANCODE_Back) ? 0 : 1;
    gb.direct.joypad_bits.b = (kb & SCANCODE_OK) ? 0 : 1;
    gb.direct.joypad_bits.select = (kb & ((uint64_t)1 << 8)) ? 0 : 1;
    gb.direct.joypad_bits.start = (kb & SCANCODE_Home) ? 0 : 1;
    gb.direct.joypad_bits.up = (kb & SCANCODE_Up) ? 0 : 1;
    gb.direct.joypad_bits.right = (kb & SCANCODE_Right) ? 0 : 1;
    gb.direct.joypad_bits.left = (kb & SCANCODE_Left) ? 0 : 1;
    gb.direct.joypad_bits.down = (kb & SCANCODE_Down) ? 0 : 1;

    if (kb & SCANCODE_Backspace)
      gb_reset(&gb);
    if (kb & SCANCODE_Toolbox) {
      if (!wasSavePressed && saveCooldown == 0) {
        write_save_file(file_name, priv.cart_ram, save_size);
        saveCooldown = SAVE_COOLDOWN;
        wasSavePressed = true;
      }
    } else if (wasSavePressed) {
      wasSavePressed = false;
    }

    if (kb & SCANCODE_Alpha) {
      if (!wasMSpFPressed) {
        priv.MSpFfCounter = !priv.MSpFfCounter;
        wasMSpFPressed = true;
        extapp_pushRectUniform(0, NW_LCD_HEIGHT / 2 + LCD_HEIGHT / 2, NW_LCD_WIDTH, NW_LCD_HEIGHT - (NW_LCD_HEIGHT / 2 + LCD_HEIGHT / 2), 0);
      }
    } else if (wasMSpFPressed) {
      wasMSpFPressed = false;
    }

    if (kb & SCANCODE_Zero) {
      running = false;
      break;
    }

    if (kb & SCANCODE_Plus) {
      gb.display.lcd_draw_line = lcd_draw_line_maximized_fast;
    }
    if (kb & SCANCODE_Multiplication) {
      gb.display.lcd_draw_line = lcd_draw_line_maximized_antialiased_light;
    }
    if (kb & SCANCODE_LeftParenthesis) {
      gb.display.lcd_draw_line = lcd_draw_line_maximized_antialiased_horizontal;
    }
    if (kb & SCANCODE_Sqrt) {
      gb.display.lcd_draw_line = lcd_draw_line_maximized_antialiased_full;
    }
    if (kb & SCANCODE_Minus) {
      gb.display.lcd_draw_line = lcd_draw_line_centered;
      extapp_pushRectUniform(0, 0, NW_LCD_WIDTH, NW_LCD_HEIGHT, 0);
    }
    if (kb & SCANCODE_Division) {
      gb.display.lcd_draw_line = NULL;
      extapp_pushRectUniform(0, 0, NW_LCD_WIDTH, NW_LCD_HEIGHT, 0);
    }

    if (kb & SCANCODE_One) {
      palette = palette_peanut_GB;
    }
    if (kb & SCANCODE_Two) {
      palette = palette_original;
    }
    if (kb & SCANCODE_Three) {
      palette = palette_gray;
    }
    if (kb & SCANCODE_Four) {
      palette = palette_gray_negative;
    }

    gb_run_frame(&gb);

    if (saveCooldown > 1) {
      saveCooldown--;
      switch(saveMessage) {
        case SAVE_READ_OK:
          extapp_drawTextSmall("Loaded save!", 10, NW_LCD_HEIGHT - 30, 65535, 0, false);
          break;
        case SAVE_READ_ERR:
          extapp_drawTextSmall("Error while loading save!", 10, NW_LCD_HEIGHT - 30, 65535, 0, false);
          break;
        case SAVE_WRITE_OK:
          extapp_drawTextSmall("Saved!", 10, NW_LCD_HEIGHT - 30, 65535, 0, false);
          break;
        case SAVE_WRITE_ERR:
          extapp_drawTextSmall("Error while writing save!", 10, NW_LCD_HEIGHT - 30, 65535, 0, false);
          break;
        case SAVE_COMPRESS_ERR:
          extapp_drawTextSmall("Error while compressing save!", 10, NW_LCD_HEIGHT - 30, 65535, 0, false);
          break;
        default:
          break;
      }
    } else if (saveCooldown == 1) {
      saveCooldown--;
      extapp_pushRectUniform(0, NW_LCD_HEIGHT / 2 + LCD_HEIGHT / 2, NW_LCD_WIDTH, NW_LCD_HEIGHT - (NW_LCD_HEIGHT / 2 + LCD_HEIGHT / 2), 0);
    }
    uint64_t end = extapp_millis();
    speed_handler(&gb, end - start);
  }

  free(gb.wram);

  // Others buffers are allocated staticly
  // free(gb.vram);
  // free(gb.hram_io);
  // free(gb.oam);

  write_save_file(file_name, priv.cart_ram, save_size);
  free(priv.cart_ram);
}
