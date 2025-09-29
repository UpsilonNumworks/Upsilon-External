#include "inc/peripherals.h"
#include "extapp_api.h"



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
