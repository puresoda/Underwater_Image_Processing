#pragma once
#ifndef IMFUSION_H
#define IMFUSION_H

#define REGULARIZATION 0.1
#define LUM_OPTION 1

// Standard includes
#include "imsharp.h"
#include "weights.h"
#include "whitebalance.h"
#include "imfunc.h"


// Fusion Functions
float* applyFusion(float* white_image, float* gamma_weight, float* sharp_weight, const int num_row, const int num_col);
void normalizeFusionWeights(float* gamma_weight, float* sharp_weight, const int num_row, const int num_col);

// Helper function to perform all steps of fusion
float* imageFusionFull(char filename[]);

#endif