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
#include <cmath>
#include <stdio.h>
#include <stdint.h>

// General Purpose Image Functions
float Q_rsqrt(float number);
float calcAverage(float* image, const int num_pixels);
float* correctGama(float* image, const int num_pixels, const float gamma);
void applyGaussianBlur(float* image, const int num_pixels);

#endif