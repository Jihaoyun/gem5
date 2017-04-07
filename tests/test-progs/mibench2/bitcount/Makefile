FILE = bitcnt_1.c bitcnt_2.c bitcnt_3.c bitcnt_4.c bitcnts.c bitfiles.c bitstrng.c bstr_i.c

bitcnts: ${FILE} Makefile
	alphaev67-unknown-linux-gnu-gcc -static ${FILE} -g -o bin/bitcnts
	gcc ${FILE} -g -o bin/bitcnts.x86

clean:
	rm -rf bitcnts output*
