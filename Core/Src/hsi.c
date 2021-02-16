#include "hsi.h"

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
    float* hsi = malloc(sizeof(float) * num_pixels);

    calcHue(rgb_image, hsi, num_pixels);
    calcSaturation(rgb_image, hsi, num_pixels);
    calcIntensity(rgb_image, hsi, num_pixels);

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
    float* rgb = malloc(sizeof(float) * num_pixels);
    float* red = rgb;
    float* green = &rgb[num_pixels];
    float* blue = &rgb[2 * num_pixels];

    float sector_val;
    for (int i = 0; i < num_pixels; i++)
    {
        sector_val = intensity[i] * (1.0 - sat[i]);

        // RG Sector
        if (hue[i] < 120)
        {
            blue[i] = sector_val;
            red[i] = intensity[i] * (1.0 + sat[i] * cos(hue[i])) / cos(60.0 - hue[i]);
            green[i] = 1 - (blue[i] + red[i]);
        }

        // GB Sector
        else if (hue[i] >= 120 && hue[i] < 240)
        {
            red[i] = sector_val;
            green[i] = intensity[i] * (1.0 + sat[i] * cos(hue[i] - 120.0)) / cos(180.0 - hue[i]);
            blue[i] = 1 - (blue[i] + red[i]);
        }

        // RB Sector
        else
        {
            green[i] = sector_val;
            blue[i] = intensity[i] * (1.0 + sat[i] * cos(hue[i] - 240.0)) / cos(300.0 - hue[i]);
            red[i] = 1 - (blue[i] + red[i]);
        }
    }

    return rgb;
}

/**
* Calculates the hues of all pixels of a given RGB image. Assumes that rgb is a flattened RGB representation of an image
* with all R values first, then G, and B. Assumes that hsv is a flattened HSI representation of the image with
* all H values first, thene S, then V.
*
* rgb = [R1 R2 R3 ... G1 G2 G3 ... B1 B2 B3]
* hsi = [H1 H2 H3 ... S1 S2 S3 ... I1 I2 I3]
* 
* @param:   rgb         The RGB image vector.
* @param:   hsi         The HSI image vector. 
* @param:   num_pixels  Number of pixels in the image
* 
* @return:              Modifies the hsi array
*/
void calcHue(float* rgb, float* hsi, const int num_pixels)
{
    float* red = rgb;
    float* green = &rgb[num_pixels];
    float* blue = &rgb[num_pixels * 2];

    float num, rden, theta;

    /*
        Calculate the hue of each pixel which is given by the formula:

                        (      (0.5*( (R - G) + (R - B) )         )
        theta = arccos  (  -----------------------------------    )
                        (  ((R - G)^2 + (R - B)(G - B) ))^0.5     )

                    |
                    { theta         (if B <= G)
        Hue =       { 360 - theta   (else)
                    |
   */
    for (int i = 0; i < num_pixels; i++)
    {
        num = 0.5 * ((red[i] - green[i]) + (red[i] - blue[i]));
        rden = Q_rsqrt(red[i] - green[i]) * (red[i] - green[i]) + (red[i] - blue[i]) * (green[i] - blue[i]));
        theta = acos(num * rden);

        hsi[i] = (blue[i] <= green[i]) ? theta : 360.0 - theta;
    }

    return;
}

/**
* Calculate the saturation of each pixel of an RGB image.
*
* rgb = [R1 R2 R3 ... G1 G2 G3 ... B1 B2 B3]
* hsi = [H1 H2 H3 ... S1 S2 S3 ... I1 I2 I3]
*
* @param:   rgb         RGB image array
* @param:   hsi         HSI image array
* @param:   num_pixels  Number of entries in the RGB and HSI array
*
* @return:              Modifies the HSI array with the proper saturation values
*/
void calcSaturation(float* rgb, float* hsi, const int num_pixels)
{
    float* saturation = &hsi[num_pixels];
    float* red = rgb;
    float* green = &rgb[num_pixels];
    float* blue = &rgb[num_pixels * 2];

    // Saturation = 1 - 3*  min(R,G,B) / (R+G+B)
    for (int i = 0; i < num_pixels; i++)
    {
        saturation[i] = 1.0 - getRGBMin(red[i], green[i], blue[i]) / getRGBAverage(red[i], green[i], blue[i]);
    }

    return;
}

/**
* Calculate the intensity of each pixel which is simply the average of its RGB values.
*
* rgb = [R1 R2 R3 ... G1 G2 G3 ... B1 B2 B3]
* hsi = [H1 H2 H3 ... S1 S2 S3 ... I1 I2 I3]
* 
* @param:   rgb         RGB image array
* @param:   hsi         HSI image array
* @param:   num_pixels  Number of entries in the RGB and HSI array
* 
* @return:              Modifies the HSI array with the proper intensity values
*/
void calcIntensity(float* rgb, float* hsi, const int num_pixels)
{
    float* intensity = &hsi[num_pixels * 2];
    float* red = rgb;
    float* green = &rgb[num_pixels];
    float* blue = &rgb[num_pixels * 2];

    // Intensity = (R+G+B) / 3
    for (int i = 0; i < num_pixels; i++)
    {
        intensity[i] = getRGBAverage(red[i], green[i], blue[i]);
    }

    return;
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
    return (red + green + blue) / 3.0;
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
    float min = red;

    if (min > blue)
        min = blue;

    if (min > green)
        min = green;

    return min;
}