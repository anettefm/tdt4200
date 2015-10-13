.PHONY: clean

CC:=gcc
libs:=-framework OpenCL
CFLAGS:=--std=c99 -O2

image.png: program
	./$<<input_tdsotm.txt > $@

program: pinkfloyd.o lodepng.o
	$(CC) $^ -o $@ $(libs)

clean:
	rm -f *.o program image.png
