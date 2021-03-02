#include "../Inc/whitebalance.h"

/**
* Applies white balance on an image to compensate for the attenuation of the red and blue channels underwater
*
* @param   image       RGB image normalized on the interval [0,1]. The image is stored as [R1 R2 R3 ..., G1 G2 G3 ..., B1 B2 B3 ...]
* @param   num_row     Number of rows in the image
* @param   num_col     Number of columns in the image
* @param   alpha       Multiplicative factor to control the amount of compensation (default should be 1)
*
* @return              Modifies the original image with the correct white balance
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

    // Apply Grey World and swap out the memory from the original image
    applyGreyWorldFull(image, num_pixels, 20);

    return;
}

/**
* Implements an approximation of the Grey World Approximation. The full Grey World approximation will be implemented later.
*/
void applyGreyWorld(float* image, const int num_pixels)
{
    float scale_factor = 0;

    for (int i = 0; i < NUM_CHANNELS; i++)
    {
        scale_factor = calcAverage(&image[i*num_pixels], num_pixels);

        for (int j = 0; j < num_pixels; j++)
            image[i * num_pixels + j] *=  (127.5f / scale_factor / 255.0f);
    }

    return;
}

/**
* Linearizes RGB image to remove the gamma correction present in SRGB images.
* 
* @param   image       The RGB image to linearize
* @param   num_pixels  The number of pixels in the image
* 
* @return              Linearized version of the input image
*/
void linearizeRGB(float* image, const int num_pixels)
{
    const int num_rgb = num_pixels * NUM_CHANNELS;

    for (int i = 0; i < num_rgb; i++)
        image[i] = linearizerHelper(image[i]);

    return;
}

/**
* Performs linearization on a single pixel according to the following equation:
* 
*         | -f(-u)                  if u < 0
* f(u) =  {  u * c                  if 0 < u < d
*         |  (u*a + b)^(lambda)     if u >= d
* 
* @param   pixel   Pixel to be linearized
* 
* @return          Linearized pixel
*/
float linearizerHelper(const float pixel)
{
    // Constants for linearization
    const float a = 1.0f / 1.055f;
    const float b = 0.055f / 1.055f;
    const float c = 1.0f / 12.92f;
    const float d = 0.04045f;
    const float lambda = 2.4f;

    if (pixel < 0)
        return -1.0f*linearizerHelper(ABS(pixel));

    else if (pixel >= 0 && pixel < d)
        return (pixel * c);

    else
        return (float) pow(pixel * a + b, lambda);
}

void applyGreyWorldFull(float* image, const int num_pixels, const int percentile)
{
    // Reference XYZ White Trismus Values for D65 Illuminant
    const float TARGET_WHITE[3] = { 0.95047,	1.00000,	1.08883 };

    // Constant Bradford matrices used to calculate the final transformation matrices
    const float bradford[9] = {
        0.8951000, 0.266400, -0.161400,
        -0.750200, 1.7135000, 0.0367000,
        0.0389000, -0.0685000, 1.0296000 };

    const float bradford_inv[9] = {
        0.9869929, -0.1470543, 0.1599627,
        0.4323053, 0.5183603, 0.0492912,
        -0.0085287, 0.0400428, 0.9684867 };

    // Convert the image to Linear RGB, then to XYZ
    linearizeRGB(image, num_pixels);
    float* xyz_image = rgb2XYZ(image, num_pixels);
    float* x = &xyz_image[0];
    float* y = &xyz_image[num_pixels];
    float* z = &xyz_image[num_pixels * 2];

    // Calculate the illuminant of the linearized RGB image
    float* illuminants = calcIlluminantRGB(image, num_pixels, percentile);
    
    // Calculate the cone values ie: bradford(3x3) * (x, y, z)^T
    float* source_cone = multiplyFlatMatrix(bradford, illuminants, NUM_CHANNELS, NUM_CHANNELS, NUM_CHANNELS, 1);
    float* target_cone = multiplyFlatMatrix(bradford, TARGET_WHITE, NUM_CHANNELS, NUM_CHANNELS, NUM_CHANNELS, 1);

    /*
    Construct the middle diagonal matrix:
    
     source_cone[0] / target_cone[0]              0                                       0
                  0                   source_cone[1] / target_cone[1]                     0
                  0                               0                           source_cone[2] / target_cone[2]
    */
    float* diag = calloc(NUM_CHANNELS * NUM_CHANNELS, sizeof(float));
    for (int i = 0; i < NUM_CHANNELS; i++)
        diag[4 * i] = source_cone[i] / target_cone[i];
    
    // Free the cone data
    free(source_cone);
    free(target_cone);

    // Get the entire transofrmation matrix
    float* intermediate = multiplyFlatMatrix(bradford_inv, diag, NUM_CHANNELS, NUM_CHANNELS, NUM_CHANNELS, NUM_CHANNELS);
    free(diag);
    float* transformation = multiplyFlatMatrix(intermediate, bradford, NUM_CHANNELS, NUM_CHANNELS, NUM_CHANNELS, NUM_CHANNELS);
    free(intermediate);

    // Apply the transformation to each XYZ pair
    float xyz_pair[NUM_CHANNELS];
    float trans_xyz[NUM_CHANNELS];

    for (int i = 0; i < num_pixels; i++)
    {
        // Load in the XYZ pair
        xyz_pair[0] = x[i];
        xyz_pair[1] = y[i];
        xyz_pair[2] = z[i];

        // Reset the output
        trans_xyz[0] = trans_xyz[1] = trans_xyz[2] = 0;

        multiplyFlatMatrixRef(transformation, xyz_pair, trans_xyz, NUM_CHANNELS, NUM_CHANNELS, NUM_CHANNELS, 1);

        // Load the result back into the original XYZ matrix
        // There's definitely a better way to do this
        x[i] = trans_xyz[0];
        y[i] = trans_xyz[1];
        z[i] = trans_xyz[2];
    }

    free(transformation);
    free(image);

    image = xyz2rgb(xyz_image, num_pixels);
    free(xyz_image);
}

