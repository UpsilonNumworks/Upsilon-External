#ifndef RAYCASTER_RENDER_H_
#define RAYCASTER_RENDER_H_

#include "libraries.h"
#include "math_functions.h"
#include "textures.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

void drawImage(int x, int y, obm_t img);
void drawImageCropped(int x, int y, obm_t img, int cropX, int cropY, int cropW, int cropH);
void drawStretchedImage(int x, int y, int targetWidth, int targetHeight, obm_t img);
void drawImageScaled(int x, int y, obm_t img, float scale);
void drawStretchedImageCropped(int x, int y, int targetWidth, int targetHeight, obm_t img, int cropX, int cropY, int cropW, int cropH);
void drawImageRotated(int x, int y, obm_t img, double angle);
void drawImageCroppedRotated(int x, int y, obm_t img, int cropX, int cropY, int cropW, int cropH, double angle);
void drawStretchedImageRotated(int x, int y, int targetWidth, int targetHeight, obm_t img, double angle);
void drawStretchedImageCroppedMask(int x, int y, int targetWidth, int targetHeight, obm_t img, int cropX, int cropY, int cropW, int cropH, uint16_t color);
void drawImageCroppedRotatedMask(int x, int y, obm_t img, int cropX, int cropY, int cropW, int cropH, double angle, uint16_t color);
void drawImageRotatedMask(int x, int y, obm_t img, double angle, uint16_t color);
void drawImageRotatedAndRemovePrevious(int x, int y, obm_t img, double angle, int prev_x, int prev_y, obm_t prev_img, double prev_angle);
#endif