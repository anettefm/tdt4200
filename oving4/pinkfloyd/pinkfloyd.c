#include <OpenCL/opencl.h> // maa endres til CL/opencl.h
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <tgmath.h>

#include "lodepng.h"


struct Color{
	float angle;
	float intensity;
};

struct CircleInfo{
	float x;
	float y;
	float radius;
	struct Color color;
};

struct LineInfo{
	float x1,y1;
	float x2,y2;
	float thickness;
	struct Color color;
};

char * readText( const char * filename){
	FILE * file = fopen( filename, "r");
	fseek( file, 0, SEEK_END);
	size_t length = ftell(file);
	(void) fseek( file, 0L, SEEK_SET);
	char * content = calloc( length+1, sizeof(char));
	int itemsread = fread( content, sizeof(char), length, file);
	if ( itemsread != length ) {
		printf("Error, reeadText(const char *), failed to read file");
		exit(1);
	}
	return content;
}


void parseLine(char * line, struct LineInfo li[], cl_int *lines){
	float x1,x2,y1,y2,thickness, angle, intensity;
	int items = sscanf(line, "line %f,%f %f,%f %f %f,%f", &x1, &y1, &x2, &y2, &thickness, &angle, &intensity);
	if ( 7 == items ){
		li[*lines].x1 = x1;
		li[*lines].x2 = x2;
		li[*lines].y1 = y1;
		li[*lines].y2 = y2;
		li[*lines].thickness = thickness;
		li[*lines].color.angle = angle;
		li[*lines].color.intensity = intensity;
		(*lines)++;
	}
}


void parseCircle(char * line, struct CircleInfo ci[], cl_int *circles){
	float x,y,radius;
	struct Color c;
	int items = sscanf(line, "circle %f,%f %f %f,%f", &x,&y,&radius, &c.angle, &c.intensity);
	if ( 5==items){
		ci[*circles].x = x;
		ci[*circles].y = y;
		ci[*circles].radius = radius;
		ci[*circles].color.angle = c.angle;
		ci[*circles].color.intensity = c.intensity;
		(*circles)++;
	}
}


void printLines(struct LineInfo li[], cl_int lines){
	for ( int i = 0 ; i < lines ; i++){
		printf("line:	from:%f,%f to:%f,%f thick:%f,	%f,%f\n", li[i].x1, li[i].y1, li[i].x2, li[i].y2, li[i].thickness,li[i].color.angle, li[i].color.intensity);
	}
}


void printCircles(struct CircleInfo ci[], cl_int circles){
	for ( int i = 0 ; i < circles ; i++){
		printf("circle %f,%f %f %f,%f\n", ci[i].x,ci[i].y,ci[i].radius, ci[i].color.angle, ci[i].color.intensity);
	}
}


