#include <stdio.h> // for stdin
#include <stdlib.h>
#include <unistd.h> // for ssize_t
#include <math.h>

//#ifdef HAVE_MPI
//#include <mpi.h>
//#endif

//#ifdef HAVE_OPENMP
//#include <omp.h>
//#endif
long gcd(int a, int b) {
    if (b == 0) {
        return a;
    }
    else {
        return gcd(b, a% b);
    }
}
int main() {
	char *inputLine = NULL; size_t lineLength = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    
    int elements_per_proc = n/size;
    
    
    int *start, *stop, *numThreads, amountOfRuns = 0;
    int k, n, rank, size, sm;
	// Read in first line of input
    
	getline(&inputLine, &lineLength, stdin);
	sscanf(inputLine, "%d", &amountOfRuns);

	stop = (int*) calloc(amountOfRuns, sizeof(int));
	start = (int*) calloc(amountOfRuns, sizeof(int));
	numThreads = (int*) calloc(amountOfRuns, sizeof(int));

    if(rank==0){
        for (int i = 0; i < amountOfRuns; ++i){
            int tot_threads, current_start, current_stop;
            // Read in each line of input that follows after first line
            free(inputLine); lineLength = 0; inputLine = NULL;
            ssize_t readChars = getline(&inputLine, &lineLength, stdin);

            // If there exists at least two matches (2x %d)...
            if (sscanf(inputLine, "%d %d %d", &current_start, &current_stop, &tot_threads) >= 2){
                if(current_start < 0 || current_stop < 0){
                    current_start = 0, current_stop = 0;
                }
            stop[i] = current_stop;
			start[i] = current_start;
			numThreads[i] = tot_threads;
            }
        }
        MPI_Bcast(start, amountOfRuns, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(stop, amountOfRuns, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(numThreads, amountOfRuns, MPI_INT, 0, MPI_COMM_WORLD);
    }


	/*
	*	Remember to only print 1 (one) sum per start/stop.
	*	In other words, a total of <amountOfRuns> sums/printfs.
	*/
    for (int i=0; i<amountOfRuns; i++){
        if (rank<amountOfRuns%size){
            start[i]+=rank;
            stop[i]+=rank+1;
        }else{
            start[i]+=rank;
            stop[i]+=rank;
        }
        int sum=0;
        for(int C=start[i]; C<stop[i]; C++){
            int sn=C;
            
            #pragma omp parallel for schedule(static) reduction(+:sum)
            for(sm=1; sm<C/2; sm++){
                sn=C-sm;
                int A=sn-sm;
                if(fmod(sqrt(sn),1.f)==0 && fmod(sqrt(sm), 1.f)==0){
                    int B=2*sqrt(sn)*sqrt(sm);
                    if(gcd(A,B)==1 && gcd(B,C)==1 && gcd(A,C)==1){
                        sum++;}
                }
            }
        }
        MPI_Reduce(&sum,sum,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
        printf("%d\n", sum);
    }

	return 0;
}
