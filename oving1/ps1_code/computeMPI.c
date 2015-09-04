#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>

/*
	A simple MPI example.
	TODO:
	1. Fill in the needed MPI code to make this run on any number of nodes.
	2. The answer must match the original serial version.
	3. Think of corner cases (valid but tricky values).

	Example input:
	./simple 2 10000

*/

int main(int argc, char **argv) {
	int rank, size; /* denne prosessorens rank*/ /* antall prosessorer*/

	
	if (argc < 3) {
		printf("This program requires two parameters:\n \
the start and end specifying a range of positive integers in which \
start is 2 or greater, and end is greater than start.\n");
		exit(1);
	}

	int start = atoi(argv[1]);
	int stop = atoi(argv[2]);


    
	if(start < 2 || stop <= start){
		printf("Start must be greater than 2 and the end must be larger than start.\n");
		exit(1);
	}

	// TODO: Compute the local range, so that all the elements are accounted for.
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    
    int local_range;
    local_range=(stop-start)/size;


	// Perform the computation
	double sum = 0.0;
	double rest;
 	double tot_sum;
    if(rank==size-1){
	    rest=(stop-start)% size;
    }else{
        rest=0;
    }
    for (int i = start+rank*local_range; i < start+(rank+1)*local_range+rest; i++) {
		sum += 1.0/log(i);
    }


	// Debug prints if needed
	printf("The proc: %d\n The sum is: %f\n", rank, sum);

	// TODO: Get the sum of each node's partial answer
	// Use MPI_Send and MPI_Recv here
    if(rank != 0){
        MPI_Send(&sum, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }else{
        tot_sum=sum;
        for(int source=1; source<size; source++){
            MPI_Recv(&sum, 1, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            tot_sum+=sum;
        }
        
    }
	// TODO: Print the global sum once only
    if(rank == 0){
	printf("The total sum is: %f\n", tot_sum);
	}
    MPI_Finalize();
	return 0;
}

