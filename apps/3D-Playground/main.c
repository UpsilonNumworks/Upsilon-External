#include <stdbool.h>
#include <stdio.h>

#include "extapp_api.h"
#include "drawing.h"
#include "renderer3d.h"

#define PI 3.141592654

void extapp_main(void) {
  uint8_t FPS = 20;

  struct Point3D points[] = {
    {-1, -1, -1},
    {-1, -1, 1},
    {1, -1, -1},
    {-1, 1, -1},
    {-1, 1, 1},
    {1, -1, 1},
    {1, 1, -1},
    {1, 1, 1},
  };

  struct Vertex vertices[] = {
    {points[0], points[1]},
    {points[0], points[2]},
    {points[0], points[3]},
    {points[2], points[5]},
    {points[3], points[6]},
    {points[3], points[4]},
    {points[4], points[7]},
    {points[6], points[7]},
    {points[7], points[5]},
    {points[5], points[1]},
    {points[4], points[1]},
    {points[2], points[6]},
  };

  uint8_t vertices_count = sizeof(vertices) / sizeof(vertices[0]);

  float angleX = 0.0F;
  float angleY = 0.0F;
  bool auto_rotate = true;

  while (true) {
    uint64_t scan = extapp_scanKeyboard();

    if ((scan & SCANCODE_Home) > 0) break;
    if ((scan & SCANCODE_Plus) > 0) FPS++;
    if ((scan & SCANCODE_Minus) > 0) FPS--;

    if ((scan & SCANCODE_Down) > 0) angleX += PI/24;
    if ((scan & SCANCODE_Up) > 0) angleX -= PI/24;
    if ((scan & SCANCODE_Right) > 0) angleY += PI/24;
    if ((scan & SCANCODE_Left) > 0) angleY -= PI/24;

    if ((scan & SCANCODE_Backspace) > 0) {
      angleX = 0;
      angleY = 0;
    }

    if ((scan & SCANCODE_OK) > 0) auto_rotate = !auto_rotate;

    if (auto_rotate) {
      angleX += PI/24;
      angleY += PI/24;
    }

    draw_rectangle(0, 0, LCD_WIDTH, LCD_HEIGHT, 0);
    
    for (uint8_t i = 0; i < vertices_count; i++) {
      struct Point3D rotatedStartPoint = R3D_rotate_x(angleX, R3D_rotate_y(angleY, vertices[i].start));
      struct Point3D rotatedEndPoint = R3D_rotate_x(angleX, R3D_rotate_y(angleY, vertices[i].end));
      struct Point2D start = R3D_project(rotatedStartPoint);
      struct Point2D end = R3D_project(rotatedEndPoint);

      draw_line(start.x, start.y, end.x, end.y, 0xffff);

      char test[10];
      sprintf(test, "Sx: %f", start.x);
      extapp_drawTextSmall(test, 0, 0, 0xffff, 0, false);

      sprintf(test, "Sy: %f", start.y);
      extapp_drawTextSmall(test, 115, 0, 0xffff, 0, false);

      sprintf(test, "Ex: %f", end.x);
      extapp_drawTextSmall(test, 0, 13, 0xffff, 0, false);

      sprintf(test, "Ey: %f", end.y);
      extapp_drawTextSmall(test, 115, 13, 0xffff, 0, false);
    }

    char angleXText[10];
    sprintf(angleXText, "θx: %f", angleX);
    extapp_drawTextSmall(angleXText, 0, LCD_HEIGHT-12, 0xffff, 0, false);

    char angleYText[10];
    sprintf(angleYText, "θy: %f", angleY);
    extapp_drawTextSmall(angleYText, 115, LCD_HEIGHT-12, 0xffff, 0, false);

    char fpsText[8];
    sprintf(fpsText, "FPS: %i", FPS);
    extapp_drawTextSmall(fpsText, LCD_WIDTH - 60, 0, 0xffff, 0, false);

    char autoRotate[16];
    sprintf(autoRotate, "Auto Rotate: %i", auto_rotate);
    extapp_drawTextSmall(autoRotate, LCD_WIDTH - 100, LCD_HEIGHT-12, 0xffff, 0, false);

    extapp_msleep(1000/FPS);
  }
}