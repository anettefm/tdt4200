#include <stdio.h>
#include <cutil.h>


__global__ void invert(unsigned char* image_h, unsigned char* image_d ){

	int id=blockIdx.x*blockDim.x+threadIdx.x;
	image_d[id]=~image_h[id];
}

