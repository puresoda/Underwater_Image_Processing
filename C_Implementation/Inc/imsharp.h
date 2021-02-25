#pragma once
#ifndef IMSARP_H
#define IMSHARP_H

#include "imfunc.h"
#include "hsi.h"

float* applyUnsharpMask(float* image, const int num_row, const int num_col);
void histogramEqualization(float* image, const int num_pixels);

#endif // ! IMSARP_H
