#define PEANUT_GB_HEADER_ONLY
#include <extapp_api.h>

#include "main.h"
#include "peanut_gb.h"

// Line buffer
static uint16_t line_buffer[267];

inline uint16_t color_from_gb_pixel(uint8_t gb_pixel) {
  uint8_t gb_color = gb_pixel;
  // uint8_t gb_color = gb_pixel & 0x3;
  return palette[gb_color];
}

inline uint16_t blend_gb_pixels(uint8_t gb_pixel_1, uint8_t gb_pixel_2) {
  // return color_from_gb_pixel(gb_pixel_1);
  // uint8_t gb_color_1 = gb_pixel_1 & 0x3;
  // uint8_t gb_color_2 = gb_pixel_2 & 0x3;
  uint8_t gb_color_1 = gb_pixel_1;
  uint8_t gb_color_2 = gb_pixel_2;
  // We can average as we only use the lower-weight bits
  // This will produce a blurry image, but in the end (It doesn't even matter,
  // Linkin Park), it will result in a better visual scaling
  uint8_t gb_color_blend = (gb_color_1 + gb_color_2) / 2;
  // uint8_t gb_color_blend = gb_color_1 > gb_color_2 ? gb_color_1 : gb_color_2;
  // uint8_t gb_color_blend = gb_color_1 > gb_color_2 ? gb_color_2 : gb_color_1;
  return palette[gb_color_blend];
}

void lcd_draw_line_centered(struct gb_s *gb, const uint8_t pixels[LCD_WIDTH], const uint_fast8_t line) {
  struct priv_t *priv = gb->direct.priv;

  #pragma unroll 40
  for(unsigned int x = 0; x < LCD_WIDTH; x++) {
    line_buffer[x] = color_from_gb_pixel(pixels[x]);
  }

  extapp_pushRect((NW_LCD_WIDTH - LCD_WIDTH) / 2, (NW_LCD_HEIGHT - LCD_HEIGHT) / 2 + line, LCD_WIDTH, 1, line_buffer);
}


void lcd_draw_line_maximized_fast(struct gb_s * gb, const uint8_t * input_pixels, const uint_fast8_t line) {
  // Nearest neighbor scaling of a 160x144 texture to a 266x240 resolution (to keep the ratio)
  // Horizontally, we multiply by 1.66 (160*1.66 = 266)
  struct priv_t *priv = gb->direct.priv;

  #pragma unroll 40
  for (int i=0; i<LCD_WIDTH; i++) {
    uint16_t color = color_from_gb_pixel(input_pixels[i]);
    // We can't use floats for performance reason, so we use a fixed point
    // representation
    line_buffer[166*i/100] = color;
    // This line is useless 1/3 times, but using an if is slower
    line_buffer[166*i/100+1] = color;
  }

  uint16_t y = (5*line)/3;
  uint8_t line_mod = line % 3;
  if (line_mod == 0) {
    extapp_pushRect((NW_LCD_WIDTH - 265) / 2, y, 265, 1, line_buffer);
    extapp_pushRect((NW_LCD_WIDTH - 265) / 2, y+1, 265, 1, line_buffer);
  } else if (line_mod == 1) {
    extapp_pushRect((NW_LCD_WIDTH - 265) / 2, y + 1, 265, 1, line_buffer);
    extapp_pushRect((NW_LCD_WIDTH - 265) / 2, y + 2, 265, 1, line_buffer);
  } else {
    extapp_pushRect((NW_LCD_WIDTH - 265) / 2, y + 1, 265, 1, line_buffer);
  }
}

