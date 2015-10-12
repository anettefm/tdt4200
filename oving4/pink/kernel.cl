
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
    float lengthsq
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

		float vec1=((float)y - (lineinfo[i].y1 + lineinfo[i].thickness / 2.0 * (1 - lineinfo[i].dy))) * ((float)y - (lineinfo[i].y1 + lineinfo[i].thickness / 2.0 * (1 - lineinfo[i].dy))) + ((float)x - (lineinfo[i].x1 + lineinfo[i].thickness / 2.0 * lineinfo[i].dy)) * ((float)x - (lineinfo[i].x1 + lineinfo[i].thickness / 2.0 * lineinfo[i].dy));
		float vec2=((float)y - (lineinfo[i].y1 - lineinfo[i].thickness / 2.0 * (1 - lineinfo[i].dy))) * ((float)y - (lineinfo[i].y1 - lineinfo[i].thickness / 2.0 * (1 - lineinfo[i].dy))) + ((float)x - (lineinfo[i].x1 - lineinfo[i].thickness / 2.0 * lineinfo[i].dy)) * ((float)x - (lineinfo[i].x1 - lineinfo[i].thickness / 2.0 * lineinfo[i].dy));
        float vec3=((float)y - (lineinfo[i].y2 + lineinfo[i].thickness / 2.0 * (1 - lineinfo[i].dy))) * ((float)y - (lineinfo[i].y2 + lineinfo[i].thickness / 2.0 * (1 - lineinfo[i].dy))) + ((float)x - (lineinfo[i].x2 + lineinfo[i].thickness / 2.0 * lineinfo[i].dy)) * ((float)x - (lineinfo[i].x2 + lineinfo[i].thickness / 2.0 * lineinfo[i].dy));
        float vec4=((float)y - (lineinfo[i].y2 - lineinfo[i].thickness / 2.0 * (1 - lineinfo[i].dy))) * ((float)y - (lineinfo[i].y2 - lineinfo[i].thickness/2.0 * (1 - lineinfo[i].dy))) + ((float)x - (lineinfo[i].x2 - lineinfo[i].thickness / 2.0 * lineinfo[i].dy)) * ((float)x - (lineinfo[i].x2 - lineinfo[i].thickness / 2.0 * lineinfo[i].dy));

        float cos1=vec1 + vec2 - lineinfo[i].thickness*lineinfo[i].thickness;
		float cos2=vec2 + vec3 - lineinfo[i].lengthsq;
        float cos3=vec3 + vec4 - lineinfo[i].thickness*lineinfo[i].thickness;
        float cos4=vec4 + vec1 - lineinfo[i].lengthsq;
		if (cos1>=0 && cos2>=0 && cos3>=0 && cos4>=0){
			image[b*3*x+3*y]=i+20;
			image[b*3*x+3*y+1]=255;
			image[b*3*x+3*y+2]=255;
		}

		
		
	}
    }


