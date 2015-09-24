#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "ppm.h"

// Image from:
// http://7-themes.com/6971875-funny-flowers-pictures.html

typedef struct {
     double red,green,blue;
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
		imageAccurate->data[i].red   = (double) image->data[i].red;
		imageAccurate->data[i].green = (double) image->data[i].green;
		imageAccurate->data[i].blue  = (double) image->data[i].blue;
	}
	imageAccurate->x = image->x;
	imageAccurate->y = image->y;
	
	return imageAccurate;
}

// Perform the new idea:
void performNewIdeaIteration(AccurateImage *imageOut, AccurateImage *imageIn, int size) {
	// Iterate over each pixel
	int numberOfValuesInEachRow = imageIn->x;
	double sum_red[2*size+1];
	for(int senterX=0; senterX<size+1; senterX++){
		int count=0;
		int county=0;
		for(int senterY=0; senterY<imageIn->y;senterY++){
			if( senterY==0){
				for(int i=0; i<size;i++){
					for(int j=0; j<size+senterX; j++){
						int offsetOfThePixel = (numberOfValuesInEachRow * (senterY + i) + j);
						sum_red[i]+= imageIn->data[offsetOfThePixel].red;
					}
				}
				for(int i=size+1; i<2*size+1; i++){
					sum_red[i]=0;
				}
				count=size+1;
			} else if(senterY+size>imageIn->y){
				sum_red[count]=0;
				county--;
			} else{
				for(int j=0; j<size+senterX; j++){
                     			int offsetOfThePixel = (numberOfValuesInEachRow * (senterY + size) + j);
	                        	sum_red[count]+= imageIn->data[offsetOfThePixel].red;
                        	}
				if(senterY+size<=2*size+1)
					county=senterY+size;
			}
			double value_red=0;
			for(int i=0; i<2*size+1;i++)
				value_red+=sum_red[i];
						
			int offsetOfThePixel = (numberOfValuesInEachRow * senterY + senterX);
                        imageOut->data[offsetOfThePixel].red = value_red/(county+senterX+size);
			
			if(count/(2*size)==1.0){
				count=0;
			}
			count++;
		}

	}

        for(int senterX=size+1; senterX<imageIn->x -2*size; senterX++){
                int count=0;
                int county=0;
                for(int senterY=0; senterY<imageIn->y;senterY++){
                        if( senterY==0){
                                for(int i=0; i<size; i++){
                                        for(int j=senterX-size; j<senterX+size; j++){
                                                int offsetOfThePixel = (numberOfValuesInEachRow * (senterY + i) + j);
                                                sum_red[i]+= imageIn->data[offsetOfThePixel].red;
                                        }
                                }
                                for(int i=size+1; i<2*size+1; i++){
                                        sum_red[i]=0;
                                }
 				count=size+1;
                        } else if(senterY+size>imageIn->y){
                                sum_red[count]=0;
                                county--;
                        } else{
                                for(int j=senterX-size; j<senterX+size; j++){
                                        int offsetOfThePixel = (numberOfValuesInEachRow * (senterY + size) + j);
                                        sum_red[count]+= imageIn->data[offsetOfThePixel].red;
                                }
                                if(senterY+size<=2*size+1)
                                        county=senterY+size;
                        }
                        double value_red=0;
                        for(int i=0; i<2*size+1;i++)
                                value_red+=sum_red[i];
                        

                        int offsetOfThePixel = (numberOfValuesInEachRow * senterY + senterX);
                        imageOut->data[offsetOfThePixel].red = value_red/(county+2*size+1);

                        if(count/(2*size)==1.0){
                                count=0;
                        }
                        count++;
                }

        }




	for(int senterX=imageIn->x-2*size; senterX<imageIn->x; senterX++){
                int count=0;
                int county=0;
                for(int senterY=0; senterY<imageIn->y;senterY++){
                        if( senterY==0){
                                for(int i=0; i<imageIn->y -senterY; i++){
                                        for(int j=senterX-size; j<imageIn->x; j++){
                                                int offsetOfThePixel = (numberOfValuesInEachRow * (senterY + i) + j);
                                                sum_red[i]+= imageIn->data[offsetOfThePixel].red;
                                        }
                                }
				count=size+1;
                                for(int i=size+1; i<2*size+1; i++){
                                        sum_red[i]=0;
                                }
                        } else if(senterY+size>imageIn->y){
                                sum_red[count]=0;
                                county--;
                        } else{
                                for(int j=senterX-size; j<imageIn->x; j++){
                                        int offsetOfThePixel = (numberOfValuesInEachRow * (senterY + size) + j);
                                        sum_red[count]+= imageIn->data[offsetOfThePixel].red;
                                }
                                if(imageIn->y-senterY<=2*size+1)
                                        county=senterY+size;
                        }
                        double value_red=0;
                        for(int i=0; i<2*size+1;i++)
                                value_red+=sum_red[i];
                        

                        int offsetOfThePixel = (numberOfValuesInEachRow * senterY + senterX);
                        imageOut->data[offsetOfThePixel].red = value_red/(county+imageIn->x-senterX+size);

                        if(count/(2*size)==1.0){
                                count=0;
                        }
                        count++;
                }

        }
		for (int senterY = 0; senterY < imageIn->y; senterY++) {
		for(int senterX = 0; senterX < imageIn->x; senterX++) {
		
			// For each pixel we compute the magic number
			double sum_green = 0.0;
			double sum_blue = 0.0;
			double countIncluded = 0.0;
			for(int y =- size; y <= size; y++) {
			
				for(int x = -size; x <= size; x++) {
					int currentY = senterY + y;
					 int currentX = senterX + x;
					
					// Check if we are outside the bounds
					if(currentX <0 )
						continue;
					if(currentX >= imageIn->x)
						continue;
					
					// Now we can begin
					//int numberOfValuesInEachRow = imageIn->x; 
					int offsetOfThePixel = (numberOfValuesInEachRow * currentY + currentX);
					sum_green += imageIn->data[offsetOfThePixel].green;
					sum_blue += imageIn->data[offsetOfThePixel].blue;
					
					// Keep track of how many values we have included
					countIncluded+=1.0;
				}
			
			}
			
			// Now we compute the final value
			double value_green = sum_green / countIncluded;
			double value_blue = sum_blue / countIncluded;

			// Update the output image
			//int numberOfValuesInEachRow = imageOut->x; // R, G and B
			int offsetOfThePixel = (numberOfValuesInEachRow * senterY + senterX);
			imageOut->data[offsetOfThePixel].green = value_green;
			imageOut->data[offsetOfThePixel].blue = value_blue;
		}
	
	}
	
}


