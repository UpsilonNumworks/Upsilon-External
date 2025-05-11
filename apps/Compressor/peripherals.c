#include "inc/peripherals.h"
#include "extapp_api.h"
#include <string.h>


/**
 * init_display: clear the screen
 */
void init_display() {
  // Draw a rectangle to fill all the screen
  extapp_pushRectUniform(0, 0, 320, 240, 0xFFFF);
}

/**
 * waitForKeyPressed: wait for a key to be pressed
 */
void waitForKeyPressed() {
  // Scan the keyboard until we get a pressed key
  while (!extapp_scanKeyboard()) {
    // Sleep 10 milliseconds
    extapp_msleep(10);
  }
}

/**
 * waitForKeyReleased: wait for no keys pressed
 */
void waitForKeyReleased() {
  // Scan the keyboard until we get no keys pressed
  while (extapp_scanKeyboard()) {
    extapp_msleep(10);
  }
}

/**
 * waitForKeyReleasedTimeout: wait for no keys pressed, but with a timeout
 * @param timeout int
 */
void waitForKeyReleasedTimeout(int timeout) {
  // Scan the keyboard until we get no keys pressed, but exit if the timeout reached
  while (extapp_scanKeyboard() && timeout > 0) {
    // Sleep 10 milliseconds
    extapp_msleep(10);
    // Decrease the timeout of 10 milliseconds
    timeout -= 10;
  }
}


/**
  * updateTitlebar: draw a title bar with the given text
  * @param title char *, the text to show
 */
void updateTitlebar(char * title) {
    extapp_pushRectUniform(0, 0, LCD_WIDTH, 18, 0x7D19);
    extapp_drawTextSmall(title, LCD_WIDTH / 2 - (strlen(title) * 7 / 2), 3, 0xFFFF, 0x7D19, false);
}
