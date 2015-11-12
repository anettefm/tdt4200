#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ppmCU.h"


/* time make rungpu:
real    0m3.814s
user    0m1.561s
sys     0m0.389s
*/

// Image from:
// http://7-themes.com/6971875-funny-flowers-pictures.html

typedef struct {
     float red,green,blue;
} AccuratePixel;

typedef struct {
     int x, y;
     AccuratePixel *data;
} AccurateImage;


// TODO: You should implement this
__global__ void convertImageToNewFormatGPU( PPMPixel *imagePPM, AccuratePixel *image ) { 

	int senterX= blockIdx.x*blockDim.x+threadIdx.x;
	int senterY=blockIdx.y*blockDim.y+ threadIdx.y;

	int numberOfValuesInEachRow = gridDim.x*blockDim.x;
	int i = numberOfValuesInEachRow * senterY + senterX;

	image[i].red   =(float)(imagePPM[i].red);
	image[i].green = (float)(imagePPM[i].green);
	image[i].blue  =(float)(imagePPM[i].blue);
 }

// Convert a high-precision format to a PPM image
PPMImage *convertNewFormatToPPM(AccurateImage *image) {
	// Make a copy
	PPMImage *imagePPM;
	imagePPM = (PPMImage *)malloc(sizeof(PPMImage));
	imagePPM->data = (PPMPixel*)malloc(image->x * image->y * sizeof(PPMPixel));
	for(int i = 0; i < image->x * image->y; i++) {
		imagePPM->data[i].red   = (unsigned char) image->data[i].red;
		imagePPM->data[i].green = (unsigned char) image->data[i].green;
		imagePPM->data[i].blue  = (unsigned char) image->data[i].blue;
	}
	imagePPM->x = image->x;
	imagePPM->y = image->y;
	
	return imagePPM;
}

AccurateImage *createEmptyImage(PPMImage *image){
	AccurateImage *imageAccurate;
	imageAccurate = (AccurateImage *)malloc(sizeof(AccurateImage));
	imageAccurate->data = (AccuratePixel*)malloc(image->x * image->y * sizeof(AccuratePixel));
	imageAccurate->x = image->x;
	imageAccurate->y = image->y;
	
	return imageAccurate;
}

// free memory of an AccurateImage
void freeImage(AccurateImage *image){
	free(image->data);
	free(image);
}


// TODO: You must implement this
// The handout code is much simpler than the MPI/OpenMP versions
__global__ void performNewIdeaIterationGPU(AccuratePixel *imageOut, AccuratePixel *imageIn, int size) { 

	int senterX= blockIdx.x*blockDim.x+threadIdx.x;
	int senterY=blockIdx.y*blockDim.y+ threadIdx.y;

	// For each pixel we compute the magic number
	float sumR = 0;
	float sumG = 0;
	float sumB = 0;
	int countIncluded = 0;
	for(int x = -size; x <= size; x++) {
	
		for(int y = -size; y <= size; y++) {
			int currentX = senterX + x;
			int currentY = senterY + y;
			
			// Check if we are outside the bounds
			if(currentX < 0)
				continue;
			if(currentX >=gridDim.x*blockDim.x)
				continue;
			if(currentY < 0)
				continue;
			if(currentY >= gridDim.y*blockDim.y)
				continue;
			
			// Now we can begin
			int numberOfValuesInEachRow = gridDim.x*blockDim.x; 
			int offsetOfThePixel = (numberOfValuesInEachRow * currentY + currentX);
			sumR += imageIn[offsetOfThePixel].red;
			sumG += imageIn[offsetOfThePixel].green;
			sumB +=imageIn[offsetOfThePixel].blue;
		
			// Keep track of how many values we have included
			countIncluded++;
		}
	
	}
	
	// Now we compute the final value for all colours
	float valueR = sumR / countIncluded;
	float valueG = sumG / countIncluded;
	float valueB = sumB / countIncluded;
	
	// Update the output image
	int numberOfValuesInEachRow =gridDim.x*blockDim.x; // R, G and B
	int offsetOfThePixel = (numberOfValuesInEachRow * senterY + senterX);
	imageOut[offsetOfThePixel].red = valueR;
	imageOut[offsetOfThePixel].green = valueG;
	imageOut[offsetOfThePixel].blue = valueB;

}


