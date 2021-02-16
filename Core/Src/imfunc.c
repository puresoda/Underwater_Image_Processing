#include "imfunc.h"

/**
* Reciprocal Square Root Function
* Credit to Wikipedia: https://en.wikipedia.org/wiki/Fast_inverse_square_root
*
* @param:   number      Input to the recipricol square root
*
* @return:              1/sqrt(number) approximation
*/
float Q_rsqrt(float number)
{
    const float x2 = number * 0.5F;
    const float threehalfs = 1.5F;

    union
    {
        float f;
        uint32_t i;
    } conv = { .f = number };
    conv.i = 0x5f3759df - (conv.i >> 1);
    conv.f *= threehalfs - (x2 * conv.f * conv.f);
    return conv.f;
}

/*
* Calculate the average value of an image array.
*
* @param:   image       The array to average over
* @param:   num_pixels  The number of indices to average over
*
* @returns:             Returns the average (sum(image) / num_pixels)
*/
float calcAverage(float* image, const int num_pixels)
{
    float avg = 0;
    for (int i = 0; i < num_pixels; i++)
        avg += image[i];

    return (avg / num_pixels);
}

/**
* Applies gamma correction to an image and clips the value between [0,1].
* 
* @param:   image       Array containing the RGB image to apply correction to
* @param:   num_pixels  Number of pixels in the RGB image
* @param:   gamma       Amount of gamma correction to apply. corr_img = img^(gamma)
* 
* @return:              Creates a new array containing the gamma corrected image
*/
float* correctGama(float* image, const int num_pixels, const float gamma)
{
    const int rgb_size = 3 * num_pixels;

    float* gamma_image = malloc(sizeof(float) * rgb_size);

    for (int i = 0; i < rgb_size; i++)
    {
        image[i] = pow(image[i], gamma);

        // Confine resulting value to between 0 and 1
        image[i] = (image[i] < 0) ? 0 : image[i];
        image[i] = (image[i] > 1) ? 1 : image[i];
    }

    return gamma_image;
}

void applyGaussianBlur(float* image, const int num_pixels)
{
    // TODO: Once the 2D Convolution function is finished, convolve "image" with this matrix (3x3)
    float gaussian_filter[9] = {0.0113, 0.0838, 0.0113, 0.0838, 0.6193, 0.0838, 0.0113, 0.0838, 0.0113 };
    return;
}