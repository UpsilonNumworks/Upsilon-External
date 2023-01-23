#include "keyboard.h"

struct KeyMapping mappings[] = {
    {'N', ((uint64_t)1 << 63), {0, 0, 0, 0, 0}},
    {'E', SCANCODE_EXE, {0, 0, 0, 0, 0}},
    {'a', SCANCODE_Exp, {1, 2, 0, 0, 0}},
    {'b', SCANCODE_Ln, {2, 1, 1, 1, 0}},
    {'c', SCANCODE_Log, {2, 1, 2, 1, 0}},
    {'d', SCANCODE_Imaginary, {2, 1, 1, 0, 0}},
    {'e', SCANCODE_Comma, {1, 0, 0, 0, 0}},
    {'f', SCANCODE_Power, {1, 1, 2, 1, 0}},
    {'g', SCANCODE_Sine, {2, 2, 1, 0, 0}},
    {'h', SCANCODE_Cosine, {1, 1, 1, 1, 0}},
    {'i', SCANCODE_Tangent, {1, 1, 0, 0, 0}},
    {'j', SCANCODE_Pi, {1, 2, 2, 2, 0}},
    {'k', SCANCODE_Sqrt, {2, 1, 2, 0, 0}},
    {'l', SCANCODE_Square, {1, 2, 1, 1, 0}},
    {'m', SCANCODE_Seven, {2, 2, 0, 0, 0}},
    {'n', SCANCODE_Eight, {2, 1, 0, 0, 0}},
    {'o', SCANCODE_Nine, {2, 2, 2, 0, 0}},
    {'p', SCANCODE_LeftParenthesis, {1, 2, 2, 1, 0}},
    {'q', SCANCODE_RightParenthesis, {2, 2, 1, 2, 0}},
    {'r', SCANCODE_Four, {1, 2, 1, 0, 0}},
    {'s', SCANCODE_Five, {1, 1, 1, 0, 0}},
    {'t', SCANCODE_Six, {2, 0, 0, 0, 0}},
    {'u', SCANCODE_Multiplication, {1, 1, 2, 0, 0}},
    {'v', SCANCODE_Division, {1, 1, 1, 2, 0}},
    {'w', SCANCODE_One, {1, 2, 2, 0, 0}},
    {'x', SCANCODE_Two, {2, 1, 1, 2, 0}},
    {'y', SCANCODE_Three, {2, 1, 2, 2, 0}},
    {'z', SCANCODE_Plus, {2, 2, 1, 1, 0}}};

struct KeyMapping get_map_from_scan(uint64_t scan)
{
  if (scan == 0)
    return mappings[0];

  for (int i = 1; i < 28; i++)
  {
    if ((scan & mappings[i].scan_code) > 0)
    {
      return mappings[i];
    }
  }

  return mappings[0];
}

void wait_for_key_release()
{
  while (extapp_scanKeyboard())
  {
  }
}