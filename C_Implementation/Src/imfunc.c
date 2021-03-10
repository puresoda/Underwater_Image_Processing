#include "../Inc/imfunc.h"

/**
* Reciprocal Square Root Function
* Credit to Wikipedia: https://en.wikipedia.org/wiki/Fast_inverse_square_root
*
* @param   number      Input to the recipricol square root
*
* @return              1/sqrt(number) approximation
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

/**
* Calculate the average value of an image array.
*
* @param   image       The array to average over
* @param   num_pixels  The number of indices to average over
*
* @returns             Returns the average (sum(image) / num_pixels)
*/
float calcAverage(float* image, const int num_pixels)
{
    float avg = 0;
    for (int i = 0; i < num_pixels; i++)
        avg += image[i];

    return (avg / (float) num_pixels);
}

/**
* Applies gamma correction to an image and clips the value between [0,1].
* 
* @param   image       Array containing the RGB image to apply correction to
* @param   num_pixels  Number of pixels in the RGB image
* @param   gamma       Amount of gamma correction to apply. corr_img = img^(gamma)
* 
* @return              Creates a new array containing the gamma corrected image
*/
float* correctGamma(float* image, const int num_pixels, const float gamma)
{
    const int rgb_size = 3 * num_pixels;

    float* gamma_image = malloc(sizeof(float) * rgb_size);

    for (int i = 0; i < rgb_size; i++)
    {
        gamma_image[i] = (float) pow(image[i], gamma);

        // Confine resulting value to between 0 and 1
        gamma_image[i] = (gamma_image[i] < 0) ? 0 : gamma_image[i];
        gamma_image[i] = (gamma_image[i] > 1) ? 1 : gamma_image[i];
    }

    return gamma_image;
}

/**
 * Applies Gaussian blur using a 3 x 3 kernel with a standard deviation of 0.5
 * 
 * @param   image           The image to be blurred (must be 2D! to do RGB, apply this function 3 times on each channel)
 * @param	num_row		    Number of rows in the image
 * @param	num_col		    Number of columns in the image
 * 
 * @return                  Allocates new memory for the resulting blurred image
 */
float* applyGaussianBlur(float* image, const int num_row, const int num_col)
{
    // Convolve "image" with the blur matrix (3x3)
    float gaussian_filter[9] = {0.0113, 0.0838, 0.0113, 0.0838, 0.6193, 0.0838, 0.0113, 0.0838, 0.0113 };

    float* output = conv2D(image, gaussian_filter, num_row, num_col, 3);

    return output;
}

/**
 * Applies Gaussian blur using a 3 x 3 kernel with a standard deviation of 0.5 by reference
 * 
 * @param   image           The image to be blurred (must be 2D! to do RGB, apply this function 3 times on each channel)
 * @param   output          Memory to place the result to
 * @param	num_row		    Number of rows in the image
 * @param	num_col		    Number of columns in the image
 * 
 * @return                  Utilizes existing memory for the result
 */
void applyGaussianBlurRef(float* image, float* output, const int num_row, const int num_col)
{
    // Convolve "image" with the blur matrix (3x3)
    float gaussian_filter[9] = { 0.0113, 0.0838, 0.0113, 0.0838, 0.6193, 0.0838, 0.0113, 0.0838, 0.0113 };
    
    convHelper(image, gaussian_filter, output, num_row, num_col, 3);
    return;
}

/**
 * Applies Laplacian edge detection using a 3 x 3 kernel
 * 
 * @param   image           The input image (must be 2D! to do RGB, apply this function on the greyscale version)
 * @param	num_row		    Number of rows in the image
 * @param	num_col		    Number of columns in the image
 * 
 * @return                  Allocates new memory for the resulting filtered image
 */
float* applyLaplacian(float* image, const int num_row, const int num_col)
{
    // Convolve "image" with the laplacian matrix (3x3)
    float lap_filter[9] = {-1.0, -1.0, -1.0, -1.0, 8.0, -1.0, -1.0, -1.0, -1.0};

    float* output = conv2D(image, lap_filter, num_row, num_col, 3);

    return output;
}

