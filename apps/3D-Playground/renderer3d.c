#include <math.h>

#include "renderer3d.h"
#include "extapp_api.h"

struct Point2D R3D_project(struct Point3D point) {
  struct Point2D point2d = {
    LCD_WIDTH / 2.0F + ((R3D_FOV * point.x) / (R3D_FOV + point.z) * 50.0F),
    LCD_HEIGHT / 2.0F + ((R3D_FOV * point.y) / (R3D_FOV + point.z) * 50.0F)
  };

  return point2d;
}

struct Point3D R3D_rotate_x(float angle, struct Point3D point) {
  struct Point3D rotated = {
    point.x,
    cosf(angle) * point.y - sinf(angle) * point.z,
    sinf(angle) * point.y + cosf(angle) * point.z
  };

  return rotated;
}

struct Point3D R3D_rotate_y(float angle, struct Point3D point) {
  struct Point3D rotated = {
    cosf(angle) * point.x - sinf(angle) * point.z,
    point.y,
    sinf(angle) * point.x + cosf(angle) * point.z
  };

  return rotated;
}