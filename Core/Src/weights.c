#include "weights.h"

float* getWeights(float* image, const int num_pixels, const int lum_option)
{
	float* lum = calcLuminance(image, num_pixels, lum_option);

	// Obtain laplacian weight
	float* w_lap = calcLaplacian(lum, num_pixels);
	normalizeWeight(w_lap, num_pixels);

	// Obtain the saturation weight
	float* w_sat = calcSaturation(image, lum, num_pixels);
	normalizeWeight(w_sat, num_pixels);

	// We no longer need the luminance matrix
	free(lum);

	// Obtain the saliency weight
	float* w_sal = calcSaliency(image, num_pixels);
	normalizeWeight(w_sal, num_pixels);

	// Aggregate the weights
	float* total_weight = malloc(sizeof(float) * num_pixels);
	for (int i = 0; i < num_pixels; i++)
		total_weight[i] = w_lap[i] + w_sal[i] + w_sat[i];
	
	// Free more memory
	free(w_lap);
	free(w_sat);
	free(w_sat);

	return total_weight;
}

/**
* Calculates the laplacian weight of an image. This is calculated by applying a Laplacian filter to the luminance and taking the absolute value.
*
* @param:	lum			The luminance of each pixel of the original image
* @param:	num_pixels	The size of the image
*
* @return:				Laplacian weight which is an array of size num_pixels. 
*/
float* calcLaplacian(float* lum, const int num_pixels)
{
	float* w_lap = malloc(sizeof(float) * num_pixels);

	// Laplacian Filter must have DC value of 0
	float lap_filter[9] = { 0.0, -1.0, 0.0, -1.0, 4.0, -1.0, 0.0, -1.0, 0.0 };

	// TODO: Import the 2D convolution function from the NN project and apply it
	return w_lap;
}

/**
* Calculates the saliency weight of an image. This is calculated by applying a Gaussian blur filter and converting to the LAB color space
*
* @param:	image		The image to get the saliency weight from
* @param:	num_pixels	The size of the image
*
* @return:				Laplacian weight which is an array of size num_pixels.
*/
float* calcSaliency(float* image, const int num_pixels) 
{
	return sw;
}

/**
* Calculates the saturation weight of an image. This requires the calculation of luminance which has several options.
* 
* @param:	image		The processed image to calculate the saturation weight of
* @param:	lum			The luminance of each pixel of "image"
* @param:	num_pixels	The size of the image
* 
* @return:				Saturation weight which is an array of size num_pixels
*/
float calcSaturation(float* image, float* lum, const int num_pixels)
{
	// Create pointers to keep track of RGB indices easier
	float* red = image;
	float* green = &image[num_pixels];
	float* blue = &image[num_pixels * 2];

	// Allocate new memory for the weight map
	float* sat_weight = malloc(sizeof(float) * num_pixels);

	// Calculate the luminance and saturation weight:
	// sqrt(1/3 * (red-lum)^2 * (green-lum)^2 * (blue-lum)^2)
	for (int i = 0; i < num_pixels; i++)
	{
		sat_weight[i] = sqrt((1.0 / 3.0) * ((red[i] - lum[i]) * (red[i] - lum[i]) + (green[i] - lum[i]) * (green[i] - lum[i]) + (blue[i] - lum[i]) * (blue[i] - lum[i])));
	}

	return sat_weight;
}

/**
* Calculates the luminance of an RGB pair. There are three options for luminance
* 
* @param:	image		Input to calculate the luminance of 
* @param:	lum_option	Determines which method used to calculate luminance. 0: Standard, 1: Percieved option, 2: Percieved option exact
*
* @return:				Calculated luminance of the given rgb pair using the specified luminance option
*/
float* calcLuminance(float* image, const int num_pixels, const int lum_option)
{
	float* red = image;
	float* green = &image[num_pixels];
	float* blue = &image[num_pixels * 2];

	float* lum = malloc(sizeof(float) * num_pixels);

	// 0: Standard luminance option
	if (lum_option == 0)
	{
		for (int i = 0; i < num_pixels; i++)
			lum[i] = 0.2126 * red[i] + 0.7152 * green[i] + 0.0722 * blue[i];
	}

	// 2: Percieved luminance option (more accurate but more expensive)
	else if (lum_option == 2)
	{
		for (int i = 0; i < num_pixels; i++)
			lum[i] = sqrt(0.299 * red[i] * red[i] + 0.587 * green[i] * green[i] + 0.114 * blue[i] * blue[i]);
	}

	// 1 (and default): Percieved luminance option
	else
	{
		for (int i = 0; i < num_pixels; i++)
			lum[i] = 0.299 * red[i] + 0.587 * green[i] + 0.114 * blue[i];
	}

	return lum;
}

