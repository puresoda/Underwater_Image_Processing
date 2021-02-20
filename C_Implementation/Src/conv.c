#include "../Inc/conv.h"

/**
* Create a matrix with zero padding around the edges. We allow for the input to be a non-square array but the filter must be square.
* 
* @param   input           The input array that is flattened by column-row order
* @param   pad_mat         Array in which the padded matrix is stored. This is of size input_num_row + 2 * floor((filter_size - 1) / 2) x input_num_col + 2 * floor((filter_size - 1) / 2)
* @param   input_num_row   The number of rows in the input matrix
* @param   input_num_col   The number of columns in the input matrix
* @param   filter_size     The number of rows and columns of the filter. The filter must be square
* 
* @return                  Modifies pad_mat with the padded version of input
*/
void padMatrix(float* input, float* pad_mat, const int input_num_row, const int input_num_col, const int filter_size)
{
    // Assuming square inputs, num_pad is the number of padding on each side of the matrix
    const int pad_add_row = (filter_size - 1) / 2;
    const int pad_add_col = (filter_size - 1) / 2;

    const int output_num_row = input_num_row + 2 * pad_add_row;
    const int output_num_col = input_num_col + 2 * pad_add_col;

    const int output_offset = output_num_row * output_num_col;
    const int input_offset = input_num_row * input_num_col;

    // Populate the padded matrix
    for (int j = 0; j < output_num_row; j++)
    {
        for (int i = 0; i < output_num_col; i++)
        {
            // Check for padding
            if (i < pad_add_col || i >= input_num_col + pad_add_col || j < pad_add_row || j >= input_num_row + pad_add_row)
                pad_mat[j * output_num_col + i] = 0;

            else
                pad_mat[j * output_num_col + i] = input[(j - pad_add_row) * input_num_col + (i - pad_add_col)];
        }
    }

    return;
}

/*
 * This function handles the convolution operation for a single filter
 */
void convHelper(float* input, float* filter, float* output, const int input_num_row, const int input_num_col, const int filter_size)
{
    // Create padded matrix
    const int pad_num_row = input_num_row + (filter_size - 1);
    const int pad_num_col = input_num_col + (filter_size - 1);
    const int pad_offset = pad_num_row * pad_num_col;

    const int filter_offset = filter_size * filter_size;

    // Allocate memory for the new padded matrix
    float* pad_mat = malloc(sizeof(float) * pad_offset);
    padMatrix(input, pad_mat, input_num_row, input_num_col, filter_size);

    float sum = 0;

    // Padding is 'same' so the output height and width is the same as the input's
    // Loop through each convolution operation
    for (int row = 0; row < input_num_row; row++)
    {
        for (int col = 0; col < input_num_col; col++)
        {
            sum = 0;

            // Add up the result of a single convolution
            for (int i = 0; i < filter_offset; i++)
                sum += pad_mat[((row + i / filter_size) * pad_num_col) + col + (i % filter_size)] * filter[i];

            output[col + row * input_num_col] = sum;
        }
    }

    free(pad_mat);
    return;
}

/*
 * Performs a 2D convolution given an input and filter
 */
float* conv2D(float* input, float* filter, const int input_num_row, const int input_num_col, const int filter_size)
{
    float* output = malloc(sizeof(float) * input_num_col * input_num_row);
    convHelper(input, filter, output, input_num_row, input_num_col, filter_size);

    return output;
}