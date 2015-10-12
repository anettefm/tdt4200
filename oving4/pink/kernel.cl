
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



__kernel void make_canvas(__global unsigned char *image, int h, int b, __global struct LineInfo *lineinfo, int lines)
    {
	int x=get_global_id(0);
	int y=get_global_id(1);
	image[b*3*y+3*x]=255;
	image[b*3*y+3*x+1]=255;
	image[b*3*y+3*x+2]=255;
	
	for (int i=0; i<lines; i++){
float x1_ned= lineinfo[i].x1 + lineinfo[i].thickness / 2.0 * lineinfo[i].dy;
float x1_opp= lineinfo[i].x1 - lineinfo[i].thickness / 2.0 * lineinfo[i].dy;	
float x2_ned= lineinfo[i].x2 + lineinfo[i].thickness / 2.0 * lineinfo[i].dy;
float x2_opp= lineinfo[i].x2 - lineinfo[i].thickness / 2.0 * lineinfo[i].dy;
float y1_opp= lineinfo[i].y1 - lineinfo[i].thickness / 2.0 * (1 - lineinfo[i].dy);
float y1_ned= lineinfo[i].y1 + lineinfo[i].thickness / 2.0 * (1 - lineinfo[i].dy);
float y2_opp= lineinfo[i].y2 - lineinfo[i].thickness / 2.0 * (1 - lineinfo[i].dy);
float y2_ned= lineinfo[i].y2 + lineinfo[i].thickness / 2.0 * (1 - lineinfo[i].dy);


float vdotu1=(x-x1_opp)*(lineinfo[i].x1-x1_opp)+(y-y1_opp)*(lineinfo[i].y1-y1_opp);
float vdotu2=(x-lineinfo[i].x1)*(x1_ned-lineinfo[i].x1)+(y1_ned-lineinfo[i].y1)*(y1_ned-lineinfo[i].y1);

float vcrossu1= sqrt((x-x1_opp)*(x-x1_opp)+(y-y1_opp)*(y-y1_opp))*sqrt((lineinfo[i].x1-x1_opp)*(lineinfo[i].x1-x1_opp)+(lineinfo[i].y1-y1_opp)*(lineinfo[i].y1-y1_opp));
float vcrossu2= sqrt((x-lineinfo[i].x1)*(x-lineinfo[i].x1)+(y-lineinfo[i].y1)*(y-lineinfo[i].y1))*sqrt((x1_ned-lineinfo[i].x1)*(x1_ned-lineinfo[i].x1)+(y1_ned-lineinfo[i].y1)*(y1_ned-lineinfo[i].y1));
float ang1=vdotu1/vcrossu1;
float ang2=vdotu2/vcrossu2;
		if (ang1*ang2>=0){
			image[b*3*y+3*x]=0;
			image[b*3*y+3*x+1]=0;
			image[b*3*y+3*x+2]=0;
		}

	
		
	}
    }


