
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
	image[b*3*x+3*y]=255;
	image[b*3*x+3*y+1]=255;
	image[b*3*x+3*y+2]=255;
	
	for (int i=0; i<1; i++){

		float dy1=((float)y-(lineinfo[i].y1+lineinfo[i].thickness/2.0*(1-lineinfo[i].dy)))/((float)x-(lineinfo[i].x1+lineinfo[i].thickness/2.0*lineinfo[i].dy));
		float dy2=((float)y-(lineinfo[i].y1-lineinfo[i].thickness/2.0*(1-lineinfo[i].dy)))/((float)x-(lineinfo[i].x1-lineinfo[i].thickness/2.0*lineinfo[i].dy));

        float dy3=((float)y-(lineinfo[i].y2+lineinfo[i].thickness/2.0*(1-lineinfo[i].dy)))/((float)x-(lineinfo[i].x2+lineinfo[i].thickness/2.0*lineinfo[i].dy));
        float dy4=((float)y-(lineinfo[i].y2-lineinfo[i].thickness/2.0*(1-lineinfo[i].dy)))/((float)x-(lineinfo[i].x2-lineinfo[i].thickness/2.0*lineinfo[i].dy));

        float cos1=dy1*dy1+dy2*dy2-lineinfo[i].thickness*lineinfo[i].thickness;
		float cos2=dy2*dy2+dy3*dy3-lineinfo[i].dy*lineinfo[i].dy;
        float cos3=dy3*dy3+dy4*dy4-lineinfo[i].thickness*lineinfo[i].thickness;
        float cos4=dy4*dy4+dy1*dy1-lineinfo[i].dy*lineinfo[i].dy;
		if (cos1>=0 && cos2>=0 && cos3>=0 && cos4>=0){
			image[b*3*x+3*y]=i+20;
			image[b*3*x+3*y+1]=255;
			image[b*3*x+3*y+2]=255;
		}

		
		
	}
    }


