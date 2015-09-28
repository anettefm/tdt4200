#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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

// Perform the new idea:
void performNewIdeaIteration(AccurateImage *imageOut1, AccurateImage *imageIn, AccurateImage *imageTemp,int size) {
	// Iterate over each pixel
	int size2=2*size+1;
	int numberOfValuesInEachRow = imageIn->x;
	
	for(int senterY=0; senterY<imageIn->x; senterY++){
		int offsetOfThePixel = (numberOfValuesInEachRow * senterY);
               	imageTemp->data[offsetOfThePixel+senterX].red = 0;
               	imageTemp->data[offsetOfThePixel + senterX].green =0;
               	imageTemp->data[offsetOfThePixel + senterX].blue =0;
               	for(int i=0; i<size; i++){
                        imageTemp->data[offsetOfThePixel].red +=imageIn->data[offsetOfThePixel +i].red ;
                        imageTemp->data[offsetOfThePixel].green +=imageIn->data[offsetOfThePixel +i].green;
                	imageTemp->data[offsetOfThePixel].blue+=imageIn->data[offsetOfThePixel +i].blue ;
                }
		for(int senterX=0; senterX<imageIn->x;senterX++){
                      	if(senterX+size>=size2 && senterX+size<imageIn->x){
                                imageTemp->data[offsetOfThePixel+senterX].red = imageTemp->data[offsetOfThePixel+senterX-1].red+imageIn->data[offsetOfThePixel +senterX+size].red-imageIn->data[offsetOfThePixel +senterX-size].red;
                                imageTemp->data[offsetOfThePixel + senterX].green  = imageTemp->data[offsetOfThePixel+senterX-1].green+imageIn->data[offsetOfThePixel +senterX+size].green-imageIn->data[offsetOfThePixel +senterX-size].green;
                                imageTemp->data[offsetOfThePixel + senterX].blue = imageTemp->data[offsetOfThePixel+senterX-1].blue+imageIn->data[offsetOfThePixel +senterX+size].blue - imageIn->data[offsetOfThePixel +senterX-size].blue;
			}else if(senterX+size<size2){
                        	imageTemp->data[offsetOfThePixel+senterX].red = imageTemp->data[offsetOfThePixel+senterX-1].red+imageIn->data[offsetOfThePixel +senterX+size].red;
                        	imageTemp->data[offsetOfThePixel + senterX].green  = imageTemp->data[offsetOfThePixel+senterX-1].green+imageIn->data[offsetOfThePixel +senterX+size].green;
                        	imageTemp->data[offsetOfThePixel + senterX].blue = imageTemp->data[offsetOfThePixel+senterX-1].blue+imageIn->data[offsetOfThePixel +senterX+size].blue;
			}else{
                                imageTemp->data[offsetOfThePixel+senterX].red = imageTemp->data[offsetOfThePixel+senterX-1].red-imageIn->data[offsetOfThePixel +senterX-size].red;
                                imageTemp->data[offsetOfThePixel + senterX].green  = imageTemp->data[offsetOfThePixel+senterX-1].green - imageIn->data[offsetOfThePixel +senterX-size].green;
                                imageTemp->data[offsetOfThePixel + senterX].blue = imageTemp->data[offsetOfThePixel+senterX-1].blue - imageIn->data[offsetOfThePixel +senterX-size].blue;	
			}
							 
		}
        }
{
ta[offsetOfThePixel].blue;
                                        }
                                }
                                for(int i=size; i<size2; i++){
                                        sum_red[i]=0;
                                        sum_green[i]=0;
                                        sum_blue[i]=0;
                                }
                                temp_red=0;
                                temp_green=0;
                                temp_blue=0;
                                count=size;
                                value_red=0.0;
                                value_green=0.0;
                                value_blue=0.0;
                                for(int i=0; i<size2 ;i++){
                                        value_red+=sum_red[i];
                                        value_green+=sum_green[i];
                                        value_blue+=sum_blue[i];
                                }
                                county=size+1;
                                for(int j=senterX-size; j<senterX+size+1; j++){
                                        int offsetOfThePixel = (numberOfValuesInEachRow * (senterY + count) + j);
                                        sum_red[count]+= imageIn->data[offsetOfThePixel].red;
                                        sum_green[count]+= imageIn->data[offsetOfThePixel].green;
                                        sum_blue[count]+= imageIn->data[offsetOfThePixel].blue;
                                }
                        } else if(senterY+size>imageIn->y-1){
                                temp_red=sum_red[count];
                                temp_green=sum_green[count];
                                temp_blue=sum_blue[count];
                                sum_red[count]=0;
                                sum_green[count]=0;
                                sum_blue[count]=0;
                                county--;
                        } else{
                                temp_red=sum_red[count];
                                temp_green=sum_green[count];
                                temp_blue=sum_blue[count];
				sum_red[count]=0;
				sum_green[count]=0;
                                sum_blue[count]=0;
                                for(int j=senterX-size; j<senterX+size+1; j++){
                                        int offsetOfThePixel = (numberOfValuesInEachRow * (senterY + size) + j);
                                        sum_red[count]+= imageIn->data[offsetOfThePixel].red;
                                        sum_green[count]+= imageIn->data[offsetOfThePixel].green;
                                        sum_blue[count]+= imageIn->data[offsetOfThePixel].blue;
                                }
                                if(county<size2)
                                        county++;
                        }

                        value_red=value_red+sum_red[count]-temp_red;
                        value_green=value_green+sum_green[count]-temp_green;
                        value_blue=value_blue+sum_blue[count]-temp_blue;

                        int offsetOfThePixel = (numberOfValuesInEachRow * senterY + senterX);
                        imageOut1->data[offsetOfThePixel].red = value_red/(county*size2);
                        imageOut1->data[offsetOfThePixel].green = value_green/(county*size2);
                        imageOut1->data[offsetOfThePixel].blue = value_blue/(county*size2);			

			count++;
                        if(count>=size2){
                                count=0;
                        }
                        
                }

        }

        for(int senterX=imageIn->x-size; senterX<imageIn->x; senterX++){
                int count=0;
                double county=0;
		double countx=imageIn->x-senterX+size;
                for(int senterY=0; senterY<imageIn->y;senterY++){
                        if( senterY==0){
                                for(int i=0; i<size; i++){
					sum_red[i]=0;
					sum_green[i]=0;
					sum_blue[i]=0;
                                        for(int j=senterX-size; j<imageIn->x; j++){
                                                int offsetOfThePixel = (numberOfValuesInEachRow * (senterY + i) + j);
						sum_red[i]+= imageIn->data[offsetOfThePixel].red;
                                                sum_green[i]+= imageIn->data[offsetOfThePixel].green;
                                                sum_blue[i]+= imageIn->data[offsetOfThePixel].blue;
                                        }
                                }
                                count=size;
				county=size+1;
                                for(int i=size; i<size2; i++){
                                        sum_red[i]=0;
					sum_green[i]=0;
					sum_blue[i]=0;
                                }
                                temp_red=0;
                                temp_green=0;
                                temp_blue=0;
                                value_red=0.0;
                                value_green=0.0;
                                value_blue=0.0;
                                for(int i=0; i<size2 ;i++){
                                        value_red+=sum_red[i];
                                        value_green+=sum_green[i];
                                        value_blue+=sum_blue[i];
                                }
                                for(int j=senterX-size; j<imageIn->x; j++){
                                        int offsetOfThePixel = (numberOfValuesInEachRow * (senterY + count) + j);
                                        sum_red[count]+= imageIn->data[offsetOfThePixel].red;
                                        sum_green[count]+= imageIn->data[offsetOfThePixel].green;
                                        sum_blue[count]+= imageIn->data[offsetOfThePixel].blue;
                                }
                        } else if(senterY+size>=imageIn->y){
                                temp_red=sum_red[count];
                                temp_green=sum_green[count];
                                temp_blue=sum_blue[count];                                
				sum_red[count]=0;
				sum_green[count]=0;
				sum_blue[count]=0;
                                county--;
                        } else{
                                temp_red=sum_red[count];
                                temp_green=sum_green[count];
                                temp_blue=sum_blue[count];
				sum_red[count]=0;
                                sum_green[count]=0;
                                sum_blue[count]=0;
                                for(int j=senterX-size; j<imageIn->x; j++){
                                        int offsetOfThePixel = (numberOfValuesInEachRow * (senterY + size) + j);
                                        sum_red[count]+= imageIn->data[offsetOfThePixel].red;
                                        sum_green[count]+= imageIn->data[offsetOfThePixel].green;
                                        sum_blue[count]+= imageIn->data[offsetOfThePixel].blue;
                                }
                                if(county<size2)
                                        county++;
				
                        }

                        value_red=value_red+sum_red[count]-temp_red;
                        value_green=value_green+sum_green[count]-temp_green;
                        value_blue=value_blue+sum_blue[count]-temp_blue;

                        int offsetOfThePixel = (numberOfValuesInEachRow * senterY + senterX);
                        imageOut1->data[offsetOfThePixel].red = value_red/((county)*countx);
                        imageOut1->data[offsetOfThePixel].green = value_green/(county*countx);
                        imageOut1->data[offsetOfThePixel].blue = value_blue/(county*countx);

			count++;
			if(count>=size2){
                                count=0;
                        }
                        
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
	
	AccurateImage *imageAccurate_temp = convertImageToNewFormat(image);

	AccurateImage *imageAccurate1_tiny = convertImageToNewFormat(image);
	AccurateImage *imageAccurate2_tiny = convertImageToNewFormat(image);

	// Process the tiny case:
		int size = 2; 
		performNewIdeaIteration(imageAccurate2_tiny, imageAccurate1_tiny, imageAccurate_temp, size);
		performNewIdeaIteration(imageAccurate1_tiny, imageAccurate2_tiny, imageAccurate_temp, size);
		performNewIdeaIteration(imageAccurate2_tiny, imageAccurate1_tiny, imageAccurate_temp, size);
		performNewIdeaIteration(imageAccurate1_tiny, imageAccurate2_tiny, imageAccurate_temp, size);
		performNewIdeaIteration(imageAccurate2_tiny, imageAccurate1_tiny, imageAccurate_temp, size);
	
	
	AccurateImage *imageAccurate1_small = convertImageToNewFormat(image);
	AccurateImage *imageAccurate2_small = convertImageToNewFormat(image);
	
	// Process the small case:
	 size = 3;
		performNewIdeaIteration(imageAccurate2_small, imageAccurate1_small, imageAccurate_temp, size);
		performNewIdeaIteration(imageAccurate1_small, imageAccurate2_small, imageAccurate_temp, size);
		performNewIdeaIteration(imageAccurate2_small, imageAccurate1_small, imageAccurate_temp, size);
		performNewIdeaIteration(imageAccurate1_small, imageAccurate2_small, imageAccurate_temp, size);
		performNewIdeaIteration(imageAccurate2_small, imageAccurate1_small, imageAccurate_temp, size);
	
	
	AccurateImage *imageAccurate1_medium = convertImageToNewFormat(image);
	AccurateImage *imageAccurate2_medium = convertImageToNewFormat(image);
	
	// Process the medium case:
	size = 5;
		performNewIdeaIteration(imageAccurate2_medium, imageAccurate1_medium, imageAccurate_temp, size);
		performNewIdeaIteration(imageAccurate1_medium, imageAccurate2_medium, imageAccurate_temp, size);
		performNewIdeaIteration(imageAccurate2_medium, imageAccurate1_medium, imageAccurate_temp, size);
		performNewIdeaIteration(imageAccurate1_medium, imageAccurate2_medium, imageAccurate_temp,size);
		performNewIdeaIteration(imageAccurate2_medium, imageAccurate1_medium, imageAccurate_temp, size);
	
	
	AccurateImage *imageAccurate1_large = convertImageToNewFormat(image);
	AccurateImage *imageAccurate2_large = convertImageToNewFormat(image);
	
	// Do each color channel
	 size = 8;
		performNewIdeaIteration(imageAccurate2_large, imageAccurate1_large, imageAccurate_temp,  size);
		performNewIdeaIteration(imageAccurate1_large, imageAccurate2_large, imageAccurate_temp, size);
		performNewIdeaIteration(imageAccurate2_large, imageAccurate1_large, imageAccurate_temp, size);
		performNewIdeaIteration(imageAccurate1_large, imageAccurate2_large, imageAccurate_temp, size);
		performNewIdeaIteration(imageAccurate2_large, imageAccurate1_large, imageAccurate_temp, size);
	
	
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

