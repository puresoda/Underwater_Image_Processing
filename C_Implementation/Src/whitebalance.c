#include "../Inc/whitebalance.h"

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
    float avg_G = calcAverage(green, num_pixels);
    float avg_B = calcAverage(blue, num_pixels);

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

/**
* Implements an approximation of the Grey World Approximation. The full Grey World approximation will be implemented later.
*/
float* applyGreyWorld(float* image, const int num_pixels)
{
    const int num_channels = 3;
    float scale_factor = 0;
    float sum = 0;

    float* corrected = malloc(sizeof(float) * num_channels * num_pixels);
    for (int i = 0; i < num_channels; i++)
    {
        sum = 0;
        for (int j = 0; j < num_pixels; j++)
            sum += image[i * num_pixels + j];

        scale_factor = sum / (float) num_pixels;

        for (int j = 0; j < num_pixels; j++)
            corrected[i * num_pixels + j] = image[i * num_pixels + j] * (127.5 / scale_factor / 255.0);
    }

    return corrected;
}

/**
* Linearizes RGB image to remove the gamma correction present in SRGB images.
* 
* @param:   image       The RGB image to linearize
* @param:   num_pixels  The number of pixels in the image
* 
* @return:              Linearized version of the input image
*/
float* linearizeRGB(float* image, const int num_pixels)
{
    float* linearized = malloc(sizeof(float) * num_pixels);
    for (int i = 0; i < num_pixels; i++)
        linearized[i] = linearizerHelper(image[i]);

    return linearized;
}

/**
* Performs linearization on a single pixel according to the following equation:
* 
*         | -f(-u)                  if u < 0
* f(u) =  {  u * c                  if 0 < u < d
*         |  (u*a + b)^(lambda)     if u >= d
* 
* @param:   pixel   Pixel to be linearized
* 
* @return:          Linearized pixel
*/
float linearizerHelper(float pixel)
{
    // Constants for linearization
    const float a = 1.0 / 1.055;
    const float b = 0.055 / 1.055;
    const float c = 1.0 / 12.92;
    const float d = 0.04045;
    const float lambda = 2.4;

    if (pixel < 0)
        return -1.0*linearizerHelper(ABS(pixel));

    else if (pixel >= 0 && pixel < d)
        return(pixel * c);

    else
        return pow(pixel * a + b, lambda);
}