// TODO: You should implement this
__global__ void performNewIdeaFinalizationGPU( AccuratePixel *imageInSmall, AccuratePixel *imageInLarge, PPMPixel *imageOut) { 
	int senterX= blockIdx.x*blockDim.x+threadIdx.x;
	int senterY=blockIdx.y*blockDim.y+ threadIdx.y;

	int numberOfValuesInEachRow = gridDim.x*blockDim.x;
	int i = numberOfValuesInEachRow * senterY + senterX;

	float value = (imageInLarge[i].red - imageInSmall[i].red);
	if(value > 255.0f)
		imageOut[i].red = 255;
	else if (value < -1.0f) {
		value = 257.0f+value;
		if(value > 255.0f)
			imageOut[i].red = 255;
		else
			imageOut[i].red = floorf(value);
	} else if (value > -1.0f && value < 0.0f) {
		imageOut[i].red = 0;
	} else {
		imageOut[i].red = floorf(value);
	}
	
	value = (imageInLarge[i].green - imageInSmall[i].green);
	if(value > 255.0f)
		imageOut[i].green = 255;
	else if (value < -1.0f) {
		value = 257.0f+value;
		if(value > 255.0f)
			imageOut[i].green = 255;
		else
			imageOut[i].green = floorf(value);
	} else if (value > -1.0f && value < 0.0f) {
		imageOut[i].green = 0;
	} else {
		imageOut[i].green = floorf(value);
	}
	
	value = (imageInLarge[i].blue - imageInSmall[i].blue);
	if(value > 255.0f)
		imageOut[i].blue = 255;
	else if (value < -1.0f) {
		value = 257.0f+value;
		if(value > 255.0f)
			imageOut[i].blue = 255;
		else
			imageOut[i].blue = floorf(value);
	} else if (value > -1.0f && value < 0.0f) {
		imageOut[i].blue = 0;
	} else {
		imageOut[i].blue = floorf(value);
	}


 }


