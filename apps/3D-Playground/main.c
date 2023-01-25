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

  float angle = 0.0;

  while (true) {
    if (extapp_isKeydown(KEY_CHAR_MINUS)) break;
    if (extapp_isKeydown(KEY_CHAR_MULT)) {
      FPS++;
    }
    if (extapp_isKeydown(KEY_CHAR_0)) {
      FPS--;
    }

    angle += PI/30;

    draw_rectangle(0, 0, LCD_WIDTH, LCD_HEIGHT, 0);
    
    for (uint8_t i = 0; i < vertices_count; i++) {
      struct Point3D rotatedStartPoint = R3D_rotate_x(angle, R3D_rotate_y(angle, vertices[i].start));
      struct Point3D rotatedEndPoint = R3D_rotate_x(angle, R3D_rotate_y(angle, vertices[i].end));
      struct Point2D start = R3D_project(rotatedStartPoint);
      struct Point2D end = R3D_project(rotatedEndPoint);

      draw_line(start.x, start.y, end.x, end.y, 0xffff);

      char test[10] = "";
      sprintf(test, "%f", start.x);
      extapp_drawTextSmall(test, 0, 0, 0xffff, 0, false);

      sprintf(test, "%f", start.y);
      extapp_drawTextSmall(test, 150, 0, 0xffff, 0, false);

      sprintf(test, "%f", end.x);
      extapp_drawTextSmall(test, 0, LCD_HEIGHT-12, 0xffff, 0, false);

      sprintf(test, "%f", end.y);
      extapp_drawTextSmall(test, 150, LCD_HEIGHT-12, 0xffff, 0, false);
    }

    char fpsText[7];
    sprintf(fpsText, "FPS %i", FPS);
    extapp_drawTextSmall(fpsText, LCD_WIDTH - 50, 0, 0xffff, 0, false);

    extapp_msleep(1000/FPS);
  }
}