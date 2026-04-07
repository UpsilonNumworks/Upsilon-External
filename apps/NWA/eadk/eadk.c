#include "extapp_api.h"
#include <stdint.h>
#include <stdio.h>
#include <setjmp.h>

///////// EADK launching compat /////////
// void *heap_end;
void *end;
// uint32_t end;
const char* eadk_external_data;
size_t eadk_external_data_size;
static char * heap_end;

extern int main(void);

void extapp_main(void) {
  // end = _heap_base + _heap_size;
  end = _heap_base;
  heap_end = _heap_base;
  eadk_external_data_size = 0;
  eadk_external_data = extapp_fileRead("NWA.bin", &eadk_external_data_size, EXTAPP_FLASH_FILE_SYSTEM);
  char buffer[100];
  sprintf(buffer, "data = 0x%lx", eadk_external_data);
  extapp_drawTextSmall(buffer, 0, 0, 0x0000, 0xFFFF, false);
  extapp_msleep(1000);
  main();
}

// Some Newlib are referencing it
void _fini (void) {};

// Callback to handle stuff usually handled by Epsilon, like exit timer
EXTERNC jmp_buf oom_jmp_buf;
void callback() {
  if (extapp_scanKeyboard() & SCANCODE_Home) {
    longjmp(oom_jmp_buf, 3);
  }
  extapp_msleep(1000);
}

///////// EADK rebindings /////////

typedef struct {
  uint16_t x;
  uint16_t y;
} eadk_point_t;

typedef struct {
  uint16_t x;
  uint16_t y;
  uint16_t width;
  uint16_t height;
} eadk_rect_t;


uint64_t eadk_keyboard_scan() {
  callback();
  return extapp_scanKeyboard();
}

// TODO: Implement events
// eadk_event_t eadk_event_get(int32_t* timeout);
uint16_t eadk_event_get(int32_t* timeout) {
  callback();
  extapp_msleep(*timeout);
  return -1;
}

void eadk_backlight_set_brightness(uint8_t brightness) {
  callback();
}

uint8_t eadk_backlight_brightness() {
  callback();
  // TODO: Check the value
  return 240;
}

bool eadk_battery_is_charging() {
  callback();
  return false;
}

uint8_t eadk_battery_level() {
  // TODO: Check the value
  return 2;
}

float eadk_battery_voltage() {
  callback();
  // TODO: Check the values returned by Epsilon
  return 3.8;
}

void eadk_display_push_rect(eadk_rect_t rect, const uint16_t* color) {
  callback();
  extapp_pushRect(rect.x, rect.y, rect.width, rect.height, color);
}

void eadk_display_push_rect_uniform(eadk_rect_t rect, uint16_t color) {
  callback();
  extapp_pushRectUniform(rect.x, rect.y, rect.width, rect.height, color);
}

void eadk_display_pull_rect(eadk_rect_t rect, uint16_t * pixels) {
  callback();
  extapp_pullRect(rect.x, rect.y, rect.width, rect.height, pixels);
}

bool eadk_display_wait_for_vblank() {
  callback();
  return extapp_waitForVBlank();
}

void eadk_display_draw_string(const char* text, eadk_point_t point,
                              bool large_font, uint16_t text_color,
                              uint16_t background_color) {
  callback();
  if (large_font) {
    extapp_drawTextLarge(text, point.x, point.y, text_color, background_color, false);
  } else {
    extapp_drawTextSmall(text, point.x, point.y, text_color, background_color, false);
  }
}

void eadk_timing_msleep(uint32_t ms) {
  callback();
  extapp_msleep(ms);
}

void eadk_timing_usleep(uint32_t us) {
  callback();
  // We don't have usleep on extapp
  extapp_msleep(us / 1000);
}

uint64_t eadk_timing_millis() {
  callback();
  return extapp_millis();
}

bool eadk_usb_is_plugged() {
  callback();
  return false;
}

uint32_t eadk_random() {
  callback();
  // TODO: Use a random generator
  return extapp_millis();
}