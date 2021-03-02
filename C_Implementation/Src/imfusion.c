#include "../Inc/imfusion.h"

float* applyFusion(float* white_image, float* gamma_weight, float* sharp_weight, const int num_row, const int num_col)
{
    const int num_pixel = num_row * num_col;
    const int num_channel = 3;
    const int num_rgb = num_pixel * num_channel;

    // Apply regularization / normalization to the weights
    normalizeFusionWeights(gamma_weight, sharp_weight, num_row, num_col);

    float* reconstructed = malloc(sizeof(float) * num_rgb);

    // Apply Naive Fusion which is simply the Haddamard product between the image and combined weight
    for (int i = 0; i < num_rgb; i++)
        reconstructed[i] = white_image[i] * (gamma_weight[i % num_pixel] + sharp_weight[i % num_pixel]);

    return reconstructed;
}

void normalizeFusionWeights(float* gamma_weight, float* sharp_weight, const int num_row, const int num_col)
{
    const int num_pixel = num_row * num_col;

    // Copy the values of gamma weight since we modify them but need original values to normalize sharp_weight
    float* temp_gamma = malloc(sizeof(float) * num_pixel);
    for (int i = 0; i < num_pixel; i++)
        temp_gamma[i] = gamma_weight[i];

    // Normalization
    // new weight = (old + regularization) / (sum(weight) + 2*regularization)
    for (int i = 0; i < num_pixel; i++)
    {
        gamma_weight[i] = (temp_gamma[i]   + REGULARIZATION) / (sharp_weight[i] + temp_gamma[i] + 2.0f * REGULARIZATION);
        sharp_weight[i] = (sharp_weight[i] + REGULARIZATION) / (sharp_weight[i] + temp_gamma[i] + 2.0f * REGULARIZATION);
    }

    free(temp_gamma);
    return;
}

float* imageFusionSeqFull(char filename[])
{
    // Read in the file
    struct Image rgb = readImage(filename);
    const int num_pixels = rgb.num_row * rgb.num_col;

    // White Balance 
    applyWhiteBalance(rgb.rgb_image, rgb.num_row, rgb.num_col, 1);
    float* white = rgb.rgb_image;
    printf("Finished White Balance!\n");

    writeImage("underwater_bitmap", rgb.rgb_image, rgb.num_row, rgb.num_col);

    // Gamma weights 
    float* gamma = correctGamma(white, num_pixels, 1.4);
    printf("Finished Gamma Correction!\n");

    float* gamma_weight = getWeights(gamma, rgb.num_row, rgb.num_col, LUM_OPTION);
    printf("Finished Gamma Weight Calculation!\n");

    free(gamma);

    // Sharpening Weigths
    float* sharp = applyUnsharpMask(white, rgb.num_row, rgb.num_col);
    printf("Finished Unsharp Mask!\n");

    float* sharp_weight = getWeights(sharp, rgb.num_row, rgb.num_col, LUM_OPTION);
    printf("Finished Unsharp Mask Weight Calculation!\n");

    free(sharp);

    float* reconstructed = applyFusion(white, gamma_weight, sharp_weight, rgb.num_row, rgb.num_col);

    writeImage("underwater_bitmap", reconstructed, rgb.num_row, rgb.num_col);

    free(rgb.rgb_image);
    free(sharp_weight);
    free(gamma_weight);
    free(reconstructed);

    return 0;
}
