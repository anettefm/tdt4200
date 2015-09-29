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
// Copy the image.
 AccurateImage *copyImage(AccurateImage *image) {
         // Make a copy
         AccurateImage *imageAccurate;
         imageAccurate = (AccurateImage *)malloc(sizeof(AccurateImage));
         imageAccurate->data = (AccuratePixel*)malloc(image->x * image->y * sizeof(AccuratePixel));
         for(int i = 0; i < image->x * image->y; i++) {
                 imageAccurate->data[i].red   =  image->data[i].red;
                 imageAccurate->data[i].green = image->data[i].green;
                 imageAccurate->data[i].blue  =  image->data[i].blue;
         }
         imageAccurate->x = image->x;
         imageAccurate->y = image->y;
 
         return imageAccurate;
}

// Perform the new idea:
void performNewIdeaIteration(AccurateImage *imageIn, AccurateImage *imageTemp, AccurateImage *imageTemp2,int size) {
	// Iterate over each pixel
	int size2=2*size+1;
	int numberOfValuesInEachRow = imageIn->x;
	
	for(int senterY=0; senterY<imageIn->y; senterY++){
		int offsetOfThePixel = (numberOfValuesInEachRow * senterY);
               	imageTemp->data[offsetOfThePixel].red = 0;
               	imageTemp->data[offsetOfThePixel].green =0;
               	imageTemp->data[offsetOfThePixel].blue =0;
		for(int i=0; i<=size; i++){
                        imageTemp->data[offsetOfThePixel].red +=imageIn->data[offsetOfThePixel +i].red ;
                        imageTemp->data[offsetOfThePixel].green +=imageIn->data[offsetOfThePixel +i].green;
                	imageTemp->data[offsetOfThePixel].blue+=imageIn->data[offsetOfThePixel +i].blue ;
                }
		for(int senterX=1; senterX<imageIn->x;senterX++){
                      	if(senterX+size>=size2 && senterX+size<imageIn->x){
                                imageTemp->data[offsetOfThePixel+senterX].red = imageTemp->data[offsetOfThePixel+senterX-1].red+imageIn->data[offsetOfThePixel +senterX+size].red-imageIn->data[offsetOfThePixel +senterX-size-1].red;
                                imageTemp->data[offsetOfThePixel + senterX].green  = imageTemp->data[offsetOfThePixel+senterX-1].green+imageIn->data[offsetOfThePixel +senterX+size].green-imageIn->data[offsetOfThePixel +senterX-size-1].green;
                                imageTemp->data[offsetOfThePixel + senterX].blue = imageTemp->data[offsetOfThePixel+senterX-1].blue+imageIn->data[offsetOfThePixel +senterX+size].blue - imageIn->data[offsetOfThePixel +senterX-size-1].blue;
			}else if(senterX+size<size2){
                        	imageTemp->data[offsetOfThePixel+senterX].red = imageTemp->data[offsetOfThePixel+senterX-1].red+imageIn->data[offsetOfThePixel +senterX+size].red;
                        	imageTemp->data[offsetOfThePixel + senterX].green  = imageTemp->data[offsetOfThePixel+senterX-1].green+imageIn->data[offsetOfThePixel +senterX+size].green;
                        	imageTemp->data[offsetOfThePixel + senterX].blue = imageTemp->data[offsetOfThePixel+senterX-1].blue+imageIn->data[offsetOfThePixel +senterX+size].blue;
			}else{
                                imageTemp->data[offsetOfThePixel+senterX].red = imageTemp->data[offsetOfThePixel+senterX-1].red-imageIn->data[offsetOfThePixel +senterX-size-1].red;
                                imageTemp->data[offsetOfThePixel + senterX].green  = imageTemp->data[offsetOfThePixel+senterX-1].green - imageIn->data[offsetOfThePixel +senterX-size-1].green;
                                imageTemp->data[offsetOfThePixel + senterX].blue = imageTemp->data[offsetOfThePixel+senterX-1].blue - imageIn->data[offsetOfThePixel +senterX-size-1].blue;	
			}		 
		}
        }
	float count;
       for(int senterX=0; senterX<imageIn->x; senterX++){
		 imageTemp2->data[senterX].red = 0;
                imageTemp2->data[senterX].green =0;
                imageTemp2->data[senterX].blue =0;
                for(int i=0; i<=size; i++){
             		int offsetOfThePixel = (numberOfValuesInEachRow * i)+senterX;
                        imageTemp2->data[senterX].red +=imageTemp->data[offsetOfThePixel].red ;
                        imageTemp2->data[senterX].green +=imageTemp->data[offsetOfThePixel].green;
                        imageTemp2->data[senterX].blue+=imageTemp->data[offsetOfThePixel].blue ;
		}
                         if (senterX>=size && senterX+size<imageIn->x){
                                 count=1.0/(size2*(size+1)); //mulig dette er feil
                         }else if(senterX<size){
                                 count=1.0/((size+senterX+1)*(1+size));
                         }else{
                                 count=1.0/((imageIn->x-senterX+size)*(1+size));
                         }
                imageIn->data[senterX].red =imageTemp2->data[senterX].red*count ;
                imageIn->data[senterX].green =imageTemp2->data[senterX].green*count;
                imageIn->data[senterX].blue=imageTemp2->data[senterX].blue*count ;
                
        }

	for(int senterY=1; senterY<=size; senterY++){
		for(int senterX=0; senterX<imageIn->x; senterX++){
			if (senterX>=size && senterX+size<imageIn->x){
				count=1.0/(size2*(senterY+size+1)); //mulig dette er feil
			}else if(senterX<size){
				count=1.0/((size+senterX+1)*(1+senterY+size));
			}else{
				count=1.0/((imageIn->x-senterX+size)*(1+senterY+size));
			}
			int offsetOfThePixel = (numberOfValuesInEachRow * senterY)+senterX;
                      	imageTemp2->data[offsetOfThePixel].red =imageTemp2->data[(numberOfValuesInEachRow *( senterY-1))+senterX].red + imageTemp->data[(numberOfValuesInEachRow * (senterY+size))+senterX].red ;
                        imageTemp2->data[offsetOfThePixel].green =imageTemp2->data[(numberOfValuesInEachRow * (senterY-1))+senterX].green + imageTemp->data[(numberOfValuesInEachRow * (senterY+size))+senterX].green ;
                        imageTemp2->data[offsetOfThePixel].blue =imageTemp2->data[(numberOfValuesInEachRow * (senterY-1))+senterX].blue + imageTemp->data[(numberOfValuesInEachRow *( senterY+size))+senterX].blue ;
			imageIn->data[offsetOfThePixel].red =imageTemp2->data[offsetOfThePixel].red*count ;
                        imageIn->data[offsetOfThePixel].green =imageTemp2->data[offsetOfThePixel].green*count;
                        imageIn->data[offsetOfThePixel].blue=imageTemp2->data[offsetOfThePixel].blue*count ;
			}
	}
	
	float count1=1.0/((size2*size2));
        for(int senterY=size+1; senterY<imageIn->y-size; senterY++){
                for(int senterX=0; senterX<imageIn->x; senterX++){
                        if (senterX>=size && senterX+size<imageIn->x){
                                count=count1; //mulig dette er feil
                        }else if(senterX<size){
                                count=1.0/((size+senterX+1)*(size2));
                        }else{
                                count=1.0/((imageIn->x-senterX+size)*(size2));
			}
                         int offsetOfThePixel = (numberOfValuesInEachRow * senterY)+senterX;
                         imageTemp2->data[offsetOfThePixel].red =imageTemp2->data[(numberOfValuesInEachRow *( senterY-1))+senterX].red + imageTemp->data[(numberOfValuesInEachRow * (senterY+size))+senterX].red-imageTemp->data[(numberOfValuesInEachRow * (senterY-size-1))+senterX].red ;
                        imageTemp2->data[offsetOfThePixel].green =imageTemp2->data[(numberOfValuesInEachRow * (senterY-1))+senterX].green + imageTemp->data[(numberOfValuesInEachRow * (senterY+size))+senterX].green -imageTemp->data[(numberOfValuesInEachRow * (senterY-size-1))+senterX].green ;
                         imageTemp2->data[offsetOfThePixel].blue =imageTemp2->data[(numberOfValuesInEachRow * (senterY-1))+senterX].blue + imageTemp->data[(numberOfValuesInEachRow *( senterY+size))+senterX].blue -imageTemp->data[(numberOfValuesInEachRow * (senterY-size-1))+senterX].blue ;

                        imageIn->data[offsetOfThePixel].red =imageTemp2->data[offsetOfThePixel].red*count ;
                        imageIn->data[offsetOfThePixel].green =imageTemp2->data[offsetOfThePixel].green*count;
                        imageIn->data[offsetOfThePixel].blue=imageTemp2->data[offsetOfThePixel].blue*count ;
                }
        }

        for(int senterY=imageIn->y-size; senterY<imageIn->y; senterY++){
                for(int senterX=0; senterX<imageIn->x; senterX++){
                        if (senterX>=size && senterX+size<imageIn->x)
                                count=1.0/(size2*(imageIn->y-senterY+size)); //mulig dette er feil
                        else if(senterX<size)
                                count=1.0/((size+senterX+1)*(imageIn->y-senterY+size));
                        else
                                count=1.0/((imageIn->x-senterX+size)*(imageIn->y-senterY+size));
                          int offsetOfThePixel = (numberOfValuesInEachRow * senterY)+senterX;                         
                          imageTemp2->data[offsetOfThePixel].red =imageTemp2->data[(numberOfValuesInEachRow *( senterY-1))+senterX].red -imageTemp->data[(numberOfValuesInEachRow * (senterY-size-1))+senterX].red ;
                         imageTemp2->data[offsetOfThePixel].green =imageTemp2->data[(numberOfValuesInEachRow * (senterY-1))+senterX].green -imageTemp->data[(numberOfValuesInEachRow * (senterY-size-1))+senterX].green ;
                         imageTemp2->data[offsetOfThePixel].blue =imageTemp2->data[(numberOfValuesInEachRow * (senterY-1))+senterX].blue -imageTemp->data[(numberOfValuesInEachRow * (senterY-size-1))+senterX].blue ;
                        imageIn->data[offsetOfThePixel].red =imageTemp2->data[offsetOfThePixel].red*count ;
                        imageIn->data[offsetOfThePixel].green =imageTemp2->data[offsetOfThePixel].green*count;
                        imageIn->data[offsetOfThePixel].blue=imageTemp2->data[offsetOfThePixel].blue*count ;
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
	
	AccurateImage *imageAccurate_temp1 = convertImageToNewFormat(image);
        AccurateImage *imageAccurate_temp2 = copyImage(imageAccurate_temp1);
	AccurateImage *imageAccurate_tiny = copyImage(imageAccurate_temp1);
	AccurateImage *imageAccurate_small =copyImage(imageAccurate_temp1);
        AccurateImage *imageAccurate_medium = copyImage(imageAccurate_temp1);
        AccurateImage *imageAccurate_large =copyImage(imageAccurate_temp1);


	// Process the tiny case:
		int size = 2; 
		performNewIdeaIteration(imageAccurate_tiny, imageAccurate_temp1, imageAccurate_temp2, size);
		performNewIdeaIteration(imageAccurate_tiny, imageAccurate_temp1, imageAccurate_temp2,  size);
		performNewIdeaIteration(imageAccurate_tiny, imageAccurate_temp1, imageAccurate_temp2, size);
		performNewIdeaIteration(imageAccurate_tiny, imageAccurate_temp1, imageAccurate_temp2, size);
		performNewIdeaIteration(imageAccurate_tiny, imageAccurate_temp1, imageAccurate_temp2, size);


	//AccurateImage *imageAccurate_small = convertImageToNewFormat(image);
	
	// Process the small case:
	 size = 3;
		performNewIdeaIteration(imageAccurate_small, imageAccurate_temp1, imageAccurate_temp2, size);
		performNewIdeaIteration(imageAccurate_small, imageAccurate_temp1, imageAccurate_temp2, size);
		performNewIdeaIteration(imageAccurate_small, imageAccurate_temp1, imageAccurate_temp2, size);
		performNewIdeaIteration(imageAccurate_small, imageAccurate_temp1, imageAccurate_temp2, size);
		performNewIdeaIteration(imageAccurate_small, imageAccurate_temp1, imageAccurate_temp2, size);
	
	
	//AccurateImage *imageAccurate_medium = convertImageToNewFormat(image);
	
	// Process the medium case:
	size = 5;
		performNewIdeaIteration(imageAccurate_medium, imageAccurate_temp1, imageAccurate_temp2, size);
		performNewIdeaIteration(imageAccurate_medium, imageAccurate_temp1, imageAccurate_temp2, size);
		performNewIdeaIteration(imageAccurate_medium, imageAccurate_temp1, imageAccurate_temp2, size);
		performNewIdeaIteration(imageAccurate_medium, imageAccurate_temp1, imageAccurate_temp2,size);
		performNewIdeaIteration(imageAccurate_medium, imageAccurate_temp1, imageAccurate_temp2, size);
	
	
//	AccurateImage *imageAccurate_large = convertImageToNewFormat(image);
	
	// Do each color channel
	 size = 8;
		performNewIdeaIteration(imageAccurate_large, imageAccurate_temp1, imageAccurate_temp2,  size);
		performNewIdeaIteration(imageAccurate_large, imageAccurate_temp1, imageAccurate_temp2, size);
		performNewIdeaIteration(imageAccurate_large, imageAccurate_temp1, imageAccurate_temp2, size);
		performNewIdeaIteration(imageAccurate_large, imageAccurate_temp1, imageAccurate_temp2, size);
		performNewIdeaIteration(imageAccurate_large, imageAccurate_temp1, imageAccurate_temp2, size);
	
	
	// Save the images.
	PPMImage *final_tiny = performNewIdeaFinalization(imageAccurate_tiny,  imageAccurate_small);
	PPMImage *final_small = performNewIdeaFinalization(imageAccurate_small,  imageAccurate_medium);
	PPMImage *final_medium = performNewIdeaFinalization(imageAccurate_medium,  imageAccurate_large);
	
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