void lcd_draw_line_maximized_antialiased_horizontal(struct gb_s * gb, const uint8_t * input_pixels, const uint_fast8_t line) {
  // Nearest neighbor scaling of a 160x144 texture to a 266x240 resolution (to keep the ratio)
  // Horizontally, we multiply by 1.66 (160*1.66 = 266)
  struct priv_t *priv = gb->direct.priv;

  #pragma unroll 40
  for (int i=0; i<LCD_WIDTH; i++) {
    uint16_t color = color_from_gb_pixel(input_pixels[i]);
    uint16_t color_blend = blend_gb_pixels(input_pixels[i], input_pixels[i + 1]);
    // We can't use floats for performance reason, so we use a fixed point
    // representation
    line_buffer[166*i/100] = color;
    // This line is useless 1/3 times, but using an if is slower
    line_buffer[166*i/100+1] = color_blend;
  }
  line_buffer[264] = 0x0000;

  uint16_t y = (5*line)/3;
  uint8_t line_mod = line % 3;
  if (line_mod == 0) {
    extapp_pushRect((NW_LCD_WIDTH - 265) / 2, y, 265, 1, line_buffer);
    extapp_pushRect((NW_LCD_WIDTH - 265) / 2, y+1, 265, 1, line_buffer);
  } else if (line_mod == 1) {
    extapp_pushRect((NW_LCD_WIDTH - 265) / 2, y + 1, 265, 1, line_buffer);
    extapp_pushRect((NW_LCD_WIDTH - 265) / 2, y + 2, 265, 1, line_buffer);
  } else {
    extapp_pushRect((NW_LCD_WIDTH - 265) / 2, y + 1, 265, 1, line_buffer);
  }
}

