#include "../Inc/imfusion.h"
#include <stdio.h>

int main()
{
	float* result = imageFusionSeqFull("underwater_bitmap.txt");
	free(result);

	return 0;
}