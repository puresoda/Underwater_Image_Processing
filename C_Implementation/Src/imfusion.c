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
    // For timing each function
    clock_t start = clock(), diff;
    int msec = 0;

    // Read in the file
    printf("----------------------------------------------------------------------------------\n\n");
    struct Image rgb = readImage(filename);
    const int num_pixels = rgb.num_row * rgb.num_col;
    diff = clock() - start;
    msec = diff * 1000 / CLOCKS_PER_SEC;
    printf("Elapsed time was %d seconds and %d millseconds\n", msec / 1000, msec % 1000);
    printf("----------------------------------------------------------------------------------\n\n");

    //------------------------------------------------------
    // White Balance 
    //------------------------------------------------------
    start = clock();
    float* white = applyWhiteBalance(rgb.rgb_image, rgb.num_row, rgb.num_col, 1);
    printf("Finished White Balance!\n");
    diff = clock() - start;
    msec = diff * 1000 / CLOCKS_PER_SEC;
    printf("Elapsed time was %d seconds and %d millseconds\n", msec / 1000, msec % 1000);
    printf("----------------------------------------------------------------------------------\n\n");

    //------------------------------------------------------
    // Gamma weights 
    //------------------------------------------------------
    start = clock();
    float* gamma = correctGamma(white, num_pixels, 1.2);
    printf("Finished Gamma Correction!\n");

    float* gamma_weight = getWeights(gamma, rgb.num_row, rgb.num_col, LUM_OPTION);

    printf("Finished Gamma Weight Calculation!\n");
    diff = clock() - start;
    msec = diff * 1000 / CLOCKS_PER_SEC;
    printf("Elapsed time was %d seconds and %d millseconds\n", msec / 1000, msec % 1000);
    printf("----------------------------------------------------------------------------------\n\n");
    free(gamma);

    //------------------------------------------------------
    // Sharpening Weigths
    //------------------------------------------------------
    start = clock();
    float* sharp = applyUnsharpMask(white, rgb.num_row, rgb.num_col);
    printf("Finished Unsharp Mask!\n");

    float* sharp_weight = getWeights(sharp, rgb.num_row, rgb.num_col, LUM_OPTION);
    printf("Finished Unsharp Mask Weight Calculation!\n");
    diff = clock() - start;
    msec = diff * 1000 / CLOCKS_PER_SEC;
    printf("Elapsed time was %d seconds and %d millseconds\n", msec / 1000, msec % 1000);
    printf("----------------------------------------------------------------------------------\n\n");

    free(sharp);

    //------------------------------------------------------
    // Image fusion
    //------------------------------------------------------
    start = clock();
    float* reconstructed = applyFusion(white, gamma_weight, sharp_weight, rgb.num_row, rgb.num_col);
    printf("Finished Image Fusion!\n");
    diff = clock() - start;
    msec = diff * 1000 / CLOCKS_PER_SEC;
    printf("Elapsed time was %d seconds and %d millseconds\n", msec / 1000, msec % 1000);
    printf("----------------------------------------------------------------------------------\n\n");

    //------------------------------------------------------
    // Write the output to text file
    //-----------------------------------------------------
    start = clock();
    writeImage("underwater_bitmap", reconstructed, rgb.num_row, rgb.num_col);
    diff = clock() - start;
    msec = diff * 1000 / CLOCKS_PER_SEC;
    printf("Elapsed time was %d seconds and %d millseconds\n", msec / 1000, msec % 1000);
    printf("----------------------------------------------------------------------------------\n\n");

    printf("\n\nCleaning up allocated memory now...\n");

    free(rgb.rgb_image);
    free(sharp_weight);
    free(gamma_weight);
    free(reconstructed);

    printf("Done!\n");
    return 0;
}
