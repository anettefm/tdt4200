.PHONY: clean

CC:=gcc
CFLAGS:=--std=c99 -O2

out.txt: program
	./$<<input.txt > $@

program: pythagoreanTriplets.c
	$(CC) $^ -o $@

clean:
	rm -f *.o program out.txt
