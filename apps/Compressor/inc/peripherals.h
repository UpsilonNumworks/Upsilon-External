#ifndef COMPRESSOR_PERIPHERALS_H_
#define COMPRESSOR_PERIPHERALS_H_

#include <stdint.h>


extern long long unsigned int keymappings[16];
extern int keymappings_await[16];

void init_display();
void waitForKeyPressed();
void waitForKeyReleased();
void waitForKeyReleasedTimeout(int timeout);
void updateTitlebar(char * title);

#endif
