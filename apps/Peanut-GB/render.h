#ifndef RENDER_H
#define RENDER_H

#include "main.h"
#include "peanut_gb.h"

// void speed_handler(struct gb_s * gb, uint16_t MSpF);
void lcd_draw_line_centered(struct gb_s *gb, const uint8_t pixels[LCD_WIDTH], const uint_fast8_t line);
void lcd_draw_line_maximized_fast(struct gb_s * gb, const uint8_t * input_pixels, const uint_fast8_t line);
void lcd_draw_line_maximized_antialiased_horizontal(struct gb_s * gb, const uint8_t * input_pixels, const uint_fast8_t line);
void lcd_draw_line_maximized_antialiased_light(struct gb_s * gb, const uint8_t * input_pixels, const uint_fast8_t line);
void lcd_draw_line_maximized_antialiased_full(struct gb_s * gb, const uint8_t * input_pixels, const uint_fast8_t line);

#endif

