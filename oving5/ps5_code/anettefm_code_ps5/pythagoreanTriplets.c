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

/**
Finner største felles multiplum
**/
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

// initsialiserer for MPI og MPI og openMP.
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


	// Leser fra fil og sprer informasjonen til alle prosessorene om MPI brukes. Informasjonen spres med å bruke MPI_Bcast().
    if(rank==0){
	getline(&inputLine, &lineLength, stdin);
	sscanf(inputLine, "%d", &amountOfRuns);
	}
 
#ifdef HAVE_MPI
        MPI_Bcast(&amountOfRuns, 1, MPI_INT, 0, MPI_COMM_WORLD);
#endif   
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
    MPI_Bcast(start, amountOfRuns, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(stop, amountOfRuns, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(numThreads, amountOfRuns, MPI_INT, 0, MPI_COMM_WORLD);
#endif



// Looper gjennom slik at antall primitive pytagoreiske tripletter telles for et og et problem av gangen. 
    for (int i=0; i<amountOfRuns; i++){

     	int sum=0;
    	int sum_glob=0;
#ifdef HAVE_OPENMP
	omp_set_num_threads(numThreads[i]);
#endif        

// Finner antall primitive pytagoreiske tripletter med å bruke at A=n²-m², B=2nm og C=m²+n². Siden det er C som må være innenfor intervallet settes C til å være start og økes med en for hver iterasjon. for hver iterasjon settes sn(som er n²) til initielt å være C. I loopen innenfor finnes sn og sm(m²). sm får 1 som startverdi og for hver iterasjon økes den med 1 mer til den like stor som sn. sn finnes ved å trekke sn fra C. Deretter regnes A og B ut og det testes om verdiene er primitive pytagoreiske tripletter, er de det økes sum med en. 
	for(int C=start[i]+rank; C<stop[i]; C+=size){
            int sn=C;

           #pragma omp parallel for schedule(static) reduction(+:sum) private(sn)

            for(sm=1; sm<C/2; sm++){
                sn=C-sm;
                int A=sn-sm;
		float m=sqrt(sn);
		float n=sqrt(sm);
                if(fmod(n,1.f)==0 && fmod(m, 1.f)==0){
                    int B=2*n*m;
                    if(gcd(A,B)==1){
                        sum++;}
                }
		
            }
        }
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

/** 
Siden det ikke skulleleveres noen pdf dokumenterer jeg speedupen her:
Testet tiden til koden. 
Ser at det er speed up ved både MPI og openMP med tanke på seriel kode. Og speedup fra bare MPI eller openMP til hybrid.

Prosessorer: 1 tråder: 1
real    0m7.361s 
user    0m7.335s 
sys     0m0.020s 

Prosessorer: 2 tråder: 1
real    0m4.776s 
user    0m7.471s 
sys     0m0.028s

Prosessorer: 1 tråder: 2
real    0m3.642s 
user    0m7.198s 
sys     0m0.032s

Prosessorer: 2 tråder: 2
real    0m2.990s 
user    0m7.748s 
sys     0m0.021s 
**/
