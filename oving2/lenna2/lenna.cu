#include <iostream>
#include "lodepng.h"
#include <stdio.h>

__global__ void invert( unsigned char* image_d ){

        int id=blockIdx.x*blockDim.x+threadIdx.x;
        image_d[id] = ~image_d[id];
}


int main( int argc, char ** argv){
        cudaEvent_t event1, event2, event3, event4;
        cudaEventCreate(&event1);
        cudaEventCreate(&event2);
        cudaEventCreate(&event3);
        cudaEventCreate(&event4);
        cudaEventRecord(event3, 0);


  size_t pngsize;
  unsigned char *png;
  const char * filename = "lenna512x512_inv.png";
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
        /*dim3 gridblock, threadBlock;
        gridBlock.x=4; gridBlock.y=4;
        threadBlock.x=5; threadBlock.y=5;
        invert<<<gridBlock, threadBlock>>>(image) */

        unsigned char* image_d;

        size_t size;
        size=height*width*3*sizeof(char);

        cudaMalloc((void**) &image_d, size);
	
//	cudaEvent_t event1, event2;
//	cudaEventCreate(&event1);
//	cudaEventCreate(&event2);
	
	cudaEventRecord(event1, 0);	
	cudaMemcpy(image_d, image, size, cudaMemcpyHostToDevice);
        cudaEventRecord(event2, 0);
	
	cudaEventSynchronize(event2);
	
	float dt_ms;
	cudaEventElapsedTime(&dt_ms, event1, event2);
	
	printf("%f\n", dt_ms);
	int threadBlock=1024;
	int gridBlock=3*512*512/threadBlock;

        invert<<<gridBlock, threadBlock>>>(image_d);

        cudaMemcpy(image, image_d, size, cudaMemcpyDeviceToHost);
  /* Save the result to a new .png file */
  lodepng_encode24_file("lenna512x512_orig.png", image , width,height);
  /*clean up */
        free(image);  cudaFree(image_d);
	 cudaEventRecord(event4, 0);
        cudaEventSynchronize(event4);
        float dt_ms_tot;
        cudaEventElapsedTime(&dt_ms_tot, event3, event4);
        printf("%f\n", dt_ms_tot);


  return 0;
}
