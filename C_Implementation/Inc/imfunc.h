#pragma once
#ifndef IMFUNC_H
#define IMFUNC_H

// MACROS
#define MAX(x,y) (x > y) ? x : y
#define MIN(x,y) (x < y) ? x : y
#define ABS(x) (x < 0) ? -x : x

// Standard includes
#include <math.h>
#include <stdlib.h>
#include "conv.h"
#include <stdint.h>

// General Purpose Image Functions
float Q_rsqrt(float number);
float calcAverage(float* image, const int num_pixels);
float* correctGamma(float* image, const int num_pixels, const float gamma);
float* applyGaussianBlur(float* image, const int num_row, const int num_col);
float* applyLaplacian(float* image, const int num_row, const int num_col);
float calcNormSquare(const float x1, const float x2, const float y1, const float y2, const float z1, const float z2);

#endif