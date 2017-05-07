susan: susan.c Makefile
	mkdir -p bin
	alphaev67-unknown-linux-gnu-gcc -static -O4 -o bin/susan susan.c -lm
	gcc -static -O4 -o bin/susan.x86 susan.c -lm
clean:
	rm -rf bin
