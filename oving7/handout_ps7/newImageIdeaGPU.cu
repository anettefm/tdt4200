#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ppmCU.h"

// Image from:
// http://7-themes.com/6971875-funny-flowers-pictures.html



// TODO: You should implement this
//__global__ void performNewIdeaFinalizationGPU( ... ) { ... }

// TODO: You should implement this
//__global__ void convertImageToNewFormatGPU( ... ) { ... }

// Perhaps some extra kernels will be practical as well?
//__global__ void ...GPU( ... ) { ... }

typedef struct {
     float red,green,blue;
} AccuratePixel;

typedef struct {
     int x, y;
     AccuratePixel *data;
} AccurateImage;

// Convert a PPM image to a high-precision format 
AccurateImage *convertImageToNewFormat(PPMImage *image) {
	// Make a copy
	AccurateImage *imageAccurate;
	imageAccurate = (AccurateImage *)malloc(sizeof(AccurateImage));
	imageAccurate->data = (AccuratePixel*)malloc(image->x * image->y * sizeof(AccuratePixel));
	for(int i = 0; i < image->x * image->y; i++) {
		imageAccurate->data[i].red   = (float) image->data[i].red;
		imageAccurate->data[i].green = (float) image->data[i].green;
		imageAccurate->data[i].blue  = (float) image->data[i].blue;
	}
	imageAccurate->x = image->x;
	imageAccurate->y = image->y;
	
	return imageAccurate;
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

/*void performNewIdeaIteration(AccurateImage *imageOut, AccurateImage *imageIn, int size) {
	
	// Iterate over each pixel
	for(int senterX = 0; senterX < imageIn->x; senterX++) {
	
		for(int senterY = 0; senterY < imageIn->y; senterY++) {
			
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
					if(currentX >= imageIn->x)
						continue;
					if(currentY < 0)
						continue;
					if(currentY >= imageIn->y)
						continue;
					
					// Now we can begin
					int numberOfValuesInEachRow = imageIn->x; 
					int offsetOfThePixel = (numberOfValuesInEachRow * currentY + currentX);
					sumR += imageIn->data[offsetOfThePixel].red;
					sumG += imageIn->data[offsetOfThePixel].green;
					sumB += imageIn->data[offsetOfThePixel].blue;
				
					// Keep track of how many values we have included
					countIncluded++;
				}
			
			}
			
			// Now we compute the final value for all colours
			float valueR = sumR / countIncluded;
			float valueG = sumG / countIncluded;
			float valueB = sumB / countIncluded;
			
			// Update the output image
			int numberOfValuesInEachRow = imageOut->x; // R, G and B
			int offsetOfThePixel = (numberOfValuesInEachRow * senterY + senterX);
			imageOut->data[offsetOfThePixel].red = valueR;
			imageOut->data[offsetOfThePixel].green = valueG;
			imageOut->data[offsetOfThePixel].blue = valueB;
		}
	}
}*/
// TODO: You must implement this
// The handout code is much simpler than the MPI/OpenMP versions
__global__ void performNewIdeaIterationGPU(AccurateImage *imageOut, AccurateImage *imageIn, int size) { 

	int senterX=threadIdx.x;
	int senterY=threadIdx.y;

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
			if(currentX >= blockDim.x)
				continue;
			if(currentY < 0)
				continue;
			if(currentY >= blockDim.y)
				continue;
			
			// Now we can begin
			int numberOfValuesInEachRow = blockDim.y; 
			int offsetOfThePixel = (numberOfValuesInEachRow * currentY + currentX);
			sumR += imageIn->data[offsetOfThePixel].red;
			sumG += imageIn->data[offsetOfThePixel].green;
			sumB += imageIn->data[offsetOfThePixel].blue;
		
			// Keep track of how many values we have included
			countIncluded++;
		}
	
	}
	
	// Now we compute the final value for all colours
	float valueR = sumR / countIncluded;
	float valueG = sumG / countIncluded;
	float valueB = sumB / countIncluded;
	
	// Update the output image
	int numberOfValuesInEachRow = blockDim.x; // R, G and B
	int offsetOfThePixel = (numberOfValuesInEachRow * senterY + senterX);
	imageOut->data[offsetOfThePixel].red = valueR;
	imageOut->data[offsetOfThePixel].green = valueG;
	imageOut->data[offsetOfThePixel].blue = valueB;
}

