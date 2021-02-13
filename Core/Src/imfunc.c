#include "imfunc.h"
#include <stdint.h>

/*
    We receive image as a matrix of pixels. Each pixel has three inputs, corresponding to RGB channel
    As such a pixel is identified as [R, G, B]
    "image" stores image as [R1, R2, R3, ..., G1, G2, G3, ..., B1, B2, B3...]
    We note that we require all RGB values to be normalized on the interval [0,1]
*/
void applyWhiteBalance(float* image, const int x_dim, const int y_dim, const float alpha)
{
    const int num_pixels = x_dim * y_dim;

    float *red = image;
    float *green = &image[num_pixels];
    float *blue = &image[num_pixels * 2];

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

/*
    Calculates the average of the "num_pixels" elements of an array.
    To get the average of a specific channel, adjust the pointer.
*/
float calcAverage(float* image, const int num_pixels)
{
    float avg = 0;
    for (int i = 0; i < num_pixels; i++)
        avg += image[i];

    return (avg / num_pixels);
}

void correctGama(double* array, const int start, const int end)
{
    int max = findMax(start, end);

    for (int i = 0; i < end; i++)
    {
        array[i + start] = pow((array[i + start] / max), 0.7);

        // Confine resulting value to between 0 and 1
        if (array[i + start] < 0)
        {
            array[i + start] = 0;
        }

        else if (array[i + start] > 1)
        {
            array[i + end] = 1;
        }
    }

    return;
}

int findMax(float* arr, const int start, const int end)
{
    int max = arr[start];

    for (int i = 0; i < end; i++)
    {
        max = (arr[i + start] > max ? arr[i + start] : max);
    }

    return max;
}

/*
    Converts a given RGB Image to the Hue Saturation Intensity Space.
    Assumes that rgb is a flattened RGB representation of an imagewith all R values first, then G, and B. 
    Assumes that hsv is a flattened HSI representation of the image with all H values first, thene S, then V.

    rgb = [R1 R2 R3 ... G1 G2 G3 ... B1 B2 B3]
    hsi = [H1 H2 H3 ... S1 S2 S3 ... I1 I2 I3]
*/
float* rgb2hsi(float* rgb_image, const int num_pixels)
{
    float* hsi = malloc(sizeof(float)*  num_pixels);

    calcHue(rgb_image, hsi, num_pixels);
    calcSaturation(rgb_image, hsi, num_pixels);
    calcIntensity(rgb_image, hsi, num_pixels);

    return hsi;
}

/*
    Conversion from HSI to RGB color space
*/
float* hsi2rgb(float* hsi, const int num_pixels)
{
    float* hue = hsi;
    float* sat = &hsi[num_pixels];
    float* intensity = &hsi[2*num_pixels];

    float* rgb = malloc(sizeof(float) * num_pixels);
    float* red = rgb;
    float* green = &rgb[num_pixels];
    float* blue = &rgb[2*num_pixels];

    float sector_val; 
    for (int i = 0; i < num_pixels; i++)
    {
        sector_val = intensity[i] * (1.0 - sat[i]);

        // RG Sector
        if(hue[i] < 120)
        {
            blue[i] = sector_val;
            red[i] = intensity[i] * (1.0 + sat[i]*cos(hue[i]))/cos(60.0-hue[i]);
            green[i] = 1 - (blue[i] + red[i]);
        }

        // GB Sector
        else if (hue[i] >= 120 && hue[i] < 240)
        {
            red[i] = sector_val;
            green[i] = intensity[i] * (1.0 + sat[i]*cos(hue[i] - 120.0))/cos(180.0-hue[i]);
            blue[i] = 1 - (blue[i] + red[i]);
        }

        // RB Sector
        else
        {
            green[i] = sector_val;
            blue[i] = intensity[i] * (1.0 + sat[i]*cos(hue[i] - 240.0))/cos(300.0 -hue[i]);
            red[i] = 1 - (blue[i] + red[i]);
        }
    }

    return hsi;
}

void hisogramEqualization(float* grey_image, const int num_pixels, const int num_bins)
{
    float* thresholds = malloc(sizeof(float)*  num_bins);
}

/*
    Calculates the hues of all pixels of a given RGB image. Assumes that rgb is a flattened RGB representation of an image
    with all R values first, then G, and B. Assumes that hsv is a flattened HSI representation of the image with
    all H values first, thene S, then V.

    rgb = [R1 R2 R3 ... G1 G2 G3 ... B1 B2 B3]
    hsi = [H1 H2 H3 ... S1 S2 S3 ... I1 I2 I3]

*/
void calcHue(float* rgb, float* hsi, const int num_pixels)
{
    float* red = rgb;
    float* green = &rgb[num_pixels];
    float* blue = &rgb[num_pixels*  2];

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
        rden = Q_rsqrt(red[i] - green[i])*(red[i] - green[i]) + (red[i] - blue[i])*(green[i] - blue[i]));
        theta = acos(num*  rden);

        hsi[i] = blue[i] <= green[i] ? theta : 360 - theta;
    }

    return;
}

