#include "../Inc/imfusion.h"
#include <stdio.h>

int main()
{

	float* result = imageFusionFull("underwater_bitmap.txt");
	free(result);
	return 0;
}