int main(int argc, char** argv) {
	
	PPMImage *image;
        
	if(argc > 1) {
		image = readPPM("flower.ppm");
	} else {
		image = readStreamPPM(stdin);
	}

	
	dim3 gridBlock, threadBlock;
	threadBlock.x=32;
	threadBlock.y=30;
    	gridBlock.x=image->x/threadBlock.x;
    	gridBlock.y=(image->y/threadBlock.y);

	cudaError_t err = cudaSuccess; 
	

	PPMPixel *imagePPMGPU;
	cudaMalloc((void**)& imagePPMGPU, 3*image->x*image->y*sizeof(char));
	err=cudaMemcpy(imagePPMGPU, image->data, 3*image->x*image->y*sizeof(unsigned char), cudaMemcpyHostToDevice);
    	if(err !=cudaSuccess){
        	printf("%s-%d\n",cudaGetErrorString(err),1);
        	getchar();  
    	}


	AccuratePixel *imageUnchangedGPU;
	AccuratePixel *imageSmallGPU;
	AccuratePixel *imageBigGPU;
	AccuratePixel *imageBufferGPU;

    err=cudaMalloc((void**)&imageUnchangedGPU, 3*image->x * image->y * sizeof(float));
    err=cudaMalloc((void**)&imageSmallGPU, 3*image->x * image->y * sizeof(float));
    err=cudaMalloc((void**)&imageBigGPU, 3*image->x * image->y * sizeof(float));
    err=cudaMalloc((void**)&imageBufferGPU, 3*image->x * image->y * sizeof(float));
    if(err !=0){
        printf("%s-%d\n",cudaGetErrorString(err),1);
        getchar();  
    }


	convertImageToNewFormatGPU<<<gridBlock, threadBlock>>>( imagePPMGPU, imageUnchangedGPU );


	// Process the tiny case:
	performNewIdeaIterationGPU<<<gridBlock, threadBlock>>>(imageSmallGPU, imageUnchangedGPU, 2);
	performNewIdeaIterationGPU<<<gridBlock, threadBlock>>>(imageBufferGPU, imageSmallGPU, 2);
	performNewIdeaIterationGPU<<<gridBlock, threadBlock>>>(imageSmallGPU, imageBufferGPU, 2);
	performNewIdeaIterationGPU<<<gridBlock, threadBlock>>>(imageBufferGPU, imageSmallGPU, 2);
	performNewIdeaIterationGPU<<<gridBlock, threadBlock>>>(imageSmallGPU, imageBufferGPU, 2);
	
	// Process the small case:
	performNewIdeaIterationGPU<<<gridBlock, threadBlock>>>(imageBigGPU, imageUnchangedGPU,3);
	performNewIdeaIterationGPU<<<gridBlock, threadBlock>>>(imageBufferGPU, imageBigGPU,3);
	performNewIdeaIterationGPU<<<gridBlock, threadBlock>>>(imageBigGPU, imageBufferGPU,3);
	performNewIdeaIterationGPU<<<gridBlock, threadBlock>>>(imageBufferGPU, imageBigGPU,3);
	performNewIdeaIterationGPU<<<gridBlock, threadBlock>>>(imageBigGPU, imageBufferGPU,3);
	

	// save tiny case result
	performNewIdeaFinalizationGPU<<<gridBlock, threadBlock>>>( imageSmallGPU, imageBigGPU, imagePPMGPU);	
	err=cudaMemcpy(image->data, imagePPMGPU,  3*image->x*image->y*sizeof(unsigned char), cudaMemcpyDeviceToHost);
    	if(err !=0){
        	printf("%s-%d\n",cudaGetErrorString(err),1);
        	getchar();  
    	}


	if(argc > 1) {
		writePPM("flower_tiny.ppm", image);
	} else {
		writeStreamPPM(stdout, image);
	}

	
	// Process the medium case:
	performNewIdeaIterationGPU<<<gridBlock, threadBlock>>>(imageSmallGPU, imageUnchangedGPU, 5);
	performNewIdeaIterationGPU<<<gridBlock, threadBlock>>>(imageBufferGPU, imageSmallGPU, 5);
	performNewIdeaIterationGPU<<<gridBlock, threadBlock>>>(imageSmallGPU, imageBufferGPU, 5);
	performNewIdeaIterationGPU<<<gridBlock, threadBlock>>>(imageBufferGPU, imageSmallGPU, 5);
	performNewIdeaIterationGPU<<<gridBlock, threadBlock>>>(imageSmallGPU, imageBufferGPU, 5);

	// save small case
	performNewIdeaFinalizationGPU<<<gridBlock, threadBlock>>>( imageBigGPU, imageSmallGPU, imagePPMGPU);	
	err=cudaMemcpy(image->data, imagePPMGPU,  3*image->x*image->y*sizeof(unsigned char), cudaMemcpyDeviceToHost);
    	if(err !=0){
        	printf("%s-%d\n",cudaGetErrorString(err),1);
        	getchar();  
    	}
	if(argc > 1) {
		writePPM("flower_small.ppm", image);
	} else {
		writeStreamPPM(stdout, image);
	}

	// process the large case
	performNewIdeaIterationGPU<<<gridBlock, threadBlock>>>(imageBigGPU, imageUnchangedGPU, 8);
	performNewIdeaIterationGPU<<<gridBlock, threadBlock>>>(imageBufferGPU, imageBigGPU, 8);
	performNewIdeaIterationGPU<<<gridBlock, threadBlock>>>(imageBigGPU, imageBufferGPU, 8);
	performNewIdeaIterationGPU<<<gridBlock, threadBlock>>>(imageBufferGPU, imageBigGPU, 8);
	performNewIdeaIterationGPU<<<gridBlock, threadBlock>>>(imageBigGPU, imageBufferGPU, 8);


	// save the medium case
	performNewIdeaFinalizationGPU<<<gridBlock, threadBlock>>>( imageSmallGPU, imageBigGPU, imagePPMGPU);	
	err=cudaMemcpy(image->data, imagePPMGPU,  3*image->x*image->y*sizeof(unsigned char), cudaMemcpyDeviceToHost);
    	if(err !=0){
        	printf("%s-%d\n",cudaGetErrorString(err),1);
        	getchar();  
    	}
	if(argc > 1) {
		writePPM("flower_medium.ppm", image);
	} else {
		writeStreamPPM(stdout, image);
	}
	
	// free all memory structures
	free(image->data);
	free(image);
	cudaFree(imagePPMGPU);
	cudaFree(imageBigGPU);
	cudaFree(imageBufferGPU);
	cudaFree(imageUnchangedGPU);
	cudaFree(imageSmallGPU);
	return 0;
}