int main(){
	cl_device_id device_id = NULL;
    	cl_command_queue command_queue = NULL;
    	cl_mem memobj_image = NULL;
        cl_mem memobj_i = NULL;
//        cl_mem memobj_lin = NULL;
    	cl_kernel kernel = NULL;
    	cl_platform_id platform_id = NULL;
    	cl_uint ret_num_devices;
    	cl_uint ret_num_platforms;
	
	// Parse input
	int numberOfInstructions;
	char* *instructions = NULL;
	size_t *instructionLengths;

	struct CircleInfo *circleinfo;
	cl_int circles = 0;
	struct LineInfo *lineinfo;
	cl_int lines = 0;
printf("tut\n");	
	FILE *stdin;
	stdin=fopen("input_tdsotm.txt", "r");
	char *line = NULL;
	size_t linelen = 0;
	int width=0, height = 0;
	ssize_t read = getline( & line, &linelen, stdin);

	// Read size of canvas
	sscanf( line, "%d,%d" , &width,&height);
	read = getline( & line, &linelen, stdin);
printf("%d, %d \n", width, height);
	// Read amount of primitives
	sscanf( line, "%d" , & numberOfInstructions);

	// Allocate memory for primitives
	instructions = calloc(sizeof(char*),numberOfInstructions);
	instructionLengths = calloc( sizeof(size_t), numberOfInstructions);
	circleinfo = calloc( sizeof(struct CircleInfo), numberOfInstructions);
	lineinfo = calloc( sizeof(struct LineInfo), numberOfInstructions);


	// Read in each primitive
	for ( int i =0 ; i < numberOfInstructions; i++){
		ssize_t read = getline( &instructions[i] , &instructionLengths[i] , stdin);
		/*Read in the line or circle here*/
	}
fclose(stdin);
	printf("tut\n");	
	unsigned char* image = (unsigned char*)calloc(width*height*3, sizeof(unsigned char));



	// Build OpenCL program (more is needed, before and after the below code)
	char * source = readText("kernel.cl");
	cl_context context;
	cl_int error_cl;

        /* Get Platform and Device Info */
        error_cl = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
        error_cl = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);

        /* Create OpenCL context */
        context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &error_cl);

        /* Create Command Queue */
        command_queue = clCreateCommandQueue(context, device_id, 0, &error_cl);
printf("1\n");
        /* Create Memory Buffer */
       memobj_image = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(image), NULL, &error_cl);
       memobj_i = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int), NULL, &error_cl);
//	memobj_cir = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(circleinfo), NULL, &error_cl);
//        memobj_lin = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(lineinfo), NULL, &error_cl);
printf("a\n");	
//	error_cl = clEnqueueWriteBuffer(command_queue, memobj_cir, CL_TRUE, 0, sizeof(circleinfo), circleinfo, 0, NULL, NULL);
  //      error_cl = clEnqueueWriteBuffer(command_queue, memobj_lin, CL_TRUE, 0, sizeof(lineinfo), lineinfo, 0, NULL, NULL);
int a=2;

      error_cl = clEnqueueWriteBuffer(command_queue, memobj_i, CL_TRUE, 0, sizeof(int),(void *) a, 0, NULL, NULL);
printf("a\n");



	cl_program program = clCreateProgramWithSource(context, 1, (const char **) &source, NULL, &error_cl);
printf("b\n");
    
    
	// Check if OpenCL function invocation failed/succeeded
	if ( !context ) {
		printf( "Error, failed to create program. \n");
		return 1;
	}
	error_cl = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);


	/* Create data paralell OpenCL kernel*/
        // Create Kernel / transfer data to device
	kernel = clCreateKernel(program, "canvas", &error_cl);

        error_cl = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&memobj_image);
        error_cl = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&memobj_i);
//	error_cl = clSetKernelArg(kernel, 1, sizeof(circleinfo), (void *)&memobj_cir);
//        error_cl = clSetKernelArg(kernel, 2, sizeof(lineinfo), (void *)&memobj_lin);
	printf("hei\n");
	size_t global_item_size=1;
	size_t local_item_size=1;
        // Execute Kernel / transfer result back from device
	error_cl = clEnqueueNDRangeKernel( command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);
int i=0;
         printf("hei %d\n", i);
	error_cl = clEnqueueReadBuffer(command_queue, memobj_image, CL_TRUE, 0, sizeof(image), &image, 0, NULL, NULL);
         error_cl = clEnqueueReadBuffer(command_queue, memobj_i, CL_TRUE, 0, sizeof(int), &i, 0, NULL, NULL);	
	printf("hei %d\n", 5);
	
	size_t memfile_length = 0;
	unsigned char * memfile = NULL;
	lodepng_encode24(&memfile, &memfile_length, image ,width, height);
printf("tit\n");
	// KEEP THIS LINE. Or make damn sure you replace it with something equivalent.
	// This "prints" your png to stdout, permitting I/O redirection
	fwrite( memfile, sizeof(unsigned char), memfile_length, stdout);

	return 0;
}
