.PHONY: clean

CC:=gcc
CFLAGS:=--std=c99 -O2

sout: seriel
	 ./$^ <input.txt --std=c99 -O2

mout: MPI
	 mpirun -n 4 ./$^ <input.txt

moout: MPIomp
	 mpirun -n 2 ./$^ <input.txt

oout: omp
	 ./$^ <input.txt


seriel: pythagoreanTriplets.c
	gcc $^ --std=c99 -o $@ -lm

MPI: pythagoreanTriplets.c
	mpicc $^ --std=c99 -o $@ -lm -DHAVE_MPI

omp: pythagoreanTriplets.c
	gcc $^ --std=c99 -o $@ -lm -fopenmp -DHAVE_OPENMP

MPIomp: pythagoreanTriplets.c
	mpicc $^ --std=c99 -o $@ -lm -fopenmp -DHAVE_OPENMP -DAHVE_MPI


clean:
	rm -f *.o program 
