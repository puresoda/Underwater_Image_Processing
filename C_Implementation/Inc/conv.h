#pragma once
#ifndef CONV_H
#define CONV_H

#include <stdlib.h>
#include <malloc.h>

void padMatrix(float* input, float* pad_mat, const int input_num_row, const int input_num_col, const int filter_size);
void convHelper(float* input, float* filter, float* output, const int input_num_row, const int input_num_col, const int filter_size);
float* conv2D(float* input, float* filter, const int input_num_row, const int input_num_col, const int filter_size);

#endif // !CONV_H
