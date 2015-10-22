#include <stdio.h> // for stdin
#include <stdlib.h>
#include <unistd.h> // for ssize_t
#include <math.h>

#ifdef HAVE_OPENMP
#include <omp.h>
#endif
long gcd(int a, int b) {
    if (b == 0) {
        return a;
    }
    else {
        return gcd(b, a% b);
    }
}


int main( int argc, char **argv ) {
    int *start, *stop, *numThreads, aquired, amountOfRuns;
    int rank, size, sm;

	rank=0;
	size=1;

	char *inputLine = NULL; size_t lineLength = 0;
    
    

	// Read in first line of input
    
	getline(&inputLine, &lineLength, stdin);
	sscanf(inputLine, "%d", &amountOfRuns);

	stop = (int*) calloc(amountOfRuns, sizeof(int));
	start = (int*) calloc(amountOfRuns, sizeof(int));
	numThreads = (int*) calloc(amountOfRuns, sizeof(int));

    if(rank==0){
        for (int i = 0; i < amountOfRuns; ++i){
            int tot_threads=NULL;
	    int current_start, current_stop;
            // Read in each line of input that follows after first line
            free(inputLine); lineLength = 0; inputLine = NULL;
            ssize_t readChars = getline(&inputLine, &lineLength, stdin);

            // If there exists at least two matches (2x %d)...
		int ret=sscanf(inputLine, "%d %d %d", &current_start, &current_stop, &tot_threads); 
            if (ret >= 2){
                if(current_start < 0 || current_stop < 0){
                    current_start = 0, current_stop = 0;
                }
            	stop[i] = current_stop;
		start[i] = current_start;
		if(ret==EOF)
			tot_threads=1;		
		numThreads[i] = tot_threads;
            }
        }

    }


	/*
	*	Remember to only print 1 (one) sum per start/stop.
	*	In other words, a total of <amountOfRuns> sums/printfs.
	*/
    for (int i=0; i<amountOfRuns; i++){
	int amountOfElements=(stop[i]-start[i])/size;
	if (amountOfElements%size>0){
        	if (rank<amountOfElements%size){
            		start[i]+=rank;
            		stop[i]+=amountOfElements+1;
        	}else{
            		start[i]+=rank;
            		stop[i]+=amountOfElements;
        	}
	}

        int sum=0;
	int sum_glob=0;

	omp_set_num_threads(numThreads[i]);
        
	for(int C=start[i]; C<stop[i]; C++){
            int sn=C;

           #pragma omp parallel for schedule(static) reduction(+:sum) private(sn)

            for(sm=1; sm<C/2; sm++){
                sn=C-sm;
                int A=sn-sm;
		float m=sqrt(sn);
		float n=sqrt(sm);
                if(fmod(n,1.f)==0 && fmod(m, 1.f)==0){
                    int B=2*n*m;
                    if(gcd(A,B)==1 && gcd(B,C)==1 && gcd(A,C)==1){
                        sum++;}
                }
		
            }
        }

	printf("%d\n", sum);


    }

	return 0;
}
