#include "../Inc/imsharp.h"

/**
* Applies the normalized unsharp masking process as defined below. I is the input RGB image with entries between [0,1].
* S is the sharpened image. G*I refers to convolution with a Gaussian Filter to perform blurring.
* 
* S = (I + N(I - G * I)) / 2
* 
* @param	image		The RGB input image with entries between [0,1]
* @param	num_row		Number of rows in the RGB image
* @param	num_col		Number of columns in the RGB image
* 
* @return				Returns a 3 * num_row * num_col entry array corresponding to the sharpened image.
*/
float* applyUnsharpMask(float* image, const int num_row, const int num_col)
{
	// Calculate some constants to be used in the algorithm
	const int num_pixels = num_row * num_col;
	const int num_channels = 3;
	const int num_rgb_pixels = num_pixels * num_channels;

	// Apply Gaussian Blur and subtract from the original image
	float* blurred = malloc(sizeof(float) * num_rgb_pixels);

	for (int i = 0; i < num_channels; i++)
		applyGaussianBlurRef(&image[num_pixels * i], &blurred[num_pixels * i], num_row, num_col);

	for (int i = 0; i < num_rgb_pixels; i++)
		blurred[i] = image[i] - blurred[i];

	// Convert to HSI to apply histogram equalization
	float* hsi_image = rgb2hsi(blurred, num_pixels);

	free(blurred);

	histogramEqualization(&hsi_image[2 * num_pixels], num_pixels);

	// sharpened = (image + normalized) / 2
	float* sharp = hsi2rgb(hsi_image, num_pixels);

	for (int i = 0; i < num_rgb_pixels; i++)
		sharp[i] = (image[i] + sharp[i]) / 2.0f;
	
	free(hsi_image);

	return sharp;
}

/**
* Applies histogram equalization on a mono-channel image. The goal of this algorithm is to redistribute pixel values such that the
* number of occurances of pixel values in each bin is approximately uniform.
* 
* @param	intensity		The monochromatic RGB image
* @param	num_pixels		Number of pixels in the RGB image
* @param	num_bins		The number of bins used in equalization, default should be 64
* 
* @return					Modifies the original intensity array
*/
void histogramEqualization(float* intensity, const int num_pixels)
{
	// Create the historgram of RGB values
	int* histogram = calloc(256, sizeof(int)) ;
	int* new_grey = malloc(sizeof(int) * 256);
	int cum_sum = 0;

	// Loop thorugh all pixel values, note that we multiply by 255 to get an integer representaton
	for (int i = 0; i < 256; i++)
		histogram[(int)(intensity[i] * 255) % 256]++;

	// Calculate the new grey values to assign
	// New grey value the cumulitive probability at that point normalized to [0,255]
	for (int i = 0; i < 256; i++)
	{
		cum_sum += histogram[i];
		new_grey[i] = round(((float)cum_sum) * 255.0f / num_pixels);
	}

	// Assign the new pixels
	for (int i = 0; i < num_pixels; i++)
		intensity[i] = (float) new_grey[(int)(intensity[i] * 255) % 256] / 255.0f;

	free(histogram);
	free(new_grey);
	return;
}