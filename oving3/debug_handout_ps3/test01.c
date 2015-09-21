#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	char *mem = malloc(100);
	strcpy(mem,  "Hello\n");
	printf("%s", mem);
	
	free(mem);
	
	return 0;
}
