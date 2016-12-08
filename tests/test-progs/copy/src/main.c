#include <stdio.h>
#include <stdlib.h>

int main() {

        int i;
        int vec[1024];
        int sum = 0;

        for ( i = 0; i < 1024; i++ )
                vec[i] = 255;
        for ( i = 0; i < 10; i++ ) {
                sum += vec[i];
        }


        printf("%d\n",sum);

        return 0;
}
