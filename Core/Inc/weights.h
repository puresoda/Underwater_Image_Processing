#pragma once
#ifndef WEIGHTS_H
#define WEIGHTS_H

#include "imfunc.h"

// Weight Functions
float* calcLaplacian(float* lum, const int num_pixels);
float* calcSaliency(float* image, const int num_pixels);
float calcSaturation(float* image, float* lum, const int num_pixels);
float* calcLuminance(float* image, const int num_pixels, const int lum_option);	

// Helper Functions
void normalizeWeight(float* weight, const int num_pixels);
float* getWeights(float* image, const int num_pixels, const int lum_option);

// Color Conversion Functions
void rgb2LAB(float* image, const int num_pixels);
float* xyz2LAB(float* image, const int num_pixels);
float* rgb2XYZ(float* image, const int num_pixels);
float labFunction(const float a, const float b);

#endif