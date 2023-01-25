#include <stdint.h>

#define TITLE_BAR_HEIGHT 18

#define PALETTE_TOOLBAR 37
#define PALETTE_TOOLBAR_TEXT 38
#define PALETTE_BATTERY 43
#define PALETTE_BATTERY_IN_CHARGE 44
#define PALETTE_BATTERY_LOW 45

uint16_t fromRGB24(uint32_t color);
void draw_pixel(uint16_t x, uint8_t y, uint16_t color);
void draw_rectangle(uint16_t x, uint8_t y, uint16_t width, uint8_t height, uint16_t color);
void draw_line(uint16_t x0, uint8_t y0, uint16_t x1, uint8_t y1, uint16_t color);
void draw_circle(uint16_t originX, uint8_t originY, uint8_t radius, uint16_t color);
void draw_title_bar(const char * title);