void lcd_draw_line_maximized_antialiased_light(struct gb_s * gb, const uint8_t * input_pixels, const uint_fast8_t line) {
  // We assume the display is rendered from the top to the bottom, without
  // skipping lines or interlacing frames, otherwise antialiasing won't work
  // Old scaling worked as below:
  // 1st line: 1 lines
  // 2nd line: 2 lines
  // 3rd line: 2 line
  // (and repeat)

  // New scaling work using subpixels, using the fact that pixels are ordered this way
  // This is one pixel :
  // RRRRRR
  // GGGGGG
  // BBBBBB
  // This is low-level, but allow to virtually increase the resolution of the
  // screen.
  // Here is the new scaling ordering:
  // 1st line: 2 lines
  // 2nd line: 1.33 line (red is quite different from green and blue and would be seen as 1.5 line)
  // 3rd line: 1.66 line (green and blue are similar, so in fact it would be seen as 1.5 line)
  struct priv_t *priv = gb->direct.priv;

  uint8_t line_mod = line % 3;

  if (line_mod == 0) {
    // First line case
    #pragma unroll 40
    for (int i = 0; i < LCD_WIDTH; i++) {
      uint32_t color = color_from_gb_pixel(input_pixels[i]);
      uint32_t color_blend = blend_gb_pixels(input_pixels[i], input_pixels[i + 1]);

      // line_buffer[166 * i / 100] = ((last_color & 0xF800) + (color & 0xF800)) / 2 | ((last_color & 0x07E0) + (color & 0x07E0)) / 2 | ((last_color & 0x001F) + (color & 0x001F)) / 2;
      line_buffer[166 * i / 100] = color;
      line_buffer[166 * i / 100 + 1] = color_blend;
    }
    // We blank the last element due to broken blend (out-of-bound when blending
    // pixels) and we don't want to bother using an if in the loop
    line_buffer[264] = 0x0000;

    // int(0 * 5 / 3) = 0
    // int(3 * 5 / 3) = 5
    uint16_t y = (5 * line) / 3;
    extapp_pushRect((NW_LCD_WIDTH - 265) / 2, y, 265, 1, line_buffer);
    extapp_pushRect((NW_LCD_WIDTH - 265) / 2, y + 1, 265, 1, line_buffer);
  } else if (line_mod == 1) {
    // Second line case
    #pragma unroll 40
    for (int i = 0; i < LCD_WIDTH; i++) {
      uint32_t color = color_from_gb_pixel(input_pixels[i]);
      uint32_t color_blend = blend_gb_pixels(input_pixels[i], input_pixels[i + 1]);

      line_buffer[166 * i / 100] = color;
      line_buffer[166 * i / 100 + 1] = color_blend;
    }
    line_buffer[264] = 0x0000;

    // int(1 * 5 / 3 + 1) = 2, the + 1 is used to compensate the previous double line
    uint16_t y = (5 * line) / 3 + 1;
    extapp_pushRect((NW_LCD_WIDTH - 265) / 2, y, 265, 1, line_buffer);
    // Second pushRect need to be done from the last line to know the other subpixel part
  } else {
    // Third line case
    #pragma unroll 40
    for (int i = 0; i < LCD_WIDTH; i++) {
      // TODO: Blend pixels directly from the palette, preprocessed, to avoid
      // the need to compute it for each pixel.
      uint32_t color = color_from_gb_pixel(input_pixels[i]);
      // New, blend the previous line with the current one for antialiasing
      // Old color with only red channel | (using the or operator to combine) green and blue from new color
      color = (line_buffer[166 * i / 100] & 0xF800) | (color & 0x07FF);

      // If you want to split with red/green for first line and blue for the next one, use the following:
      // color = (line_buffer[166 * i / 100] & 0xFFE0) | (color & 0x001F);

      line_buffer[166 * i / 100] = color;

      // Don't overwrite next pixel, otherwise we will corrupt it
      if ((166 * i / 100) + 1 != (166 * (i + 1) / 100)) {
        uint32_t color_blend = blend_gb_pixels(input_pixels[i], input_pixels[i + 1]);
        // uint32_t color_blend = color;
        color_blend = (line_buffer[166 * i / 100 + 1] & 0xF800) | (color_blend & 0x07FF);
        line_buffer[166 * i / 100 + 1] = color_blend;
      }
    }
    line_buffer[264] = 0x0000;

    // int(2 * 5 / 3) = 3, we send the line from the line before, so no need to + 1
    uint16_t y = (5 * line) / 3;
    extapp_pushRect((NW_LCD_WIDTH - 265) / 2, y, 265, 1, line_buffer);

    #pragma unroll 40
    for (int i = 0; i < LCD_WIDTH; i++) {
      uint32_t color = color_from_gb_pixel(input_pixels[i]);
      uint32_t color_blend = blend_gb_pixels(input_pixels[i], input_pixels[i + 1]);
      line_buffer[166 * i / 100] = color;
      line_buffer[166 * i / 100 + 1] = color_blend;
    }
    line_buffer[264] = 0x0000;

    extapp_pushRect((NW_LCD_WIDTH - 265) / 2, y + 1, 265, 1, line_buffer);
  }
}


