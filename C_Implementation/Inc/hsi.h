#pragma once
#ifndef HSI_H
#define HSI_H

#include "imfunc.h"

// Main Conversion Functions
float* rgb2hsi(float* rgb_image, const int num_pixels);
float* hsi2rgb(float* hsi, const int num_pixels);

// Conversion of Individual Components
void calcHue(float* rgb, float* hsi, const int num_pixels);
void calcSaturation(float* rgb, float* hsi, const int num_pixels);
void calcIntensity(float* rgb, float* hsi, const int num_pixels);

// Helper Functions
float getRGBAverage(const float red, const float green, const float blue);
float getRGBMin(const float red, const float green, const float blue);

void permuteColors(const float hue, const float primary, const float secondary, const float tertiary, float* red, float* green, float* blue);
#endif