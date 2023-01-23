#include <stdint.h>

void draw_pixel(uint16_t x, uint16_t y, uint16_t color);
void draw_rectangle(uint16_t x, uint8_t y, uint16_t width, uint8_t height, uint16_t color);
void draw_line(uint16_t x0, uint8_t y0, uint16_t x1, uint8_t y1, uint16_t color);
void draw_circle(uint16_t originX, uint8_t originY, uint8_t radius, uint16_t color);