#include <iostream>
#include "lodepng.h"

__global__ void invert(unsigned char* image_h, unsigned char* image_d ){

        int id=blockIdx.x*blockDim.x+threadIdx.x;
        image_d[id]=~image_h[id];
}


int main( int argc, char ** argv){

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

        unigned char* image_d;

        size_t size;
        size=high*width*3*sizeof(char);

        cudaMalloc((void**) &image_d, size);
        cudaMemcpy(image, image_d, size, cudaMemcpyHostToDevice);
        int gridBlock;
        int threadBlock;

        invert<<gridBlock, threadBlock>>(image, image_d);

        cudaMemcpy(image_d, image, cudaMemcpyDeviceToHost);

  /* Save the result to a new .png file */
  lodepng_encode24_file("lenna512x512_orig.png", image , width,height);
  /*clean up */
        free(image);  cudaFree(image_d);

  return 0;
}

