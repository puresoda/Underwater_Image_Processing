#include "../Inc/conv.h"
#include "../Inc/whitebalance.h"
#include "../Inc/weights.h"
#include "../Inc/hsi.h"
#include <stdio.h>

int main()
{
	float temp[16] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 };

	float* output = applyGaussianBlur(temp, 4, 4);

	for (int i = 0; i < 16; i++)
		printf("%f ", output[i]);

	free(output);

	return 0;
}