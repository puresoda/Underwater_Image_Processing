#pragma once
#ifndef IMFUNC_H
#define IMFUNC_H

// MACROS
#define MAX(x,y) (x > y) ? x : y
#define MIN(x,y) (x < y) ? x : y
#define ABS(x) (x < 0) ? -x : x

// Standard includes
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "conv.h"
#include <stdint.h>

struct Image
{
	int num_row;
	int num_col;
	float* rgb_image;
};

// General Purpose Image Functions
float Q_rsqrt(float number);
float calcAverage(float* image, const int num_pixels);
float* correctGamma(float* image, const int num_pixels, const float gamma);
float* applyGaussianBlur(float* image, const int num_row, const int num_col);
void applyGaussianBlurRef(float* image, float* output, const int num_row, const int num_col);
float* applyLaplacian(float* image, const int num_row, const int num_col);
float calcNormSquare(const float x1, const float x2, const float y1, const float y2, const float z1, const float z2);

// Image Reading and writing
struct Image readImage(const char file_name[]);
int writeImage(const char file_name[], float* image, const int num_row, const int num_col);

#endif