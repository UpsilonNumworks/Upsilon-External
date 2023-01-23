#include "extapp_api.h"
#include <string.h>

struct KeyMapping
{
  char key;
  uint64_t scan_code;
  uint8_t morse[5];
};

struct KeyMapping get_map_from_scan(uint64_t scan);
void wait_for_key_release();