// Perform the final step, and save it as a ppm in imageOut
void performNewIdeaFinalization(AccurateImage *imageInSmall, AccurateImage *imageInLarge, PPMImage *imageOut) {

	
	imageOut->x = imageInSmall->x;
	imageOut->y = imageInSmall->y;
	
	for(int i = 0; i < imageInSmall->x * imageInSmall->y; i++) {
		float value = (imageInLarge->data[i].red - imageInSmall->data[i].red);
		if(value > 255.0f)
			imageOut->data[i].red = 255;
		else if (value < -1.0f) {
			value = 257.0f+value;
			if(value > 255.0f)
				imageOut->data[i].red = 255;
			else
				imageOut->data[i].red = floorf(value);
		} else if (value > -1.0f && value < 0.0f) {
			imageOut->data[i].red = 0;
		} else {
			imageOut->data[i].red = floorf(value);
		}
		
		value = (imageInLarge->data[i].green - imageInSmall->data[i].green);
		if(value > 255.0f)
			imageOut->data[i].green = 255;
		else if (value < -1.0f) {
			value = 257.0f+value;
			if(value > 255.0f)
				imageOut->data[i].green = 255;
			else
				imageOut->data[i].green = floorf(value);
		} else if (value > -1.0f && value < 0.0f) {
			imageOut->data[i].green = 0;
		} else {
			imageOut->data[i].green = floorf(value);
		}
		
		value = (imageInLarge->data[i].blue - imageInSmall->data[i].blue);
		if(value > 255.0f)
			imageOut->data[i].blue = 255;
		else if (value < -1.0f) {
			value = 257.0f+value;
			if(value > 255.0f)
				imageOut->data[i].blue = 255;
			else
				imageOut->data[i].blue = floorf(value);
		} else if (value > -1.0f && value < 0.0f) {
			imageOut->data[i].blue = 0;
		} else {
			imageOut->data[i].blue = floorf(value);
		}
	}
}

