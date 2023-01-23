#include <stdlib.h>

#include "extapp_api.h"

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