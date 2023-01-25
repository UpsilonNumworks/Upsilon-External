#include "extapp_api.h"
#include "drawing.h"

void extapp_main(void) {
  draw_title_bar("PLAYGROUND");

  draw_pixel(0, 0, 0);

  while (extapp_scanKeyboard()) {
    extapp_msleep(10);
  }
}