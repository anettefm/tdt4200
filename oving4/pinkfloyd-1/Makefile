.PHONY: clean

CC:=gcc
libs:=-framework opencl
CFLAGS:=--std=c99 -O2

image.png: program
	./program

program: pinkfloyd.o lodepng.o
	$(CC) $^ -o $@ $(libs)

clean:
	rm -f *.o program image.png
