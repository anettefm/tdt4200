#include <stdio.h> // for stdin
#include <stdlib.h>
#include <unistd.h> // for ssize_t
#include <math.h>

#ifdef HAVE_MPI
#include <mpi.h>
#endif

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
#ifdef HAVE_MPI
	#ifdef HAVE_OPENMP
		MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &aquired);
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	#else
	MPI_Init(&argc, &argv);
	#endif 
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#else
	rank=0;
	size=1;
#endif
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
#ifdef HAVE_MPI
    MPI_Bcast(&amountOfRuns, 2, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(start, amountOfRuns, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(stop, amountOfRuns, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(numThreads, amountOfRuns, MPI_INT, 0, MPI_COMM_WORLD);

#endif


    for (int i=0; i<amountOfRuns; i++){
	int amountOfElements=(stop[i]-start[i])/size;
        int rest=(stop[i]-start[i])%size;
  //      printf("rest %d, start %d, stop %d\n", rest, start[i], stop[i]);
	if (rest>0){
        	if (rank<rest){
            		start[i]+=amountOfElements*rank+rank;
            		stop[i]=start[i]+amountOfElements;
     //           printf("1 %d, start %d, stop %d\n", rank, start[i], stop[i]);

        	}else{
            		start[i]+=amountOfElements*rank+rest;
            		stop[i]=start[i]+amountOfElements-1;
   //             printf("2 %d, start %d, stop %d\n", rank, start[i], stop[i]);

        	}
    }else{
        start[i]+=amountOfElements*rank;
        stop[i]=start[i]+amountOfElements-1;
 //               printf("3 %d, start %d, stop %d\n", rank, start[i], stop[i]);
    }
        int sum=0;
	int sum_glob=0;
#ifdef HAVE_OPENMP
	omp_set_num_threads(numThreads[i]);
#endif        
	for(int C=start[i]; C<=stop[i]; C++){
            int sn=C;
#ifdef HAVE_OPENMP
           #pragma omp parallel for schedule(static) reduction(+:sum) private(sn)
#endif
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
	//printf("%d %d\n",rank, sum);
       // printf("%d, %d, %d, %d, %d\n", i, rank, sum, start[i], stop[i]);
#ifdef HAVE_MPI
        MPI_Reduce(&sum,&sum_glob,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
        if (rank==0)
        printf("%d\n", sum_glob);
#else
	printf("%d\n", sum);
#endif

    }
#ifdef HAVE_MPI
MPI_Finalize();
#endif
	return 0;
}
