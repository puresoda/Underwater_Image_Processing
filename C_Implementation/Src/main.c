#include "../Inc/imfusion.h"
#include <stdio.h>

int main()
{
	float* result = imageFusionSeqFull("03_bitmap.txt");
	free(result);

	return 0;
}