float calcIlluminant(float* image, const int num_pixels, const int percentile)
{
    // Create the historgram of RGB values
    int histogram[256] = { 0 };
    int cum_sum_forward[256] = { 0 };
    int cum_sum_backward[256] = { 0 };
    int idx_high = -1;
    int idx_low = -1;

    // Loop thorugh all pixel values, note that we multiply by 255 to get an integer representaton
    for (int i = 0; i < num_pixels; i++)
        histogram[(int)(image[i] * 255) % 256]++;

    // Thresholds to determine the mask
    float low_threshold = num_pixels * percentile / 100.0f;
    float high_threshold = num_pixels * (100.0f - percentile) / 100.0f;

    // Initialize the values of the indexes that satisfy the thershold requirement
    if (cum_sum_forward[0] > low_threshold)
        idx_low = 0;

    if (cum_sum_backward[0] > high_threshold)
        idx_high = 0;

    // Get the cumulative sum going forward and backwards
    cum_sum_forward[0] = histogram[0];
    cum_sum_backward[0] = histogram[255];

    // Loop through last 255 elements of the histogram
    for (int i = 1; i < 256; i++)
    {
        // Calculate the cumulative sums and update the indexes
        cum_sum_forward[i] = cum_sum_forward[i - 1] + histogram[i];
        cum_sum_backward[i] = cum_sum_backward[i - 1] + histogram[255 - i];

        if (idx_low == -1 && cum_sum_forward[i] > low_threshold)
            idx_low = i;

        if (idx_high == -1 && cum_sum_backward[i] > high_threshold)
            idx_high = (255-i);
    }

    // Get the L1 norm of the pixels within our new range
    float eps = 7;
    float sum = 0;
    int count = 0;

    for (int i = 0; i < num_pixels; i++)
    {
        if ((image[i] * 255.0f) <= idx_high + eps && (image[i] * 255.0f) >= idx_low - eps)
        {
            sum += ABS(image[i]);
            count++;
        }
    }

    if (count == 0)
        return 0;
    else
        return (sum / count);
}

float* calcIlluminantRGB(float* image, const int num_pixels, const int percentile)
{
    static float illuminants[NUM_CHANNELS] = { 0 };

    for (int i = 0; i < NUM_CHANNELS; i++)
        illuminants[i] = calcIlluminant(&image[i * num_pixels], num_pixels, percentile);

    return illuminants;
}

float* multiplyFlatMatrix(float* left_mat, float* right_mat, const int left_num_row, const int left_num_col, const int right_num_row, const int right_num_col)
{
    // Allocate memory for the output and perform the multiplication
    float* output = calloc(left_num_row * right_num_col, sizeof(float));

    if (multiplyFlatMatrixRef(left_mat, right_mat, output, left_num_row, left_num_col, right_num_row, right_num_col) != 0)
    {
        free(output);
        output = NULL;
    }

    return output;
}

int multiplyFlatMatrixRef(float* left_mat, float* right_mat, float* output, const int left_num_row, const int left_num_col, const int right_num_row, const int right_num_col)
{
    // Sanity check to make sure matrix multiplication is valid
    if (left_num_col != right_num_row)
    {
        printf("Dimension error in matrix multipliation! Please check the dimensions and try again.");
        return -1;
    }

    // Perform the matrix multiplication
    for (int i = 0; i < left_num_row; i++)
        for (int j = 0; j < left_num_col; j++)
            for (int k = 0; k < right_num_col; k++)
                output[i * right_num_col + k] += left_mat[i * left_num_col + j] * right_mat[j * right_num_col + k];

    return 0;
}

