FILE1 = qsort_small.c
FILE2 = qsort_large.c

all: qsort_small qsort_large

qsort_small: qsort_small.c Makefile
	mkdir -p bin
	gcc -static qsort_small.c -O3 -g -o bin/qsort_small.x86 -lm
	alphaev67-unknown-linux-gnu-gcc -static qsort_small.c -O3 -o bin/qsort_small -lm
qsort_large: qsort_large.c Makefile
	mkdir -p bin
	gcc -static qsort_large.c -O3 -o bin/qsort_large.x86 -lm
	alphaev67-unknown-linux-gnu-gcc -static qsort_large.c -O3 -o bin/qsort_large -lm
clean:
	rm -rf bin
