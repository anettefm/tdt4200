#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#define MPI_MAXPE 4
#define NBR_EXCH_TAG 1

#include "ppm.h"


// Image from:
// http://7-themes.com/6971875-funny-flowers-pictures.html

typedef struct {
	float red,green,blue;
} AccuratePixel;

typedef struct {
	int x, y;
	AccuratePixel *data;
} AccurateImage;

// Convert ppm to high precision format.
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

// Perform the new idea:
// Using MPI inside this function is not needed
void performNewIdeaIteration(AccurateImage *imageOut, AccurateImage *imageIn,int size) {
	int countIncluded = 0;
	int offsetOfThePixel=0;
	float sum_red = 0;
	float sum_blue = 0;
	float sum_green =0;
	int numberOfValuesInEachRow = imageIn->x;

	// line buffer that will save the sum of some pixel in the column
	AccuratePixel *line_buffer = (AccuratePixel*) malloc(imageIn->x*sizeof(AccuratePixel));
	memset(line_buffer,0,imageIn->x*sizeof(AccuratePixel));

	// Iterate over each line of pixelx.
	for(int senterY = 0; senterY < imageIn->y; senterY++) {
		// first and last line considered  by the computation of the pixel in the line senterY
		int starty = senterY-size;
		int endy = senterY+size;

		if (starty <=0){
			starty = 0;
			if(senterY == 0){
				// for all pixel in the first line, we sum all pixel of the column (until the line endy)
				// we save the result in the array line_buffer
				for(int line_y=starty; line_y < endy; line_y++){
					for(int i=0; i<imageIn->x; i++){
						line_buffer[i].blue+=imageIn->data[numberOfValuesInEachRow*line_y+i].blue;
						line_buffer[i].red+=imageIn->data[numberOfValuesInEachRow*line_y+i].red;
						line_buffer[i].green+=imageIn->data[numberOfValuesInEachRow*line_y+i].green;
					}
				}
			}
			for(int i=0; i<imageIn->x; i++){
				// add the next pixel of the next line in the column x
				line_buffer[i].blue+=imageIn->data[numberOfValuesInEachRow*endy+i].blue;
				line_buffer[i].red+=imageIn->data[numberOfValuesInEachRow*endy+i].red;
				line_buffer[i].green+=imageIn->data[numberOfValuesInEachRow*endy+i].green;
			}

		}

		else if (endy >= imageIn->y ){
			// for the last lines, we just need to subtract the first added line
			endy = imageIn->y-1;
			for(int i=0; i<imageIn->x; i++){
				line_buffer[i].blue-=imageIn->data[numberOfValuesInEachRow*(starty-1)+i].blue;
				line_buffer[i].red-=imageIn->data[numberOfValuesInEachRow*(starty-1)+i].red;
				line_buffer[i].green-=imageIn->data[numberOfValuesInEachRow*(starty-1)+i].green;
			}	
		}else{
			// general case
			// add the next line and remove the first added line
			for(int i=0; i<imageIn->x; i++){
				line_buffer[i].blue+=imageIn->data[numberOfValuesInEachRow*endy+i].blue-imageIn->data[numberOfValuesInEachRow*(starty-1)+i].blue;
				line_buffer[i].red+=imageIn->data[numberOfValuesInEachRow*endy+i].red-imageIn->data[numberOfValuesInEachRow*(starty-1)+i].red;
				line_buffer[i].green+=imageIn->data[numberOfValuesInEachRow*endy+i].green-imageIn->data[numberOfValuesInEachRow*(starty-1)+i].green;
			}	
		}
		
		sum_green =0;
		sum_red = 0;
		sum_blue = 0;
		for(int senterX = 0; senterX < imageIn->x; senterX++) {
			// in this loop, we do exactly the same thing as before but only with the array line_buffer

			int startx = senterX-size;
			int endx = senterX+size;

			if (startx <=0){
				startx = 0;
				if(senterX==0){
					for (int x=startx; x < endx; x++){
						sum_red += line_buffer[x].red;
						sum_green += line_buffer[x].green;
						sum_blue += line_buffer[x].blue;
					}
				}
				sum_red +=line_buffer[endx].red;
				sum_green +=line_buffer[endx].green;
				sum_blue +=line_buffer[endx].blue;
			}else if (endx >= imageIn->x){
				endx = imageIn->x-1;
				sum_red -=line_buffer[startx-1].red;
				sum_green -=line_buffer[startx-1].green;
				sum_blue -=line_buffer[startx-1].blue;

			}else{
				sum_red += (line_buffer[endx].red-line_buffer[startx-1].red);
				sum_green += (line_buffer[endx].green-line_buffer[startx-1].green);
				sum_blue += (line_buffer[endx].blue-line_buffer[startx-1].blue);
			}			

			// we save each pixel in the output image
			offsetOfThePixel = (numberOfValuesInEachRow * senterY + senterX);
			countIncluded=(endx-startx+1)*(endy-starty+1);

			imageOut->data[offsetOfThePixel].red = sum_red/countIncluded;
			imageOut->data[offsetOfThePixel].green = sum_green/countIncluded;
			imageOut->data[offsetOfThePixel].blue = sum_blue/countIncluded;
		}

	}

	// free memory
	free(line_buffer);	
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
		}  else {
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
			imageOut->data[i].green = 0.0f;
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

	// All use of MPI can be in this function
	// Process the four cases in parallel
	// Exchanging image buffers gives quite big messages
	// Use asynchronous MPI to post the receives ahead of the sends

    
    	int rank, size;

	MPI_Request request;
	MPI_Status status;
        

	MPI_Init(&argc, &argv);

    	MPI_Comm_size(MPI_COMM_WORLD, &size);
   	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	PPMImage *image;

	// The MPI version will always read from file
        image = readPPM("flower.ppm");

	PPMImage *imageOut;
	imageOut = (PPMImage *)malloc(sizeof(PPMImage));
	imageOut->data = (PPMPixel*)malloc(image->x * image->y * sizeof(PPMPixel));

        AccurateImage *imageUnchanged = convertImageToNewFormat(image);
	AccurateImage *imageBuffer = createEmptyImage(image);
	AccurateImage *imageSmall = createEmptyImage(image);
	AccurateImage *imageMedium = createEmptyImage(image);
	AccurateImage *imageBig = createEmptyImage(image);


    if(rank==0){



  
 // save the unchanged image from input image
	// Process the tiny case:
	performNewIdeaIteration(imageBuffer, imageUnchanged, 2);
	performNewIdeaIteration(imageUnchanged, imageBuffer, 2);
	performNewIdeaIteration(imageBuffer, imageUnchanged, 2);
	performNewIdeaIteration(imageUnchanged, imageBuffer, 2);
	performNewIdeaIteration(imageBuffer, imageUnchis process called "init", but exited without calling "finalize".
By rule, all processes that call "init" MUST call "finalize" prior to
exiting or it will be considered an "abnormal termination"

anged, 2);

	MPI_Irecv(imageSmall->data, 3*imageUnchanged->x*imageUnchanged->y, MPI_FLOAT, 1, 1 , MPI_COMM_WORLD, &request);
	MPI_Wait(&request, &status);
	MPI_Irecv(imageMedium->data, 3*imageUnchanged->x*imageUnchanged->y, MPI_FLOAT, 2, 2 , MPI_COMM_WORLD, &request);
	performNewIdeaFinalization(imageBuffer,  imageSmall, imageOut);
	writePPM("flower_small.ppm", imageOut);	
	MPI_Wait(&request, &status);
	MPI_Irecv(imageBig->data, 3*imageUnchanged->x*imageUnchanged->y, MPI_FLOAT, 3, 3 , MPI_COMM_WORLD, &request);
	performNewIdeaFinalization(imageSmall,  imageMedium,imageOut);
	writePPM("flower_medium.ppm", imageOut);	
	MPI_Wait(&request, &status);
	performNewIdeaFinalization(imageMedium,  imageBig,imageOut);
	writePPM("flower_large.ppm", imageOut);
      

printf("rank %d \n", rank);






    } else if (rank==1){


        //MPI_Irecv(&imageSmall->data, 3*imageBuffer->x*imageBuffer->y, MPI_FLOAT, 0, tag01 , MPI_COMM_WORLD, &request);
        // Process the small case:

        performNewIdeaIteration(imageBuffer, imageUnchanged,3);
        performNewIdeaIteration(imageUnchanged, imageBuffer,3);
        performNewIdeaIteration(imageBuffer, imageUnchanged,3);
        performNewIdeaIteration(imageUnchanged, imageBuffer,3);
        performNewIdeaIteration(imageBuffer, imageUnchanged,3);

        // send og motta
	MPI_Send(imageBuffer->data, 3*imageUnchanged->x*imageUnchanged->y, MPI_FLOAT, 0, 1 , MPI_COMM_WORLD);
              printf("rank %d \n", rank);


    } else if (rank==2){

        // Process the medium case:
        performNewIdeaIteration(imageBuffer, imageUnchanged, 5);
        performNewIdeaIteration(imageUnchanged, imageBuffer, 5);
        performNewIdeaIteration(imageBuffer, imageUnchanged, 5);
        performNewIdeaIteration(imageUnchanged, imageBuffer, 5);
        performNewIdeaIteration(imageBuffer, imageUnchanged, 5);

        // send og motta
	MPI_Send(imageBuffer->data,3*imageUnchanged->x*imageUnchanged->y, MPI_FLOAT, 0, 2, MPI_COMM_WORLD);
	
              printf("rank %d \n", rank);


    }else{
        
	
        // process the large case
        performNewIdeaIteration(imageBuffer, imageUnchanged, 8);
        performNewIdeaIteration(imageUnchanged, imageBuffer, 8);
        performNewIdeaIteration(imageBuffer, imageUnchanged, 8);
        performNewIdeaIteration(imageUnchanged, imageBuffer, 8);
        performNewIdeaIteration(imageBuffer, imageUnchanged, 8);

        MPI_Send(imageBuffer->data,3*imageUnchanged->x*imageUnchanged->y, MPI_FLOAT, 0, 3, MPI_COMM_WORLD);

      printf("rank %d \n", rank);

    }


MPI_Barrier(MPI_COMM_WORLD);
	
	// free all memory structures
	freeImage(imageUnchanged);
	freeImage(imageBuffer);
	freeImage(imageSmall);
	free(imageOut->data);
	free(imageOut);
	free(image->data);
	free(image);
	
	return 0;
}