/*
    Calculate the saturation of each pixel. Assumes that rgb is a flattened RGB representation of an image
    with all R values first, then G, and B. Assumes that hsv is a flattened HSI representation of the image with
    all H values first, thene S, then V.

    rgb = [R1 R2 R3 ... G1 G2 G3 ... B1 B2 B3]
    hsi = [H1 H2 H3 ... S1 S2 S3 ... I1 I2 I3]
*/
void calcSaturation(float* rgb, float* hsi, const int num_pixels)
{
    float* red = rgb;
    float* green = &rgb[num_pixels];
    float* blue = &rgb[num_pixels * 2];

    float min;

    // Saturation = 1 - 3*  min(R,G,B) / (R+G+B)
    for (int i = 0; i < num_pixels; i++)
    {
        hsi[i + num_pixels] = 1.0 - getRGBMin(red[i], green[i], blue[i]) / getRGBAverage(red[i], green[i], blue[i]);
    }

    return;
}

/*
    Calculate the intensity of each pixel. Assumes that rgb is a flattened RGB representation of an image
    with all R values first, then G, and B. Assumes that hsv is a flattened HSI representation of the image with
    all H values first, thene S, then V.

    rgb = [R1 R2 R3 ... G1 G2 G3 ... B1 B2 B3]
    hsi = [H1 H2 H3 ... S1 S2 S3 ... I1 I2 I3]
*/
void calcIntensity(float* rgb, float* hsi, const int num_pixels)
{
    // Intensity = (R+G+B) / 3
    for (int i = 0; i < num_pixels; i++)
    {
        getRGBAverage(red[i], green[i], blue[i]);
    }

    return;
}

/*
    Simple function to calculate the  minimum value of an RGB Pair:
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

/*
    Simple function to get the average of an RGB Pair
*/
float getRGBAverage(const float red, const float green, const float blue)
{
    return (red + green + blue) / 3.0;
}

/*
    Reciprocal Square Root Function
    Credit to Wikipedia: https://en.wikipedia.org/wiki/Fast_inverse_square_root
*/
float Q_rsqrt(float number)
{
    const float x2 = number * 0.5F;
    const float threehalfs = 1.5F;

    union
    {
        float f;
        uint32_t i;
    } conv = {.f = number};
    conv.i = 0x5f3759df - (conv.i >> 1);
    conv.f *= threehalfs - (x2 * conv.f * conv.f);
    return conv.f;
}

void Weights(int luminance_option){
	float* wk = malloc(num_pixels * sizeof(float));
	
	// Get individual weights
	for(int i = 0; i < num_pixels; i++){
		GetWeights(i, wk[i], luminance_option);
	}
	
	// Normalize the weights
	float normalization_factor = 0.0;
	
	for(int i = 0; i < num_pixels; i++){
		float index = (float) i;
		normalization_factor += wk[i] + index*0.1;
	}
	
	for(int i = 0; i < num_pixels; i++){
		wk_normalized[i] = (wk[i] + 0.1)/normalization_factor;
	}
	
	free(wk);
	
	return;
}