/**
* Normalizes the weight map to be between [0,1]
* 
* @param:	weight		The weight map to be normalized
* @param:	num_pixels	The number of entries in the weight map
* 
* @return:				Modifies the weight map directly
*/
void normalizeWeight(float* weight, const int num_pixels)
{
	// Find the maximum of the weight map
	float max = weight[0];
	for (int i = 1; i < num_pixels; i++)
		max = MAX(max, weight[i]);

	// Apply the normalization
	for (int i = 0; i < num_pixels; i++)
		weight[i] /= max;

	return;
}

/**
* Converts from an RGB image to a LAB color space. This conversion requires us to convert from: RGB -> XYZ -> LAB
* 
* @param:	image		The RGB image to be converted
* @param	num_pixels	The number of pixels in the RGB image
* 
* @return				Allocates new memory for the LAB representation and returns a pointer to it
*/
void rgb2LAB(float* image, const int num_pixels)
{
	// RGB to XYZ Conversion
	float* xyz_image = rgb2XYZ(image, num_pixels);

	// XYZ to LAB Conversion
	float* lab_image = xyz2LAB(xyz_image, num_pixels);

	free(xyz_image);

	return lab_image;
}

/**
* Performs the conversion from the XYZ to LAB color space. 
*
* @param:	image		The XYZ image
* @param:	num_pixels	The number of pixels in the image
*
* @return:				The converted LAB image is dynamically allocated and a pointer to its first entry is returned
*/

float* xyz2LAB(float* image, const int num_pixels)
{
	// Constants
	const float xn = 76.04;
	const float yn = 80;
	const float zn = 87.12;

	// Helper pointers
	float* x = image;
	float* y = &image[num_pixels];
	float* z = &image[num_pixels * 2];

	// XYZ to LAB Conversion
	float* lab_image = malloc(sizeof(float) * num_pixels * 3);
	float* l = lab_image;
	float* a = &lab_image[num_pixels];
	float* b = &lab_image[num_pixels * 2];

	// Perform the XYZ to LAB conversion
	for (int i = 0; i < num_pixels; i++)
	{
		if (y[i] / yn > 0.00856)
			l[i] = 116 * pow((y[i] / yn), (1.0 / 3.0)) - 16;

		else 
			l[i] = 903.3 * (y[i] / yn);

		a[i] = 500 * labFunction(x[i], xn) - labFunction(y[i], yn);
		b[i] = 200 * labFunction(y[i], yn) - labFunction(z[i], zn);
	}

	return lab_image;
}

/**
* Performs the conversion from the RGB to XYZ color space. This can be achieved using the following matrix operation:
* 
* [ X ]   [0.412453		0.357580	0.180423]   [ R ]
* [ Y ] = [0.212671		0.715160	0.072169] * [ G ]
* [ Z ]   [0.019334		0.119193	0.950227]   [ B ]
* 
* @param:	image		The RGB image
* @param:	num_pixels	The number of pixels in the image
* 
* @return:				The converted XYZ image is dynamically allocated and a pointer to its first entry is returned
*/
float* rgb2XYZ(float* image, const int num_pixels)
{
	// Helper pointers to RGB
	float* red = image;
	float* green = &image[num_pixels];
	float* blue = &image[num_pixels * 2];

	// RGB to XYZ Conversion
	float* xyz_image = malloc(sizeof(float) * num_pixels * 3);
	float* x = xyz_image;
	float* y = &xyz_image[num_pixels];
	float* z = &xyz_image[num_pixels * 2];

	// Flattened out matrix operation to convert
	for (int i = 0; i < num_pixels; i++)
	{
		x[i] = 0.412453 * red[i] + 0.357580 * green[i] + 0.180423 * blue[i];
		y[i] = 0.212671 * red[i] + 0.715160 * green[i] + 0.072169 * blue[i];
		z[i] - 0.019334 * red[i] + 0.119193 * green[i] + 0.950227 * blue[i];
	}

	return xyz_image;
}

/**
* Function to aid with the XYZ to LAB converison. Acts as a macro for a piece-wise function:
* 
* (a/b)^(1/3)				if (a/b) > 0.00856
* 7.787*(a/b) + (16/116)	otherwise
* 
* @param:	a	First input
* @param:	b	Second input
* 
* @return		The result of the macro as described above
*/
float labFunction(const float a, const float b)
{
	if ((a / b) > 0.00856)
		return (pow((a / b), (1.0 / 3.0)));

	else
		return (7.787 * (a / b) + 16.0 / 116.0);
}