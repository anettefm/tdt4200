#include <OpenCL/opencl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <tgmath.h>

#include "lodepng.h"

  #define MAX_SOURCE_SIZE (0x100000)

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
    cl_context context = NULL;
    cl_command_queue command_queue = NULL;
    cl_mem memobj = NULL;
    cl_program program = NULL;
    cl_kernel kernel = NULL;
    cl_platform_id platform_id = NULL;
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_int ret;
    
    
    
    
    // Parse input
	int numberOfInstructions;
	char* *instructions = NULL;
	size_t *instructionLengths;

	struct CircleInfo *circleinfo;
	cl_int circles = 0;
	struct LineInfo *lineinfo;
	cl_int lines = 0;

    FILE *stdin;
    stdin=fopen("input_tdsotm.txt", "r");
    
	char *line = NULL;
	size_t linelen = 0;
	int width=0, height = 0;
	ssize_t read = getline( & line, &linelen, stdin);

	// Read size of canvas
	sscanf( line, "%d,%d" , &width,&height);
	read = getline( & line, &linelen, stdin);

	// Read amount of primitives
	sscanf( line, "%d" , & numberOfInstructions);
    printf("hei");
	// Allocate memory for primitives
	instructions = calloc(sizeof(char*),numberOfInstructions);
	instructionLengths = calloc( sizeof(size_t), numberOfInstructions);
	circleinfo = calloc( sizeof(struct CircleInfo), numberOfInstructions);
	lineinfo = calloc( sizeof(struct LineInfo), numberOfInstructions);

	// Read in each primitive
	for ( int i =0 ; i < numberOfInstructions; i++){
		ssize_t read = getline( &instructions[i] , &instructionLengths[i] , stdin);
        parseLine(line, lineinfo, &lines);
        parseCircle(line, circleinfo , &circles);
    }
    fclose(stdin);

	// Build OpenCL program (more is needed, before and after the below code)
//	char * source = readText("kernel.cl");
    

    /* Get Platform and Device Info */
    ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);

    /* Create OpenCL context */
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

    /* Create Command Queue */
    command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

    /* Create Memory Buffer */
    memobj = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int), NULL, &ret);


    char * source = readText("kernel.cl");
    program = clCreateProgramWithSource(context, 1,(const char **) &source,NULL, &ret);
    
    // Check if OpenCL function invocation failed/succeeded
    if ( !context ) {
        printf( "Error, failed to create program. \n");
        return 1;
    }
    /* Build Kernel Program */
printf("%d\n", ret);
    ret = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
        printf("a%d\n", ret);
    
    if (ret != CL_SUCCESS){
        
        size_t len;
        
        char buffer[2048];
        
        
        
        printf("Error: Failed to build program executable!\n");
        
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        printf("%s\n", buffer);
        
        exit(1);
        
    }
    /* Create OpenCL Kernel */
    kernel = clCreateKernel(program, "canvas", &ret);
    
    /* Set OpenCL Kernel Parameters */
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&memobj);
    
    /* Execute OpenCL Kernel */
    ret = clEnqueueTask(command_queue, kernel, 0, NULL,NULL);
    int a=NULL;
    /* Copy results from the memory buffer */
    ret = clEnqueueReadBuffer(command_queue, memobj, CL_TRUE, 0, sizeof(int), (void *)a, 0, NULL, NULL);
    printf("%d\n", ret);
    
    // Remember that more is needed before OpenCL can create kernel

	// Create Kernel / transfer data to device

	// Execute Kernel / transfer result back from device

	size_t memfile_length = 0;
	unsigned char * memfile = NULL;
/*	lodepng_encode24(
		&memfile,
		&memfile_length,
		image,
	width,
		height);*/

	// KEEP THIS LINE. Or make damn sure you replace it with something equivalent.
	// This "prints" your png to stdout, permitting I/O redirection
	fwrite( memfile, sizeof(unsigned char), memfile_length, stdout);
    /* Finalization */
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(memobj);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);

    return 0;
}