void GetWeights(int index, float wk, int luminance_option){
	float luminance = CalculateLuminance(index, num_pixels, luminance_option);
	
	// Obtain wl
	float wl = LaplacianContrastWeight(luminance);
	
	// Obtain ws
	float ws = SaliencyWeight();
	
	// Obtain wsat
	float wsat = SaturationWeight(index, luminance);
	
	wk = wl + ws + wsat;
}

// Laplacian Contrast Weight
float LaplacianContrastWeight(int luminance){
	float wl = 0.0;
	float lap_filter[9] = {0.0, -1.0, 0.0, -1.0, 4.0, -1.0, 0.0, -1.0, 0.0};
	
	return wl;
}

// Saliency Weight
float SaliencyWeight(int index){
	sw = (array[index] - avg_R)*(array[index] - avg_R) + (array[index+num_pixels] - avg_G)*(array[index+num_pixels] - avg_G) + (array[index+2*num_pixels] - avg_B)*(array[index+2*num_pixels] - avg_B)/
	return sw;
}

// Saturation Weight
float SaturationWeight(int index, float luminance){
	wsat = (1.0/3.0) * ((array[index] - luminance)*(array[index] - luminance) + (array[index+num_pixels] - luminance)*(array[index+num_pixels] - luminance) + (array[index+2*num_pixels] - luminance)*(array[index+2*num_pixels] - luminance));
	return wsat;
}

// Calculating Luminance
float CalculateLuminance(int index, int num_pixels, int luminance_option){
	float luminance = 0;
	switch(luminance_option)
	{
		case 0:
		luminance = 0.2126*array[index] + 0.7152*array[index+num_pixels] + 0.0722*array[index+2*num_pixels];
		break;
		case 2:
		luminance = 0.299*array[index]*array[index] + 0.587*array[index+num_pixels]*array[index+num_pixels] + 0.114*array[index+2*num_pixels]*array[index+2*num_pixels];
		break;
		case 1:
		default:
		luminance = 0.299*array[index] + 0.587*array[index+num_pixels] + 0.114*array[index+2*num_pixels];
		break;
	}
	
	return luminance;
}


ConvertRGBtoLAB(){
	for(int i = 0; i < num_pixels; i++){
		LABConversionFunction(i);
	}
}

void LABConversionFunction(int i){
	
	float xn = 76.04;
	float yn = 80;
	float zn = 87.12;
	
	// RGB to XYZ conversion
	xyz_image[i] = 0.412453*array[i] + 0.357580*array[i+num_pixels] + 0.180423*array[i+2*num_pixels];
	xyz_image[i+num_pixels] = 0.212671*array[i] + 0.715160*array[i+num_pixels] + 0.072169*array[i+2*num_pixels];
	xyz_image[i+2*num_pixels] - 0.019334*array[i] + 0.119193*array[i+num_pixels] + 0.950227*array[i+2*num_pixels];
	
	// XYZ to LAB conversion
	if(xyz_image[i+num_pixels]/yn > 0.00856){
		lab_image[i] = 116*pow((xyz_image[i+num_pixels]/yn), (1/3)) - 16;
	}
	else{
		lab_image[i] = 903.3*(xyz_image[i+num_pixels]/yn);
	}
	
	lab_image[i+num_pixels] = 500*f(xyz_image[i], xn) - f(xyz_image[i+num_pixels], yn);
	
	lab_image[i+2*num_pixels] = 200*f(xyz_image[i+num_pixels], yn) - f(xyz_image[i+2*num_pixels], zn);
}

float f(float a, float b){
	if((a/b) > 0.00856){
		return (pow((a/b), (1/3)));
	}
	else{
		return (7.787*(a/b) + 16.0/116.0);
}
