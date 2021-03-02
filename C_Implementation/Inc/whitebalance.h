#pragma once
#ifndef WHITE_H
#define WHITE_H

#include "imfunc.h"
#include "weights.h"

#define NUM_BINS (2 << 10)

float* applyWhiteBalance(float* image, const int num_row, const int num_col, const float alpha);
void  applyGreyWorld(float* image, const int num_pixels);
void linearizeRGB(float* image, const int num_pixels);
float linearizerHelper(const float pixel);

float* applyGreyWorldFull(float* image, const int num_pixels, const int percentile);
float calcIlluminant(float* image, const int num_pixels, const int percentile);
float* calcIlluminantRGB(float* image, const int num_pixels, const int percentile);
float* multiplyFlatMatrix(float* left_mat, float* right_mat, const int left_num_row, const int left_num_col, const int right_num_row, const int right_num_col);
int multiplyFlatMatrixRef(float* left_mat, float* right_mat, float* output, const int left_num_row, const int left_num_col, const int right_num_row, const int right_num_col);

#endif