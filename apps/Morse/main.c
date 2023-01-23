#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "extapp_api.h"
#include "keyboard.h"

void extapp_main(void)
{
  wait_for_key_release();

  extapp_pushRectUniform(0, 0, LCD_WIDTH, LCD_HEIGHT, 0x0);

  char input[256] = "";
  uint8_t input_cursor = 0;
  uint8_t morse[256 * 5] = {};
  uint16_t morse_cursor = 0;

  for (uint16_t i = 0; i < 256 * 5; i++)
  {
    morse[i] = 0;
  }

  while (true)
  {
    if (extapp_isKeydown(KEY_CHAR_1))
      break;

    uint64_t scan = extapp_scanKeyboard();
    struct KeyMapping key = get_map_from_scan(scan);
    if (key.key == 'E')
    {
      break;
    }
    else if (key.key != 'N')
    {
      input[input_cursor] = key.key;
      input_cursor++;

      for (uint8_t i = 0; i < 6; i++)
      {
        uint8_t next = i + 1;
        if (next > 5)
          next = 5;
        bool endOfLetter = ((key.morse[i] == 0) && (key.morse[next] == 0));
        if ((key.morse[i] != 0) || endOfLetter)
        {
          morse[morse_cursor] = key.morse[i];
          morse_cursor++;
          if (endOfLetter)
            break;
        }
      }
    }

    extapp_drawTextLarge(input, 0, 0, 0xffff, 0x0, false);
    wait_for_key_release();
  }

  for (uint16_t i = 0; i < 256 * 5; i++)
  {
    extapp_setLedColor(0);
    extapp_msleep(250);

    if ((morse[i] == 0) && (morse[i + 1] == 0))
      break;

    if ((morse[i] == 1) || (morse[i] == 2))
    {
      extapp_setLedColor(0x07E0);
      if (morse[i] == 1)
        extapp_msleep(250);
      else if (morse[i] == 2)
        extapp_msleep(750);
    }
    else
    {
      extapp_setLedColor(0x001F);
      extapp_msleep(250);
    }
  }
}