#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {

    FILE* fi;
    FILE* fo;
    int n,i;
    int data;
    int* debug_mem;


    fi = fopen(argv[1],"r");
    fo = fopen(argv[2],"w");
    n = atoi(argv[3]);

    debug_mem = (int* ) malloc(n*sizeof(int));

    for ( i = 0; i < n; i++)
        debug_mem[i] = 0;

    while ( fscanf(fi,"%x", &data) == 1  ) {

        data = (data >> 21) & 0x7F;
        debug_mem[data]++;

    }

    for ( i = 0; i < n; i++ )
        fprintf(fo,"%d %d\n", debug_mem[i], i);

    return 0;

}
