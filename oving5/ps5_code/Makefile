.PHONY: clean

CC:=gcc
CFLAGS:=--std=c99 -O2

sout: seriel
	 ./$^ <input.txt --std=c99 -O2

mout2: MPI
	mpirun -n 2 ./$^ <input.txt

mout1: MPI
	mpirun -n 1 ./$^ <input.txt
	

moout2: MPIomp
	 mpirun -n 2 ./$^ <input.txt

moout1: MPIomp
	 mpirun -n 1 ./$^ <input.txt

oout: omp
	 ./$^ <input.txt


seriel: pythagoreanTriplets.c
	gcc $^ --std=c99 -o $@ -lm

MPI: pythagoreanTriplets.c
	mpicc $^ --std=c99 -o $@ -lm -DHAVE_MPI

omp: pythagoreanTriplets.c
	gcc $^ --std=c99 -o $@ -lm -fopenmp -DHAVE_OPENMP

MPIomp: pythagoreanTriplets.c
	mpicc $^ --std=c99 -o $@ -lm -fopenmp -DHAVE_OPENMP -DHAVE_MPI


clean:
	rm -f *.o program 