// Perform the final step, and return it as ppm.
PPMImage * performNewIdeaFinalization(AccurateImage *imageInSmall, AccurateImage *imageInLarge) {
	PPMImage *imageOut;
	imageOut = (PPMImage *)malloc(sizeof(PPMImage));
	imageOut->data = (PPMPixel*)malloc(imageInSmall->x * imageInSmall->y * sizeof(PPMPixel));
	
	imageOut->x = imageInSmall->x;
	imageOut->y = imageInSmall->y;
	
	for(int i = 0; i < imageInSmall->x * imageInSmall->y; i++) {
		double value = (imageInLarge->data[i].red - imageInSmall->data[i].red);
		if(value > 255.0)
			imageOut->data[i].red = 255.0;
		else if (value < -1.0) {
			value = 257.0+value;
			if(value > 255.0)
				imageOut->data[i].red = 255.0;
			else
				imageOut->data[i].red = floor(value);
		} else if (value > -1.0 && value < 0.0) {
			imageOut->data[i].red = 0.0;
		} else {
			imageOut->data[i].red = floor(value);
		}
		
		value = (imageInLarge->data[i].green - imageInSmall->data[i].green);
		if(value > 255.0)
			imageOut->data[i].green = 255.0;
		else if (value < -1.0) {
			value = 257.0+value;
			if(value > 255.0)
				imageOut->data[i].green = 255.0;
			else
				imageOut->data[i].green = floor(value);
		} else if (value > -1.0 && value < 0.0) {
			imageOut->data[i].green = 0.0;
		} else {
			imageOut->data[i].green = floor(value);
		}
		
		value = (imageInLarge->data[i].blue - imageInSmall->data[i].blue);
		if(value > 255.0)
			imageOut->data[i].blue = 255.0;
		else if (value < -1.0) {
			value = 257.0+value;
			if(value > 255.0)
				imageOut->data[i].blue = 255.0;
			else
				imageOut->data[i].blue = floor(value);
		} else if (value > -1.0 && value < 0.0) {
			imageOut->data[i].blue = 0.0;
		} else {
			imageOut->data[i].blue = floor(value);
		}
	}
	
	
	return imageOut;
}