/**
 * Calcualtes the squared Euclidian Distance between two points (x1, y1, z1) and (x2, y2, z2)
 * ie: (x1-x2)^2 + (y1-y2)^2 + (z1-z2)^2
 * 
 * @param   x1      Refer to the equation for the meaning
 * @param   y1      Refer to the equation for the meaning
 * @param   z1      Refer to the equation for the meaning
 * @param   x2      Refer to the equation for the meaning
 * @param   y2      Refer to the equation for the meaning
 * @param   z2      Refer to the equation for the meaning
 * 
 * @return          Returns the squared Euclidian Distance: (x1-x2)^2 + (y1-y2)^2 + (z1-z2)^2
 */
float calcNormSquare(const float x1, const float x2, const float y1, const float y2, const float z1, const float z2)
{
    return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) + (z1 - z2) * (z1 - z2);
}

/**
 * Handles reading an input bitmap image file and its attributes.
 * 
 * @param   file_name       File name of the bitmap representing the image (.txt)
 * 
 * @return                  Returns a structure with three memmbers: the number of rows, number of columns, pointer to the array representing the RGB image
 */
struct Image readImage(const char file_name[])
{
    FILE* image_file;
    char file_location[100] = "./";
    strcat_s(file_location, sizeof(file_location), file_name);

    // Declare the structure to store the dimensions of the image as well as the image itself
    struct Image im;
    int pixel_val = 0;
    
    // Test if the file can be opened
    errno_t err;
    if ((err = fopen_s(&image_file, file_location, "r")) != 0)
    {
        printf("File was not opened\n");
        im.num_col = -1;
        im.num_row = -1;
        im.rgb_image = NULL;
        return im;
    }

    // Read in the number of rows and columns
    fscanf_s(image_file, "%d", &pixel_val);
    im.num_row = pixel_val;

    fscanf_s(image_file, "%d", &pixel_val);
    im.num_col = pixel_val;

    // Allocate memory for the image
    const int num_rgb_pixels = im.num_col * im.num_col * 3;
    im.rgb_image = malloc(sizeof(float) * num_rgb_pixels);

    for (int i = 0; i < num_rgb_pixels; i++)
    {
        fscanf_s(image_file, "%d", &pixel_val);
        im.rgb_image[i] = (float)(pixel_val) / 255.0f;
    }

    fclose(image_file);
    printf("File was read successfully!\n");

    return im;
}

/**
 * Handles writing an array to a text file. The result will be "file_name" + "_corrected.txt"
 * 
 * @param   file_name       Base file name of the output text file
 * @param   image           RGB image to written
 * @param   num_row         Number of rows in the image
 * @param   num_col         Number of columns in the image
 * 
 * @return                  Returns 0 if successful, -1 otherwise.
 */
int writeImage(const char file_name[], float* image, const int num_row, const int num_col)
{
    // Build up the string containing the full filename
    FILE* image_file;
    char file_location[100] = "./";
    strcat_s(file_location, sizeof(file_location), file_name);
    strcat_s(file_location, sizeof(file_location), "_corrected.txt");

    // Check if we can write to the file
    errno_t err;
    if ((err = fopen_s(&image_file, file_location, "w")) != 0)
    {
        printf("File could be written to.\n");
        return -1;
    }

    // Write the number of rows and columns
    fprintf_s(image_file, "%d\n", num_row);
    fprintf_s(image_file, "%d\n", num_col);

    // Write the actual pixels
    const int num_rgb_pixels = num_col * num_row * 3;

    for (int i = 0; i < num_rgb_pixels; i++)
    {
        fprintf_s(image_file, "%.6f\n", image[i]);
    }

    fclose(image_file);
    printf("Image was written successfully!\n");

    return 0;
}

