.PHONY: clean all run run_serial run_par

START:=2
STOP:=10000000
PROCESSES:=4

all: serial par

run: all
	$(MAKE) run_serial
	$(MAKE) run_par	

serial: computeSerial.c
	gcc -o $@ computeSerial.c -std=c99 -lm -O2

run_serial: serial
	./serial $(START) $(STOP)

par: computeMPI.c
	mpicc -o $@ computeMPI.c -std=c99 -lm -O2

run_par: par
	mpirun -np $(PROCESSES) ./par $(START) $(STOP)

clean:
	rm -f serial
