#include "inc/render.h"


void drawImage(int x, int y, obm_t img) {
    for (int i = 0; i < img.width; i++)
    {
        for (int j = 0; j < img.height; j++)
        {
            if ((img.pixels)[i + j * img.width] !=  0xF81F)
            {
                if (0 <= x + i && x + i < SCREEN_WIDTH  && 0 <= y + j && y + j < SCREEN_HEIGHT)
                {
                    extapp_pushRectUniform(x + i, y + j, 1, 1, (img.pixels)[i + j * img.width]);
                }
            }
        }
    }
}

void drawImageCropped(int x, int y, obm_t img, int cropX, int cropY, int cropW, int cropH) {
    
    int endX = (cropX + cropW < img.width) ? cropX + cropW : img.width;
    int endY = (cropY + cropH < img.height) ? cropY + cropH : img.height;
    
    
    if (cropX >= img.width || cropY >= img.height || cropX >= endX || cropY >= endY)
        return;
    
    uint16_t* pixels = img.pixels;
    int imgWidth = img.width;
    
    for (int j = cropY; j < endY; j++)
    {
        int rowOffset = j * imgWidth;
        int destY = y + j - cropY;
        
        for (int i = cropX; i < endX; i++)
        {
            uint16_t pixel = pixels[i + rowOffset];
            if (pixel != 0xF81F)
            {
                if (0 <= x + i - cropX && x + i - cropX < SCREEN_WIDTH  && 0 <= destY && destY < SCREEN_HEIGHT)
                {
                    extapp_pushRectUniform(x + i - cropX, destY, 1, 1, pixel);
                }
            }
        }
    }
}

void drawStretchedImage(int x, int y, int targetWidth, int targetHeight, obm_t img) {
   double scaleX = (double)img.width / targetWidth;
    double scaleY = (double)img.height / targetHeight;

    for (int destY = 0; destY <= targetHeight; destY++)
    {
        int srcY = (int)(destY * scaleY);
        if (srcY >= img.height)
            srcY = img.height - 1;

        for (int destX = 0; destX <= targetWidth; destX++)
        {
            int srcX = (int)(destX * scaleX);
            if (srcX >= img.width)
                srcX = img.width - 1;

            uint16_t pixel = img.pixels[srcX + srcY * img.width];
            if (pixel !=  0xF81F)
            {
                if (0 <= x + destX && x + destX < SCREEN_WIDTH  && 0 <= y + destY && y + destY < SCREEN_HEIGHT)
                {
                    extapp_pushRectUniform(x + destX, y + destY, 1, 1, pixel);
                }
            }
        }
    }
}

void drawImageScaled(int x, int y, obm_t img, float scale) {
    drawStretchedImage(x, y, img.width * scale, img.height * scale, img);
}

void drawStretchedImageCropped(int x, int y, int targetWidth, int targetHeight, obm_t img, int cropX, int cropY, int cropW, int cropH) {

    int endX = (cropX + cropW < img.width) ? cropX + cropW : img.width;
    int endY = (cropY + cropH < img.height) ? cropY + cropH : img.height;
    

    if (cropX >= img.width || cropY >= img.height || cropX >= endX || cropY >= endY)
        return;
    
    double scaleX = (double)cropW / targetWidth;
    double scaleY = (double)cropH / targetHeight;

    for (int destY = 0; destY <= targetHeight; destY++)
    {
        int srcY = cropY + (int)(destY * scaleY);
        if (srcY >= endY)
            srcY = endY - 1;

        for (int destX = 0; destX <= targetWidth; destX++)
        {
            int srcX = cropX + (int)(destX * scaleX);
            if (srcX >= endX)
                srcX = endX - 1;

            uint16_t pixel = img.pixels[srcX + srcY * img.width];
            if (pixel != 0xF81F)
            {
                if (0 <= x + destX && x + destX < SCREEN_WIDTH && 0 <= y + destY && y + destY < SCREEN_HEIGHT)
                {
                    extapp_pushRectUniform(x + destX, y + destY, 1, 1, pixel);
                }
            }
        }
    }
}

