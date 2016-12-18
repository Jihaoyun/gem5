#include <stdio.h>
#include <stdlib.h>

int main() {

        int i;
                int aa,bb;
        for ( i = 0; i < 1024; i++ ) {

                        if ( i % 2 == 0 ) {
                                aa = 3;
                                bb = 4;
                        }
                        else {
                                aa = 2;
                                bb = 2;
                        }


                        if ( aa == 2 )
                                aa = 0;
                        if ( bb == 2 )
                                bb = 0;
                        if ( aa == bb )
                                printf("Pippo");

                }

        return 0;
}
