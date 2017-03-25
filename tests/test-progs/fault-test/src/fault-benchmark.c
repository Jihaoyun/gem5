#include <stdio.h>

// Gem5 headers
#include "util/m5/m5op.h"

int main(int argc, char **argv) {

  m5_dumpreset_stats(0, 0);

  int volatile i = 0, j = 0, k = 0, z = 0, w = 0;

  for (z = 0; z < 1024; z++) {
    while (++i < 100) {
      for (j = 0; j < i; j++) {
        k++;
        if (k % 2 == 0)
          w++;
      }

      if (i % 2 == 0) {
        if (k % 2) {
          w++;
        }
        else if (j > k) {
          w++;
        }
      }
      k++;
    }
  }

  m5_dumpreset_stats(0, 0);

  return 0;
}
