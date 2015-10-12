
// Write any additional support functions as well as the actual kernel function in this file

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

float red( float deg ) {
	float a1 = 1.f/60;
	float b1 = 2;
	float a2 = -1.f/60;
	float b2 = 2;
	float asc = deg*a2+b2;
	float desc = deg*a1+b1;
	return fmax( .0f , fmin( 1.f, fmin(asc,desc)));
}

float green( float deg ) {
	float a1 = 1.f/60;
	float b1 = 0;
	float a2 = -1.f/60;
	float b2 = 4;
	float asc = deg*a2+b2;
	float desc  = deg*a1+b1;
	return fmax( .0f , fmin( 1.f, fmin(asc,desc)));
}

float blue( float deg ) {
	float a1 = 1.f/60;
	float b1 = -2;
	float a2 = -1.f/60;
	float b2 = 6;
	float asc = deg*a2+b2;
	float desc = deg*a1+b1;
	return fmax( .0f , fmin( 1.f, fmin(asc,desc)));
}



__kernel void make_canvas(__global unsigned char *image, int h, int b, __global struct LineInfo *lineinfo, int lines){
	int x=get_global_id(0);
	int y=get_global_id(1);
	image[b*3*y+3*x]=0;
	image[b*3*y+3*x+1]=0;
	image[b*3*y+3*x+2]=0;



	
	for (int i=0; i<lines; i++){

		float cros=(lineinfo[i].x2-lineinfo[i].x1)*(lineinfo[i].x2-lineinfo[i].x1)+(lineinfo[i].y2-lineinfo[i].y1)*(lineinfo[i].y2-lineinfo[i].y1);
		float len= ((x-lineinfo[i].x1)*(lineinfo[i].x2-lineinfo[i].x1)+(y-lineinfo[i].y1)*(lineinfo[i].y2-lineinfo[i].y1))/cros;
		if (len>=0.0 && len<=1.0){
			float2 proj;
			proj.x=lineinfo[i].x1+len*(lineinfo[i].x2-lineinfo[i].x1);
			proj.y=lineinfo[i].y1+len*(lineinfo[i].y2-lineinfo[i].y1);
			float dist=(x-proj.x)*(x-proj.x)+(y-proj.y)*(y-proj.y);
			if(sqrt(dist)<=lineinfo[i].thickness){
				image[b*3*y+3*x]+=red(lineinfo[i].color.angle)*lineinfo[i].color.intensity;
				image[b*3*y+3*x+1]+=green(lineinfo[i].color.angle)*lineinfo[i].color.intensity;
				image[b*3*y+3*x+2]+=blue(lineinfo[i].color.angle)*lineinfo[i].color.intensity;
			}
		}
	}
}


