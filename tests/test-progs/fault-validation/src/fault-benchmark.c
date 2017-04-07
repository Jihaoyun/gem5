#include <stdio.h>

// Gem5 headers
#include "util/m5/m5op.h"

int main(int argc, char **argv) {



  int i;
  int add = 0;


  m5_dumpreset_stats(0, 0);
  for (i = 0; i < 1024; i++) {
      if (  i % 2 == 0 )
        add++;
      else
        add--;
    }

  m5_dumpreset_stats(0, 0);
  return 0;
}