void drawStretchedImageCroppedMask(int x, int y, int targetWidth, int targetHeight, obm_t img, int cropX, int cropY, int cropW, int cropH, uint16_t color) {

    int endX = (cropX + cropW < img.width) ? cropX + cropW : img.width;
    int endY = (cropY + cropH < img.height) ? cropY + cropH : img.height;
    

    if (cropX >= img.width || cropY >= img.height || cropX >= endX || cropY >= endY)
        return;
    
    double scaleX = (double)cropW / targetWidth;
    double scaleY = (double)cropH / targetHeight;

    for (int destY = 0; destY <= targetHeight; destY++)
    {
        int srcY = cropY + (int)(destY * scaleY);
        if (srcY >= endY)
            srcY = endY - 1;

        for (int destX = 0; destX <= targetWidth; destX++)
        {
            int srcX = cropX + (int)(destX * scaleX);
            if (srcX >= endX)
                srcX = endX - 1;

            uint16_t pixel = img.pixels[srcX + srcY * img.width];
            if (pixel != 0xF81F)
            {
                if (0 <= x + destX && x + destX < SCREEN_WIDTH && 0 <= y + destY && y + destY < SCREEN_HEIGHT)
                {
                    extapp_pushRectUniform(x + destX, y + destY, 1, 1, color);
                }
            }
        }
    }
}

void drawImageRotated(int x, int y, obm_t img, double angle) {

    double cosA = cos(angle);
    double sinA = sin(angle);
    
    int centerX = img.width / 2;
    int centerY = img.height / 2;
    

    int maxDim = (int)(sqrt(img.width * img.width + img.height * img.height)) + 1;
    
    for (int destY = -maxDim; destY < maxDim; destY++)
    {
        for (int destX = -maxDim; destX < maxDim; destX++)
        {

            int srcX = (int)(destX * cosA + destY * sinA) + centerX;
            int srcY = (int)(-destX * sinA + destY * cosA) + centerY;
            
            if (srcX >= 0 && srcX < img.width && srcY >= 0 && srcY < img.height)
            {
                uint16_t pixel = img.pixels[srcX + srcY * img.width];
                if (pixel != 0xF81F)
                {
                    int screenX = x + destX + centerX;
                    int screenY = y + destY + centerY;
                    if (screenX >= 0 && screenX < SCREEN_WIDTH && screenY >= 0 && screenY < SCREEN_HEIGHT)
                    {
                        extapp_pushRectUniform(screenX, screenY, 1, 1, pixel);
                    }
                }
            }
        }
    }
}

