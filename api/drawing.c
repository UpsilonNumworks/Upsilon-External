#include <stdlib.h>

#include "drawing.h"
#include "extapp_api.h"

uint16_t fromRGB24(uint32_t color) {
  return ((color & 0xF80000) >> 8) | ((color & 0x00FC00) >> 5) | ((color & 0x0000F8) >> 3);
}

// For the Y coordinate an usigned byte is used because
// the LCD Height is 240 which is less than an unsigned
// byte maximum value (255)

void draw_pixel(uint16_t x, uint8_t y, uint16_t color) {
  extapp_pushRectUniform(x, y, 1, 1, color);
}

void draw_rectangle(uint16_t x, uint8_t y, uint16_t width, uint8_t height, uint16_t color) {
  extapp_pushRectUniform(x, y, width, height, color);
}

int8_t sign(int16_t arg) {
  if (arg < 0) return -1;
  else if (arg == 0) return 0;
  else return 1;
}

void draw_line(uint16_t x0, uint8_t y0, uint16_t x1, uint8_t y1, uint16_t color) {
  int16_t x = x0;
  int16_t y = y0;
  int16_t deltaX = abs(x1 - x0);
  int16_t deltaY = abs(y1 - y0);
  int8_t s0 = sign(x1 - x0);
  int8_t s1 = sign(y1 - y0);
  uint8_t interchange = 0;

  if (deltaY > deltaX) {
    int16_t t = deltaX;
    deltaX = deltaY;
    deltaY = t;
    interchange = 1;
  } else {
    interchange = 0;
  }

  int16_t e = 2*deltaY - deltaX;
  int16_t a = 2*deltaY;
  int16_t b = 2*deltaY - 2*deltaX;

  for (int16_t i = 0; i < deltaX; i++) {
    if (e < 0) {
      if (interchange == 1) y = y + s1;
      else x = x + s0;
      e = e + a;
    } else {
      y = y + s1;
      x = x + s0;
      e = e + b;
    }

    draw_pixel(x, y, color);
  }
}

void draw_circle(uint16_t originX, uint8_t originY, uint8_t radius, uint16_t color) {
  int16_t x = radius, y = 0;
  int16_t P = 1 - radius;

  while (x > y) {
    y++;

    if (P <= 0) {
        P = P + 2*y +1;
    } else {
        x--;
        P = P + 2*y - 2*x + 1;
    }

    if (x < y) break;

    draw_pixel(x + originX, y + originY, color);
    draw_pixel(-x + originX, y + originY, color);
    draw_pixel(x + originX, -y + originY, color);
    draw_pixel(-x + originX, -y + originY, color);

    if (x != y) {
        draw_pixel(y + originX, x + originY, color);
        draw_pixel(-y + originX, x + originY, color);
        draw_pixel(y + originX, -x + originY, color);
        draw_pixel(-y + originX, -x + originY, color);
    }
  }
}

void draw_title_bar(const char * title) {
  // Container at the top of the screen
  draw_rectangle(0, 0, LCD_WIDTH, TITLE_BAR_HEIGHT, extapp_fromPalette(PALETTE_TOOLBAR));

  // Centered title
  uint8_t title_width = extapp_drawTextSmall(title, 0, 0, 0, 0, true);
  extapp_drawTextSmall(title, (LCD_WIDTH / 2) - (title_width / 2), 3, extapp_fromPalette(PALETTE_TOOLBAR_TEXT), extapp_fromPalette(PALETTE_TOOLBAR), false);

  // Battery Icon (outline)
  uint16_t battery_origin[] = { LCD_WIDTH - 20 - 1, 5 - 1 }; // -1 On both because the screen origin is (0;0) and not (1;1)
  draw_rectangle(1 + battery_origin[0], battery_origin[1], 13, 1, extapp_fromPalette(PALETTE_BATTERY));
  draw_rectangle(1 + battery_origin[0], 8 + battery_origin[1], 13, 1, extapp_fromPalette(PALETTE_BATTERY));
  draw_rectangle(battery_origin[0], 1 + battery_origin[1], 1, 7, extapp_fromPalette(PALETTE_BATTERY));
  draw_rectangle(14 + battery_origin[0], 1 + battery_origin[1], 1, 7, extapp_fromPalette(PALETTE_BATTERY));
  draw_rectangle(15 + battery_origin[0], 3 + battery_origin[1], 1, 3, extapp_fromPalette(PALETTE_BATTERY));

  // State dependant drawing
  if (extapp_isBatteryCharging()) {
    // TODO (Finish): Charging...
    draw_rectangle(2 + battery_origin[0], 2 + battery_origin[1], 11, 5, extapp_fromPalette(PALETTE_BATTERY_IN_CHARGE));
  } else {
    uint8_t battery_level = extapp_getBatteryLevel();

    if (battery_level == BATTERY_FULL) {
      // Fully charged
      draw_rectangle(2 + battery_origin[0], 2 + battery_origin[1], 11, 5, extapp_fromPalette(PALETTE_BATTERY));
    } else if (battery_level == BATTERY_SOMEWHERE_INBETWEEN) {
      // Somewhere in between
      draw_rectangle(2 + battery_origin[0], 2 + battery_origin[1], 5, 5, extapp_fromPalette(PALETTE_BATTERY));
      draw_rectangle(2 + 5 + battery_origin[0], 2 + battery_origin[1], 6, 5, extapp_blendColor(
        extapp_fromPalette(PALETTE_BATTERY),
        extapp_fromPalette(PALETTE_TOOLBAR),
        128
      ));
    } else if (battery_level == BATTERY_LOW) {
      // TODO: Low battery
    } else {
      // Battery is empty (How am I alive??)
    }
  }
}