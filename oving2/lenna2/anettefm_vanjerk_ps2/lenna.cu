#include <iostream>
#include "lodepng.h"
#include <stdio.h>
//#include <stdint.h> /* for uint64 definition */
//#include <time.h> /* for clock_gettime */

// #define BILLION 1000000000L


__global__ void invert( unsigned char* image_d ){

        int id=blockIdx.x*blockDim.x+threadIdx.x;
        image_d[id] = ~image_d[id];
}


int main( int argc, char ** argv){
// variabler til tidtaking
//	uint64_t diff;
//	struct timespec start_hele, start_minne, start_exe, end;

  size_t pngsize;
  unsigned char *png;
  const char * filename = "lenna512x512_inv.png";


 //       clock_gettime(CLOCK_MONOTONIC, &start_hele); /* mark start time */

  /* Read in the image */
  lodepng_load_file(&png, &pngsize, filename);

  unsigned char *image;
  unsigned int width, height;
  /* Decode it into a RGB 8-bit per channel vector */
  unsigned int error = lodepng_decode24(&image, &width, &height, png, pngsize);

  /* Check if read and decode of .png went well */
  if(error != 0){
      std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
  }

  // Do work
         unsigned char* image_d;

        size_t size;
        size=height*width*3*sizeof(char);

        cudaMalloc((void**) &image_d, size);

//	clock_gettime(CLOCK_MONOTONIC, &start_minne); /* mark start time */	
//        clock_gettime(CLOCK_MONOTONIC, &start_exe); /* mark start time */

	cudaMemcpy(image_d, image, size, cudaMemcpyHostToDevice);
	cudaDeviceSynchronize();	

//	clock_gettime(CLOCK_MONOTONIC, &end); /* mark the end time */
//        diff = BILLION * (end.tv_sec - start_minne.tv_sec) + end.tv_nsec - start_minne.tv_nsec; printf("minne: elapsed time = %llu nanoseconds\n", (long long unsigned int) diff);

	int threadBlock=1024;
	int gridBlock=3*512*512/threadBlock;

        invert<<<gridBlock, threadBlock>>>(image_d);

        cudaMemcpy(image, image_d, size, cudaMemcpyDeviceToHost);

//	clock_gettime(CLOCK_MONOTONIC, &end); /* mark the end time */
//	diff = BILLION * (end.tv_sec - start_exe.tv_sec) + end.tv_nsec - start_exe.tv_nsec; printf("exe: elapsed time = %llu nanoseconds\n", (long long unsigned int) diff);


  /* Save the result to a new .png file */
  lodepng_encode24_file("lenna512x512_orig.png", image , width,height);
  /*clean up */
//        clock_gettime(CLOCK_MONOTONIC, &end); /* mark the end time */
//        diff = BILLION * (end.tv_sec - start_hele.tv_sec) + end.tv_nsec - start_hele.tv_nsec; printf("hele: elapsed time = %llu nanoseconds\n", (long long unsigned int) diff);

        free(image);  cudaFree(image_d);
  return 0;
}
