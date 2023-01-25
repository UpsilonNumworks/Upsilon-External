#include <stdint.h>

#define R3D_FOV 10

struct Point3D {
  float x;
  float y;
  float z;
};

struct Point2D {
  float x;
  float y;
};

struct Vertex {
  struct Point3D start;
  struct Point3D end;
};

struct Point2D R3D_project(struct Point3D point);
struct Point3D R3D_rotate_x(float angle, struct Point3D point);
struct Point3D R3D_rotate_y(float angle, struct Point3D point);