// Full antialiasing is not used, just present for reference
// When I implemented it, I tried to compare it with the light scaling, and
// didn't see any difference, even when looking closely. Either it's not working
// (which is probably not the case as when interlacing is enabled, it's even
// more broken than light antialiasing), or the benefits are very lights.
// When using it, I've around 0.6-0.7 ms/f of overhead compared to light
// antialising, so it's not worth the tradeoff.
void lcd_draw_line_maximized_antialiased_full(struct gb_s * gb, const uint8_t * input_pixels, const uint_fast8_t line) {
  // We assume the display is rendered from the top to the bottom, without
  // skipping lines or interlacing frames, otherwise antialiasing won't work
  // Old scaling is working as below:
  // 1st line: 1 lines
  // 2nd line: 2 lines
  // 3rd line: 2 line
  // (and repeat)

  // New scaling work using subpixels, using the fact that pixels are ordered this way
  // This is one pixel :
  // RRRRRR
  // GGGGGG
  // BBBBBB
  // This is low-level, but allow to virtually increase the resolution of the
  // screen.
  // Here is the new scaling ordering:
  // 1st line: 1.66 line
  // 2nd line: 1.66 line
  // 3rd line: 1.66 line
  struct priv_t *priv = gb->direct.priv;

  uint8_t line_mod = line % 3;

  if (line_mod == 0) {
    #pragma unroll 40
    for (int i = 0; i < LCD_WIDTH; i++) {
      uint32_t color = color_from_gb_pixel(input_pixels[i]);
      uint32_t color_blend = blend_gb_pixels(input_pixels[i], input_pixels[i + 1]);

      line_buffer[166 * i / 100] = color;
      line_buffer[166 * i / 100 + 1] = color_blend;
    }
    // We blank the last element due to broken blend (out-of-bound when blending
    // pixels) and we don't want to bother using an if in the loop
    line_buffer[264] = 0x0000;

    // int(0 * 5 / 3) = 0
    uint16_t y = (5 * line) / 3;
    extapp_pushRect((NW_LCD_WIDTH - 265) / 2, y, 265, 1, line_buffer);
  } else if (line_mod == 1) {

    #pragma unroll 40
    for (int i = 0; i < LCD_WIDTH; i++) {
      uint32_t color = color_from_gb_pixel(input_pixels[i]);
      color = (line_buffer[166 * i / 100] & 0xFFE0) | (color & 0x001F);
      line_buffer[166 * i / 100] = color;
      if ((166 * i / 100) + 1 != (166 * (i + 1) / 100)) {
        uint32_t color_blend = blend_gb_pixels(input_pixels[i], input_pixels[i + 1]);
        color_blend = (line_buffer[166 * i / 100 + 1] & 0xFFE0) | (color_blend & 0x001F);
        line_buffer[166 * i / 100 + 1] = color_blend;
      }
    }
    line_buffer[264] = 0x0000;

    // int(1 * 5 / 3) = 1
    uint16_t y = (5 * line) / 3;
    extapp_pushRect((NW_LCD_WIDTH - 265) / 2, y, 265, 1, line_buffer);

    #pragma unroll 40
    for (int i = 0; i < LCD_WIDTH; i++) {
      uint32_t color = color_from_gb_pixel(input_pixels[i]);
      uint32_t color_blend = blend_gb_pixels(input_pixels[i], input_pixels[i + 1]);

      line_buffer[166 * i / 100] = color;
      line_buffer[166 * i / 100 + 1] = color_blend;
    }
    line_buffer[264] = 0x0000;

    extapp_pushRect((NW_LCD_WIDTH - 265) / 2, y + 1, 265, 1, line_buffer);
  } else {
    #pragma unroll 40
    for (int i = 0; i < LCD_WIDTH; i++) {
      uint32_t color = color_from_gb_pixel(input_pixels[i]);
      color = (line_buffer[166 * i / 100] & 0xF800) | (color & 0x07FF);
      line_buffer[166 * i / 100] = color;

      if ((166 * i / 100) + 1 != (166 * (i + 1) / 100)) {
        uint32_t color_blend = blend_gb_pixels(input_pixels[i], input_pixels[i + 1]);
        color_blend = (line_buffer[166 * i / 100 + 1] & 0xF800) | (color_blend & 0x07FF);
        line_buffer[166 * i / 100 + 1] = color_blend;
      }
    }
    line_buffer[264] = 0x0000;

    uint16_t y = (5 * line) / 3;
    // int(2 * 5 / 3) = 3
    extapp_pushRect((NW_LCD_WIDTH - 265) / 2, y, 265, 1, line_buffer);

    #pragma unroll 40
    for (int i = 0; i < LCD_WIDTH; i++) {
      uint32_t color = color_from_gb_pixel(input_pixels[i]);
      uint32_t color_blend = blend_gb_pixels(input_pixels[i], input_pixels[i + 1]);

      line_buffer[166 * i / 100] = color;
      line_buffer[166 * i / 100 + 1] = color_blend;
    }
    line_buffer[264] = 0x0000;

    extapp_pushRect((NW_LCD_WIDTH - 265) / 2, y + 1, 265, 1, line_buffer);

  }
}