int main(int argc, char** argv) {
	
	PPMImage *image;
	
	if(argc > 1) {
		image = readPPM("flower.ppm");
	} else {
		image = readStreamPPM(stdin);
	}
	AccurateImage *imageAccurate1_tiny = convertImageToNewFormat(image);
	AccurateImage *imageAccurate2_tiny = convertImageToNewFormat(image);
	
	// Process the tiny case:
		int size = 2; 
		performNewIdeaIteration(imageAccurate2_tiny, imageAccurate1_tiny,  size);
		performNewIdeaIteration(imageAccurate1_tiny, imageAccurate2_tiny,  size);
		performNewIdeaIteration(imageAccurate2_tiny, imageAccurate1_tiny,  size);
		performNewIdeaIteration(imageAccurate1_tiny, imageAccurate2_tiny,  size);
		performNewIdeaIteration(imageAccurate2_tiny, imageAccurate1_tiny,  size);
	
	
	
	AccurateImage *imageAccurate1_small = convertImageToNewFormat(image);
	AccurateImage *imageAccurate2_small = convertImageToNewFormat(image);
	
	// Process the small case:
	 size = 3;
		performNewIdeaIteration(imageAccurate2_small, imageAccurate1_small,  size);
		performNewIdeaIteration(imageAccurate1_small, imageAccurate2_small,  size);
		performNewIdeaIteration(imageAccurate2_small, imageAccurate1_small,  size);
		performNewIdeaIteration(imageAccurate1_small, imageAccurate2_small,  size);
		performNewIdeaIteration(imageAccurate2_small, imageAccurate1_small,  size);
	
	
	AccurateImage *imageAccurate1_medium = convertImageToNewFormat(image);
	AccurateImage *imageAccurate2_medium = convertImageToNewFormat(image);
	
	// Process the medium case:
	size = 5;
		performNewIdeaIteration(imageAccurate2_medium, imageAccurate1_medium,  size);
		performNewIdeaIteration(imageAccurate1_medium, imageAccurate2_medium,  size);
		performNewIdeaIteration(imageAccurate2_medium, imageAccurate1_medium,  size);
		performNewIdeaIteration(imageAccurate1_medium, imageAccurate2_medium,  size);
		performNewIdeaIteration(imageAccurate2_medium, imageAccurate1_medium,  size);
	
	
	AccurateImage *imageAccurate1_large = convertImageToNewFormat(image);
	AccurateImage *imageAccurate2_large = convertImageToNewFormat(image);
	
	// Do each color channel
	 size = 8;
		performNewIdeaIteration(imageAccurate2_large, imageAccurate1_large,  size);
		performNewIdeaIteration(imageAccurate1_large, imageAccurate2_large,  size);
		performNewIdeaIteration(imageAccurate2_large, imageAccurate1_large,  size);
		performNewIdeaIteration(imageAccurate1_large, imageAccurate2_large,  size);
		performNewIdeaIteration(imageAccurate2_large, imageAccurate1_large,  size);
	
	
	// Save the images.
	PPMImage *final_tiny = performNewIdeaFinalization(imageAccurate2_tiny,  imageAccurate2_small);
	PPMImage *final_small = performNewIdeaFinalization(imageAccurate2_small,  imageAccurate2_medium);
	PPMImage *final_medium = performNewIdeaFinalization(imageAccurate2_medium,  imageAccurate2_large);
	
	if(argc > 1) {
		writePPM("flower_tiny.ppm", final_tiny);
		writePPM("flower_small.ppm", final_small);
		writePPM("flower_medium.ppm", final_medium);
	} else {
		writeStreamPPM(stdout, final_tiny);
		writeStreamPPM(stdout, final_small);
		writeStreamPPM(stdout, final_medium);
	}
	return 0;
}