int main(int argc, char** argv) {
	
	PPMImage *image;
        
	if(argc > 1) {
		image = readPPM("flower.ppm");
	} else {
		image = readStreamPPM(stdin);
	}

	AccurateImage *imageUnchanged = convertImageToNewFormat(image); // save the unchanged image from input image
	AccurateImage *imageBuffer = createEmptyImage(image);
	AccurateImage *imageSmall = createEmptyImage(image);
	AccurateImage *imageBig = createEmptyImage(image);
	
	PPMImage *imageOut;
	imageOut = (PPMImage *)malloc(sizeof(PPMImage));
	imageOut->data = (PPMPixel*)malloc(image->x * image->y * sizeof(PPMPixel));
	
	dim3 gridBlock, threadBlock;

	threadBlock.x=32;
	threadBlock.y=30;
    gridBlock.x=imageBuffer->x/threadBlock.x;
    gridBlock.y=imageBuffer->y/threadBlock.y;

/*
    int numPoints    = 16,
        gpuBlockSize = 4,
        pointSize    = sizeof(point),
        numBytes     = numPoints * pointSize,
        gpuGridSize  = numPoints / gpuBlockSize;

        // allocate memory
    point *cpuPointArray,
          *gpuPointArray;
    cpuPointArray = (point*)malloc(numBytes);
    cudaMalloc((void**)&gpuPointArray, numBytes);*/

	AccurateImage *imageUnchangedGPU;
	AccurateImage *imageSmallGPU;
	AccurateImage *imageBigGPU;
	AccurateImage *imageBufferGPU;

    cudaMalloc((void**)&imageUnchangedGPU, image->x * image->y * sizeof(AccuratePixel));
    cudaMalloc((void**)&imageSmallGPU, image->x * image->y * sizeof(AccuratePixel));
    cudaMalloc((void**)&imageBigGPU, image->x * image->y * sizeof(AccuratePixel));
    cudaMalloc((void**)&imageBufferGPU, image->x * image->y * sizeof(AccuratePixel));

    cudaMemcpy(imageUnchangedGPU->data, imageUnchanged->data, 3*imageUnchanged->x*imageUnchanged->y, cudaMemcpyHostToDevice);


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
	
	cudaMemcpy(imageSmall->data, imageSmallGPU->data, 3*imageUnchanged->x*imageUnchanged->y, cudaMemcpyDeviceToHost);
	cudaMemcpy(imageBig->data, imageBigGPU->data, 3*imageUnchanged->x*imageUnchanged->y, cudaMemcpyDeviceToHost);
	// save tiny case result
	performNewIdeaFinalization(imageSmall,  imageBig, imageOut);
	if(argc > 1) {
		writePPM("flower_tinyGPU.ppm", imageOut);
	} else {
		writeStreamPPM(stdout, imageOut);
	}

	
	// Process the medium case:
	performNewIdeaIterationGPU<<<gridBlock, threadBlock>>>(imageSmallGPU, imageUnchangedGPU, 5);
	performNewIdeaIterationGPU<<<gridBlock, threadBlock>>>(imageBufferGPU, imageSmallGPU, 5);
	performNewIdeaIterationGPU<<<gridBlock, threadBlock>>>(imageSmallGPU, imageBufferGPU, 5);
	performNewIdeaIterationGPU<<<gridBlock, threadBlock>>>(imageBufferGPU, imageSmallGPU, 5);
	performNewIdeaIterationGPU<<<gridBlock, threadBlock>>>(imageSmallGPU, imageBufferGPU, 5);

	cudaMemcpy(imageSmall->data, imageSmallGPU->data, 3*imageUnchanged->x*imageUnchanged->y, cudaMemcpyDeviceToHost);

	// save small case
	performNewIdeaFinalization(imageBig,  imageSmall,imageOut);
	if(argc > 1) {
		writePPM("flower_smallGPU.ppm", imageOut);
	} else {
		writeStreamPPM(stdout, imageOut);
	}

	// process the large case
	performNewIdeaIterationGPU<<<gridBlock, threadBlock>>>(imageBigGPU, imageUnchangedGPU, 8);
	performNewIdeaIterationGPU<<<gridBlock, threadBlock>>>(imageBufferGPU, imageBigGPU, 8);
	performNewIdeaIterationGPU<<<gridBlock, threadBlock>>>(imageBigGPU, imageBufferGPU, 8);
	performNewIdeaIterationGPU<<<gridBlock, threadBlock>>>(imageBufferGPU, imageBigGPU, 8);
	performNewIdeaIterationGPU<<<gridBlock, threadBlock>>>(imageBigGPU, imageBufferGPU, 8);

	cudaMemcpy(imageBig->data, imageBigGPU->data, 3*imageUnchanged->x*imageUnchanged->y, cudaMemcpyDeviceToHost);

	// save the medium case
	performNewIdeaFinalization(imageSmall,  imageBig, imageOut);
	if(argc > 1) {
		writePPM("flower_mediumGPU.ppm", imageOut);
	} else {
		writeStreamPPM(stdout, imageOut);
	}
	
	// free all memory structures
	freeImage(imageUnchanged);
	freeImage(imageBuffer);
	freeImage(imageSmall);
	freeImage(imageBig);
	free(imageOut->data);
	free(imageOut);
	free(image->data);
	free(image);
	
	return 0;
}