void drawImageRotatedAndRemovePrevious(int x, int y, obm_t img, double angle, int prev_x, int prev_y, obm_t prev_img, double prev_angle) {
    const uint16_t BG = 0xFFFF;

    double cosA = cos(angle);
    double sinA = sin(angle);
    double prevCosA = cos(prev_angle);
    double prevSinA = sin(prev_angle);

    int centerX = img.width / 2;
    int centerY = img.height / 2;
    int prevCenterX = prev_img.width / 2;
    int prevCenterY = prev_img.height / 2;

    int maxDim = (int)(sqrt(img.width * img.width + img.height * img.height)) + 1;
    int prevMaxDim = (int)(sqrt(prev_img.width * prev_img.width + prev_img.height * prev_img.height)) + 1;

    int cx = x + centerX;
    int cy = y + centerY;
    int pcx = prev_x + prevCenterX;
    int pcy = prev_y + prevCenterY;

    int minX = cx - maxDim;
    int maxX = cx + maxDim - 1;
    int minY = cy - maxDim;
    int maxY = cy + maxDim - 1;

    int pminX = pcx - prevMaxDim;
    int pmaxX = pcx + prevMaxDim - 1;
    int pminY = pcy - prevMaxDim;
    int pmaxY = pcy + prevMaxDim - 1;

    if (pminX < minX) minX = pminX;
    if (pminY < minY) minY = pminY;
    if (pmaxX > maxX) maxX = pmaxX;
    if (pmaxY > maxY) maxY = pmaxY;

    if (minX < 0) minX = 0;
    if (minY < 0) minY = 0;
    if (maxX >= SCREEN_WIDTH) maxX = SCREEN_WIDTH - 1;
    if (maxY >= SCREEN_HEIGHT) maxY = SCREEN_HEIGHT - 1;

    uint16_t* pixelsNew = img.pixels;
    uint16_t* pixelsPrev = prev_img.pixels;
    int imgW = img.width, imgH = img.height;
    int prevW = prev_img.width, prevH = prev_img.height;

    for (int screenY = minY; screenY <= maxY; ++screenY) {
        int relYNew = screenY - cy;
        int relYPrev = screenY - pcy;

        for (int screenX = minX; screenX <= maxX; ++screenX) {
            int relXNew = screenX - cx;
            int srcXNew = (int)(relXNew * cosA + relYNew * sinA) + centerX;
            int srcYNew = (int)(-relXNew * sinA + relYNew * cosA) + centerY;

            int newOpaque = 0;
            uint16_t outColor;

            if ((unsigned)srcXNew < (unsigned)imgW && (unsigned)srcYNew < (unsigned)imgH) {
                uint16_t px = pixelsNew[srcXNew + srcYNew * imgW];
                if (px != 0xF81F) {
                    extapp_pushRectUniform(screenX, screenY, 1, 1, px);
                    continue;
                }
            }

            int relXPrev = screenX - pcx;
            int srcXPrev = (int)(relXPrev * prevCosA + relYPrev * prevSinA) + prevCenterX;
            int srcYPrev = (int)(-relXPrev * prevSinA + relYPrev * prevCosA) + prevCenterY;

            if ((unsigned)srcXPrev < (unsigned)prevW && (unsigned)srcYPrev < (unsigned)prevH) {
                uint16_t ppx = pixelsPrev[srcXPrev + srcYPrev * prevW];
                if (ppx != 0xF81F) {
                    extapp_pushRectUniform(screenX, screenY, 1, 1, BG);
                }
            }
            
        }
    }
}


void drawImageRotatedMask(int x, int y, obm_t img, double angle, uint16_t color) {

    double cosA = cos(angle);
    double sinA = sin(angle);
    
    int centerX = img.width / 2;
    int centerY = img.height / 2;

    int maxDim = (int)(sqrt(img.width * img.width + img.height * img.height)) + 1;
    
    for (int destY = -maxDim; destY < maxDim; destY++)
    {
        for (int destX = -maxDim; destX < maxDim; destX++)
        {

            int srcX = (int)(destX * cosA + destY * sinA) + centerX;
            int srcY = (int)(-destX * sinA + destY * cosA) + centerY;
            
            if (srcX >= 0 && srcX < img.width && srcY >= 0 && srcY < img.height)
            {
                uint16_t pixel = img.pixels[srcX + srcY * img.width];
                if (pixel != 0xF81F)
                {
                    int screenX = x + destX + centerX;
                    int screenY = y + destY + centerY;
                    if (screenX >= 0 && screenX < SCREEN_WIDTH && screenY >= 0 && screenY < SCREEN_HEIGHT)
                    {
                        extapp_pushRectUniform(screenX, screenY, 1, 1, color);
                    }
                }
            }
        }
    }
}

