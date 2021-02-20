#pragma once
#ifndef WHITE_H
#define WHITE_H

#include "imfunc.h"

void applyWhiteBalance(float* image, const int num_row, const int num_col, const float alpha);
float* applyGreyWorld(float* image, const int num_pixels);
float* linearizeRGB(float* image, const int num_pixels);
float linearizerHelper(const float pixel);

#endif