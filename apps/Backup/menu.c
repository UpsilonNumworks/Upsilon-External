#include "menu.h"
#include "main.h"

#include <extapp_api.h>
#include <stdint.h>
#include <string.h>

// Different transmission presets
const struct TransmissionProfile profiles[] = {
  {2, 25, 0, 1273, "1. 2×2 pixels, ≈5s, fastest"},
  // {2, 25, 3, 535, "2×2 pixels, ≈11s, high error correction"},
  {3, 15, 0, 520, "2. 3×3 pixels, ≈12s"},
  // {3, 15, 3, 220, "3×3 pixels, ≈27s, high error correction"},
  {4, 10, 0, 271, "3. 4×4 pixels, ≈22s, slowest"},
};

void open_website() {
  // High pixel size to be sure people can scan it, even with a bad QR Code
  // scanner app
  draw_qr(WEBSITE_URL, (struct TransmissionProfile){7,4,0,sizeof(WEBSITE_URL), "Website"});
  // draw_qr(WEBSITE_URL, (struct TransmissionProfile){5,6,3,60, "Website"});
  // Wait for key to be released then pressed again
  while (extapp_scanKeyboard()) { extapp_msleep(10); }
  while (!extapp_scanKeyboard()) { extapp_msleep(10); }
}

void updateTitlebar(char * title) {
  extapp_pushRectUniform(0, 0, 320, 18, PRIMARY_COLOR);
  extapp_drawTextSmall(title, (320 / 2) - (strlen(title) * 7 / 2), 3, 0xFFFF, PRIMARY_COLOR, false);
}

void draw_menu(uint8_t selected) {
  updateTitlebar("BACKUP");
  extapp_pushRectUniform(0, 18, 320, 222, 0xFFFF);
  int i = 0;
  for (i = 0; i < sizeof(profiles)/sizeof(struct TransmissionProfile); i++) {
    extapp_drawTextLarge(profiles[i].profileName, 0, (i + 1) * 20, i == selected ? PRIMARY_COLOR : 0x0000, 0xFFFF, false);
  }
  extapp_drawTextLarge("4. Open website", 0, (i + 1) * 20, i == selected ? PRIMARY_COLOR : 0x0000, 0xFFFF, false);
}

struct TransmissionProfile select_profile() {
  draw_menu(0);

  // Dummy value returned to exit the app
  const struct TransmissionProfile exit = {
    0,0,0,0,0
  };

  const uint8_t option_count = sizeof(profiles)/sizeof(struct TransmissionProfile) + 1;
  uint8_t cursor = 0;
  uint64_t base_scancode = extapp_scanKeyboard();
  while (true) {
    uint64_t scancode = extapp_scanKeyboard();

    // Filter out previously pressed keys
    uint64_t filtered_scancode = scancode & (~base_scancode);

    if(filtered_scancode & (SCANCODE_Back | SCANCODE_Home | SCANCODE_OnOff)) {
        return exit;
    } else if (filtered_scancode & SCANCODE_Up) {
      cursor--;
      cursor = cursor >= option_count ? option_count - 1 : cursor;
      draw_menu(cursor);
      // Wait for release
      while (extapp_scanKeyboard() & SCANCODE_Up) { extapp_msleep(10); }
    } else if (filtered_scancode & SCANCODE_Down) {
      cursor++;
      cursor = cursor >= option_count ? 0 : cursor;
      draw_menu(cursor);
      // Wait for release
      while (extapp_scanKeyboard() & SCANCODE_Down) { extapp_msleep(10); }
    } else if (filtered_scancode & (SCANCODE_OK | SCANCODE_EXE)) {
      if (cursor < sizeof(profiles)/sizeof(struct TransmissionProfile)) {
        return profiles[cursor];
      } else if (cursor == sizeof(profiles)/sizeof(struct TransmissionProfile)) {
        open_website();
        // Reset base scancode as keys may have changed during website display
        base_scancode = extapp_scanKeyboard();
        scancode = base_scancode;
        draw_menu(cursor);
      }
    // TODO: More efficient way to check for numbers and avoid hardcoding
    } else if (filtered_scancode & SCANCODE_One) {
      return profiles[0];
    } else if (filtered_scancode & SCANCODE_Two) {
      return profiles[1];
    } else if (filtered_scancode & SCANCODE_Three) {
      return profiles[2];
    } else if (filtered_scancode & SCANCODE_Four) {
      open_website();
      base_scancode = extapp_scanKeyboard();
      scancode = base_scancode;
      draw_menu(cursor);
    }

    // Remove released pressed keys from base scancode
    base_scancode = base_scancode & scancode;

    // Sleep 10 ms to save battery
    extapp_msleep(10);
  }


  return exit;
}
