#include "../Inc/hsi.h"

/**
* Converts from the Hue-Saturation-Intensity color space to the RGB color space.
*
* @param:   rgb_image   The RGB image array stored in the form [R1 R2... G1 G2... B1 B2...]
* @param:   num_pixels  The number of pixels in the RGB image
*
* @return:              Converted HSI image in the form [R1 R2... G1 G2... B1 B2...]
*
*/
float* rgb2hsi(float* rgb_image, const int num_pixels)
{
    float* hsi = malloc(sizeof(float) * num_pixels * 3);

    float* red = rgb_image;
    float* green = &rgb_image[num_pixels];
    float* blue = &rgb_image[num_pixels*2];

    float max_rgb, min_rgb;
    float delta;
    enum color {r , g, b};
    enum color max_color;

    for (int i = 0; i < num_pixels; i++)
    {
        // Get the minimum of the pair
        min_rgb = getRGBMin(red[i], green[i], blue[i]);

        // Get the maximum of the RGB pair
        if (red[i] > green[i])
        {
            max_rgb = red[i];
            max_color = r;
        }

        else
        {
            max_rgb = green[i];
            max_color = g;
        }

        if (max_rgb < blue[i])
        {
            max_rgb = blue[i];
            max_color = b;
        }

        delta = max_rgb - min_rgb;

        switch (max_color)
        {
        case r:
            hsi[i] = 60.0f * ((int)((green[i] - blue[i]) / (delta)) % 6);
            break;

        case g:
            hsi[i] = 60.0f * (((blue[i] - red[i]) / (delta)) + 2.0f);
            break;

        case b:
            hsi[i] = 60.0f * (((red[i] - green[i]) / (delta)) + 4.0f);
            break;
        }

        hsi[i + num_pixels] = (max_rgb == 0) ? 0 : (delta / max_rgb);
        hsi[i + 2 * num_pixels] = max_rgb;
    }

    return hsi;
}

/**
* Converts from the Hue-Saturation-Intensity color space to the RGB color space.
*
* @param:   hsi         The Hue-Saturation-Intensity array stored in the form [H1 H2... S1 S2... I1 I2...]
* @param:   num_pixels  The number of pixels in the RGB image
*
* @return:              Converted RGB image in the form [R1 R2... G1 G2... B1 B2...]
*/
float* hsi2rgb(float* hsi, const int num_pixels)
{
    // Create pointers to make indexing the image easier
    float* hue = hsi;
    float* sat = &hsi[num_pixels];
    float* intensity = &hsi[2 * num_pixels];

    // Allocate new memory for the RGB image
    float* rgb = malloc(sizeof(float) * num_pixels * 3);
    float* red = rgb;
    float* green = &rgb[num_pixels];
    float* blue = &rgb[2 * num_pixels];

    float primary, secondary, tertiary;
    for (int i = 0; i < num_pixels; i++)
    {
        primary = intensity[i] * sat[i];
        secondary = primary * (1 -ABS((int)(hue[i] / 60.0) % 2 - 1));
        tertiary = intensity[i] - primary;

        permuteColors(hue[i], primary, secondary, tertiary, &red[i], &green[i], &blue[i]);
    }

    return rgb;
}

/**
* Simple function to get the average of an RGB Pair
* 
* @param:   red     Red value of RGB pair
* @param:   green   Green value
* @param:   blue    Blue value
* 
* @return:          Average of all the inputs
*/
float getRGBAverage(const float red, const float green, const float blue)
{
    return (red + green + blue) / 3.0f;
}

/**
* Simple function to get the minimum of an RGB Pair
*
* @param:   red     Red value of RGB pair
* @param:   green   Green value
* @param:   blue    Blue value
*
* @return:          Minimum of the pair
*/
float getRGBMin(const float red, const float green, const float blue)
{
    float min_rgb = MIN(red,green);
    min_rgb = MIN(min_rgb, blue);

    return min_rgb;
}

/**
* Simple function to get the maximum of an RGB Pair
*
* @param:   red     Red value of RGB pair
* @param:   green   Green value
* @param:   blue    Blue value
*
* @return:          Maximum of the pair
*/
float getRGBMax(const float red, const float green, const float blue)
{
    float max_rgb = MAX(red, green);
    max_rgb = MAX(max_rgb, blue);

    return max_rgb;
}

/**
* Function to correctly allocate values to RGB pixels based on the hue.
* 
* @param:   hue         Hue of the pixel
* @param:   primary     First RGB value (intensity * saturation)
* @param:   secondary   Second RGB value (intensity * saturation * (1 - | mod2(hue / 60 - 1 |)
* @param:   tertiary    Third RGB value (intensity - primary)
* @param:   red         Red pixel
* @param    green       Green pixel
* @param:   blue        Blue pixel
* 
* @return:              Modifies red, blue, and green pixels by reference
*/
void permuteColors(const float hue, const float primary, const float secondary, const float tertiary, float* red, float* green, float* blue)
{
    if (0 <= hue && hue < 60)
    {
        *red = primary + tertiary;
        *green = secondary + tertiary;
        *blue = tertiary;
    }

    else if (60 <= hue && hue < 120)
    {
        *green = primary + tertiary;
        *red = secondary + tertiary;
        *blue = tertiary;
    }

    else if (120 <= hue && hue < 180)
    {
        *green = primary + tertiary;
        *blue = secondary + tertiary;
        *red = tertiary;
    }

    else if (180 <= hue && hue < 240)
    {
        *blue = primary + tertiary;
        *green = secondary + tertiary;
        *red = tertiary;
    }

    else if (240 <= hue && hue < 300)
    {
        *blue = primary + tertiary;
        *red = secondary + tertiary;
        *green = tertiary;
    }

    else
    {
        *red = primary + tertiary;
        *blue = secondary + tertiary;
        *green = tertiary;
    }

    return;
}