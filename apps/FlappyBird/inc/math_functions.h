#ifndef RAYCASTER_MATH_FUNC_H_
#define RAYCASTER_MATH_FUNC_H_

#include "libraries.h"

#define M_PI 3.14159265359

double sqrt(double x);
double fmod(double x, double y);
double cos(double angle);
double sin(double angle);
double tan(double angle);
double fabs(double x);
double acos(double x);
double asin(double x);
double atan(double x);
double atan2(double y, double x);
int fast_exp(int x, int y);
int floorf(double x);
int ceilf(double x);
float fabsf(float x);
#endif