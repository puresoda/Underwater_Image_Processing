#include "whitebalance.h"

/**
* Applies white balance on an image to compensate for the attenuation of the red and blue channels underwater
*
* @param:   image       RGB image normalized on the interval [0,1]. The image is stored as [R1 R2 R3 ..., G1 G2 G3 ..., B1 B2 B3 ...]
* @param:   num_row     Number of rows in the image
* @param:   num_col     Number of columns in the image
* @param:   alpha       Multiplicative factor to control the amount of compensation (default should be 1)
*
* @return:              Modifies the original image with the correct white balance
*/
void applyWhiteBalance(float* image, const int num_row, const int num_col, const float alpha)
{
    const int num_pixels = num_row * num_col;

    float* red = image;
    float* green = &image[num_pixels];
    float* blue = &image[num_pixels * 2];

    float avg_R = calcAverage(red, num_pixels);
    float avg_G = calcAverage(blue, num_pixels);
    float avg_B = calcAverage(green, num_pixels);

    // Apply Red Channel Compensation
    // This accounts for the fact that longer wavelength light is attentuated with water depth
    for (int i = 0; i < num_pixels; i++)
    {
        red[i] += alpha * (avg_G - avg_R) * (1 - red[i]) * green[i];
    }

    // Apply Blue Channel Compensation
    // In turbid waters or with high concentration of planktons, we also use the following equation
    for (int i = 0; i < num_pixels; i++)
    {
        blue[i] += alpha * (avg_G - avg_B) * (1 - blue[i]) * green[i];
    }

    return;
}

// TODO: Implement Grey-World approximation function
float* applyGreyWorld(float* image, const int num_pixels)
{
    return NULL;
}