//struct Image readImageParallel(const char base_file_name[])
//{
//    char* prefixes[3] = { "red_" , "green_" , "blue_" };
//
//    FILE* image_files[3];
//    char* file_locations[3][100] = { "./", "./", "./" };
//
//    // Test out the read on the red image
//    strcat_s(file_locations[0], sizeof(file_locations[0]), prefixes[0]);
//    strcat_s(file_locations[0], sizeof(file_locations[0]), base_file_name);
//    strcat_s(file_locations[0], sizeof(file_locations[0]), ".txt");
//
//    // Allocate memory to store the RGB image
//    // Note that we will take the dimensions from the first file and check that they match the rest of the files
//    struct Image im;
//    int pixel_val = 0;
//
//    errno_t err;
//    if ((err = fopen_s(&image_files[0], file_locations[0], "r")) != 0)
//    {
//        printf("Red image file was not opened\n");
//        im.num_col = -1;
//        im.num_row = -1;
//        im.rgb_image = NULL;
//        return im;
//    }
//
//    // Read in the number of rows and columns
//    fscanf_s(image_files[0], "%d", &pixel_val);
//    im.num_row = pixel_val;
//
//    fscanf_s(image_files[0], "%d", &pixel_val);
//    im.num_col = pixel_val;
//
//    fclose(image_files[0]);
//
//    // Allocate memory for the image
//    const int num_pixels = im.num_col * im.num_col;
//    const int num_rgb_pixels = num_pixels * 3;
//    im.rgb_image = malloc(sizeof(float) * num_rgb_pixels);
//
//    // Construct the threads and arguments
//    pthread_t thread_id[READ_THREADS];
//    struct arg_struct args[3];
//
//    for (int i = 0; i < READ_THREADS; i++)
//    {
//        // Open the file for each thread before calling the thread
//        strcat_s(file_locations[i], sizeof(file_locations[i]), prefixes[i]);
//        strcat_s(file_locations[i], sizeof(file_locations[i]), base_file_name);
//        strcat_s(file_locations[i], sizeof(file_locations[i]), ".txt");
//
//        if ((err = fopen_s(&image_files[i], file_locations[i], "r")) != 0)
//        {
//            printf("File %d was not opened\n", i);
//            return im;
//        }
//
//        // Create the argument struct
//        args[i].check_num_col = im.num_col;
//        args[i].check_num_row = im.num_row;
//        args[i].id = i;
//        args[i].image_file = &image_files[i];
//        args[i].output = &im.rgb_image[i * num_pixels];
//
//        // Create the thread for the R, G, and B images
//        pthread_create(&thread_id[i], NULL, &readSingleChannel, (void*)&args[i]);
//    }
//
//    // Wait for the threads to finish
//    for (int i = 0; i < READ_THREADS; i++)
//        pthread_join(thread_id[i], NULL);
//
//    return im;
//}
//
//void readSingleChannel(void* vargs)
//{
//    // Cast void argument into the correct structure
//    struct arg_struct* args = (struct arg_struct*)vargs;
//
//    // Declare the structure to store the dimensions of the image as well as the image itself
//    int pixel_val = 0;
//
//    // Read in the number of rows and columns
//    fscanf_s(args->image_file, "%d", &pixel_val);
//    const int num_row = pixel_val;
//
//    fscanf_s(args->image_file, "%d", &pixel_val);
//    const int num_col = pixel_val;
//    
//    if (num_row != args->check_num_row || num_col != args->check_num_col)
//    {
//        printf("Image dimensions of file %d did not match between input text files! Please check and try again.\n", args->id);
//        return;
//    }
//
//    const int num_pixels = num_row * num_col;
//
//    for (int i = 0; i < num_pixels; i++)
//    {
//        fscanf_s(args->image_file, "%d", &pixel_val);
//        args->output[i] = (float)(pixel_val) / 255.0f;
//    }
//
//    fclose(args->image_file);
//    printf("File %d was read successfully!\n", args->id);
//
//    return;
//}