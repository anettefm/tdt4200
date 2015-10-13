#include <openCL/opencl.h>
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
	float dy;
    float lengthsq;
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
		fprintf(stderr,"Error, reeadText(const char *), failed to read file");
		exit(1);
	}
	return content;
}


void parseLine(char * line, struct LineInfo li[], cl_int *lines, int h, int b){
	float x1,x2,y1,y2,thickness, angle, intensity;
	int items = sscanf(line, "line %f,%f %f,%f %f %f,%f", &x1, &y1, &x2, &y2, &thickness, &angle, &intensity);
	if ( 7 == items ){

		li[*lines].x1 = x1*b;
		li[*lines].x2 = x2*b;
		li[*lines].y1 = y1*h;
		li[*lines].y2 = y2*h;
		li[*lines].thickness = thickness*h;
		li[*lines].color.angle = angle;
		li[*lines].color.intensity = intensity;
		li[*lines].dy=-(y2-y1)/(x2-x1);
        	li[*lines].lengthsq=sqrt((li[*lines].y2-li[*lines].y1)*(li[*lines].y2-li[*lines].y1)+(li[*lines].x2-li[*lines].x1)*(li[*lines].x2-li[*lines].x1));
		(*lines)++;
	}
}


void parseCircle(char * line, struct CircleInfo ci[], cl_int *circles, int h, int b){
	float x,y,radius;
	struct Color c;
	int items = sscanf(line, "circle %f,%f %f %f,%f", &x,&y,&radius, &c.angle, &c.intensity);
	if ( 5==items){
		ci[*circles].x = x*b;
		ci[*circles].y = y*h;
		ci[*circles].radius = radius*h;
		ci[*circles].color.angle = c.angle;
		ci[*circles].color.intensity = c.intensity;
		(*circles)++;
	}
}


void printLines(struct LineInfo li[], cl_int lines){
	for ( int i = 0 ; i < lines ; i++){
		fprintf(stderr,"line:	from:%f,%f to:%f,%f thick:%f,	%f,%f\n", li[i].x1, li[i].y1, li[i].x2, li[i].y2, li[i].thickness,li[i].color.angle, li[i].color.intensity);
	}
}


void printCircles(struct CircleInfo ci[], cl_int circles){
	for ( int i = 0 ; i < circles ; i++){
		fprintf(stderr,"circle %f,%f %f %f,%f\n", ci[i].x,ci[i].y,ci[i].radius, ci[i].color.angle, ci[i].color.intensity);
	}
}


int main(){
	cl_device_id device_id = NULL;
	cl_context context = NULL;
	cl_command_queue command_queue = NULL;
	cl_program program = NULL;
	cl_kernel kernel = NULL;
	cl_platform_id platform_id = NULL;
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;
	cl_int ret=CL_SUCCESS;

	cl_int memobj_h;
	cl_int memobj_b;
    cl_mem memobj_image;
	cl_mem memobj_line;
    cl_mem memobj_circle;
	// Parse input
	int numberOfInstructions;
	char* *instructions = NULL;
	size_t *instructionLengths;
	struct CircleInfo *circleinfo;
	cl_int circles = 0;
	struct LineInfo *lineinfo;
	cl_int lines = 0;

	char *line = NULL;
	size_t linelen = 0;
	int width=0, height = 0;
	ssize_t read = getline( & line, &linelen, stdin);
		
	// Read size of canvas
	sscanf( line, "%d,%d" , &width,&height);
	read = getline( & line, &linelen, stdin);

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
		parseCircle(instructions[i], circleinfo, &circles, height, width);
		parseLine(instructions[i], lineinfo, &lines, height, width);
	}

	ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);

	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &ret_num_devices);

	/* Create OpenCL context */
	context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

	/* Create Command Queue */
	command_queue = clCreateCommandQueue(context, device_id, 0, &ret);


	// Build OpenCL program (more is needed, before and after the below code)
	char * source = readText("kernel.cl");
	program = clCreateProgramWithSource(context, 1,(const char **) &source,NULL, &ret);
	// Check if OpenCL function invocation failed/succeeded
	if ( !context ) {
		fprintf(stderr, "Error, failed to create program. \n");
		return 1;
	}

	/* Build Kernel Program */
	ret = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);

    if (ret !=CL_SUCCESS){
        size_t len;
        char buffer[2048];      
        fprintf(stderr,"Error: Failed to build program executable!\n");
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        fprintf(stderr,"%s\n", buffer);
        exit(1);
    }


    /* Create OpenCL Kernel */
    kernel = clCreateKernel(program, "make_canvas", &ret);
    if (ret !=CL_SUCCESS){
        size_t len;
        char buffer[2048];      
        fprintf(stderr,"Error: Failed to create kernel!\n");
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        fprintf(stderr,"%s\n", buffer);
        exit(1);
    }

	unsigned char* image=(unsigned char*)malloc(height*width*3*sizeof(unsigned char));
	memobj_b=width;
	memobj_h=height;
	cl_int mem_lines=lines;
    cl_int mem_circles=circles;
    /* Create Memory Buffer */
    memobj_image = clCreateBuffer(context, CL_MEM_READ_WRITE, 3*height*width*sizeof(unsigned char), NULL, &ret);

    memobj_line = clCreateBuffer(context, CL_MEM_READ_WRITE, lines*sizeof(struct LineInfo), NULL, &ret);
    memobj_circle = clCreateBuffer(context, CL_MEM_READ_WRITE, circles*sizeof(struct CircleInfo), NULL, &ret);

	ret=clEnqueueWriteBuffer(command_queue, memobj_line, CL_TRUE, 0, lines*sizeof(struct LineInfo), lineinfo, 0, NULL, NULL); 
	ret=clEnqueueWriteBuffer(command_queue, memobj_circle, CL_TRUE, 0, circles*sizeof(struct CircleInfo), circleinfo, 0, NULL, NULL);
    	/* Set OpenCL Kernel Parameters */
    	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&memobj_image);
    	ret = clSetKernelArg(kernel, 1, sizeof(int), &memobj_h);
    	ret = clSetKernelArg(kernel, 2, sizeof(int), &memobj_b);
    ret = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&memobj_circle);
    ret = clSetKernelArg(kernel, 4, sizeof(int), &mem_circles);
	ret = clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *)&memobj_line);
  	ret = clSetKernelArg(kernel, 6, sizeof(int), &mem_lines);
	
	const size_t g_ws[2] ={  height, width} ;

	
    /* Execute OpenCL Kernel */
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 2,NULL,  &g_ws , NULL , 0 , NULL , NULL );
    if (ret !=CL_SUCCESS){
        size_t len;
        char buffer[2048];      
        fprintf(stderr,"Error: Failed to run kernel!\n");
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        fprintf(stderr, "%s\n", buffer);
        exit(1);
    }

    /* Copy results from the memory buffer */
    ret = clEnqueueReadBuffer(command_queue, memobj_image, CL_TRUE, 0, width*height*3*sizeof(unsigned char), image, 0, NULL, NULL);



    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);


	size_t memfile_length = 0;
	unsigned char * memfile = NULL;
	lodepng_encode24(&memfile, &memfile_length, image ,width, height);

	// KEEP THIS LINE. Or make damn sure you replace it with something equivalent.
	// This "prints" your png to stdout, permitting I/O redirection
	fwrite( memfile, sizeof(unsigned char), memfile_length, stdout);

	return 0;
}
