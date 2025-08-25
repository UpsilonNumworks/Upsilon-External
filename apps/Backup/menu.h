#ifndef MENU_H
#define MENU_H

#include <stdint.h>

void open_website();
void updateTitlebar(char * title);
void draw_menu(uint8_t selected);
struct TransmissionProfile select_profile();


#endif
