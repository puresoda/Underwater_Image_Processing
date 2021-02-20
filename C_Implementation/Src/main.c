#include "../Inc/conv.h"
#include "../Inc/whitebalance.h"
#include "../Inc/weights.h"
#include "../Inc/hsi.h"
#include <stdio.h>

int main()
{
	float temp[] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27 };

	for (int i = 0; i < 27; i++)
		temp[i] /= 27.0;

	float* output = correctGamma(temp, 9, 1.1);

	for (int i = 0; i < 27; i++)
		printf("%f ", output[i]);

	free(output);

	return 0;
}