void drawImageCroppedRotated(int x, int y, obm_t img, int cropX, int cropY, int cropW, int cropH, double angle) {

    double cosA = cos(angle);
    double sinA = sin(angle);
    
    int centerX = cropW / 2;
    int centerY = cropH / 2;
    
    int maxDim = (int)(sqrt(cropW * cropW + cropH * cropH)) + 1;
    
    for (int destY = -maxDim; destY < maxDim; destY++)
    {
        for (int destX = -maxDim; destX < maxDim; destX++)
        {
            int srcX = (int)(destX * cosA + destY * sinA) + centerX + cropX;
            int srcY = (int)(-destX * sinA + destY * cosA) + centerY + cropY;
            
            if (srcX >= cropX && srcX < cropX + cropW && srcY >= cropY && srcY < cropY + cropH &&
                srcX >= 0 && srcX < img.width && srcY >= 0 && srcY < img.height)
            {
                uint16_t pixel = img.pixels[srcX + srcY * img.width];
                if (pixel != 0xF81F)
                {
                    int screenX = x + destX + centerX;
                    int screenY = y + destY + centerY;
                    if (screenX >= 0 && screenX < SCREEN_WIDTH && screenY >= 0 && screenY < SCREEN_HEIGHT)
                    {
                        extapp_pushRectUniform(screenX, screenY, 1, 1, pixel);
                    }
                }
            }
        }
    }
}


void drawImageCroppedRotatedMask(int x, int y, obm_t img, int cropX, int cropY, int cropW, int cropH, double angle, uint16_t color) {

    double cosA = cos(angle);
    double sinA = sin(angle);
    
    int centerX = cropW / 2;
    int centerY = cropH / 2;
    
    int maxDim = (int)(sqrt(cropW * cropW + cropH * cropH)) + 1;
    
    for (int destY = -maxDim; destY < maxDim; destY++)
    {
        for (int destX = -maxDim; destX < maxDim; destX++)
        {
            int srcX = (int)(destX * cosA + destY * sinA) + centerX + cropX;
            int srcY = (int)(-destX * sinA + destY * cosA) + centerY + cropY;
            
            if (srcX >= cropX && srcX < cropX + cropW && srcY >= cropY && srcY < cropY + cropH &&
                srcX >= 0 && srcX < img.width && srcY >= 0 && srcY < img.height)
            {
                uint16_t pixel = img.pixels[srcX + srcY * img.width];
                if (pixel != 0xF81F)
                {
                    int screenX = x + destX + centerX;
                    int screenY = y + destY + centerY;
                    if (screenX >= 0 && screenX < SCREEN_WIDTH && screenY >= 0 && screenY < SCREEN_HEIGHT)
                    {
                        extapp_pushRectUniform(screenX, screenY, 1, 1, color);
                    }
                }
            }
        }
    }
}


void drawStretchedImageRotated(int x, int y, int targetWidth, int targetHeight, obm_t img, double angle) {

    double cosA = cos(angle);
    double sinA = sin(angle);
    double scaleX = (double)img.width / targetWidth;
    double scaleY = (double)img.height / targetHeight;
    
    int centerX = targetWidth / 2;
    int centerY = targetHeight / 2;
    
    int maxDim = (int)(sqrt(targetWidth * targetWidth + targetHeight * targetHeight)) + 1;
    
    for (int destY = -maxDim; destY < maxDim; destY++)
    {
        for (int destX = -maxDim; destX < maxDim; destX++)
        {
            int scaledX = (int)(destX * cosA + destY * sinA) + centerX;
            int scaledY = (int)(-destX * sinA + destY * cosA) + centerY;
            
            if (scaledX >= 0 && scaledX < targetWidth && scaledY >= 0 && scaledY < targetHeight)
            {
                int srcX = (int)(scaledX * scaleX);
                int srcY = (int)(scaledY * scaleY);
                
                if (srcX >= 0 && srcX < img.width && srcY >= 0 && srcY < img.height)
                {
                    uint16_t pixel = img.pixels[srcX + srcY * img.width];
                    if (pixel != 0xF81F)
                    {
                        int screenX = x + destX + centerX;
                        int screenY = y + destY + centerY;
                        if (screenX >= 0 && screenX < SCREEN_WIDTH && screenY >= 0 && screenY < SCREEN_HEIGHT)
                        {
                            extapp_pushRectUniform(screenX, screenY, 1, 1, pixel);
                        }
                    }
                }
            }
        }
    }
}