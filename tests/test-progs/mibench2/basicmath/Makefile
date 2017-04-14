FILE1 = basicmath_small.c   rad2deg.c  cubic.c   isqrt.c
FILE2 = basicmath_large.c   rad2deg.c  cubic.c   isqrt.c

all: basicmath_small basicmath_large

basicmath_small: ${FILE1} Makefile
	mkdir -p bin
	gcc -static  ${FILE1} -o bin/basicmath_small.x86 -lm
	alphaev67-unknown-linux-gnu-gcc -static  ${FILE1} -o bin/basicmath_small -lm
basicmath_large: ${FILE2} Makefile
	mkdir -p bin
	gcc -static  ${FILE2} -g -o bin/basicmath_large.x86 -lm
	alphaev67-unknown-linux-gnu-gcc -static  ${FILE2} -o bin/basicmath_large -lm
clean:
	rm -rf bin
