/* +++Date last modified: 05-Jul-1997 */

/*
**  BITCNTS.C - Test program for bit counting functions
**
**  public domain by Bob Stout & Auke Reitsma
*/

#include <float.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "bitops.h"
#include "conio.h"

#define FUNCS  7

static int CDECL bit_shifter(long int x);

int main(int argc, char *argv[])
{
  clock_t start, stop;
  double ct, cmin = DBL_MAX, cmax = 0;
  int i, cminix, cmaxix;
  long j, n, seed;
  int iterations;
  static int (* CDECL pBitCntFunc[FUNCS])(long) = {
    bit_count,
    bitcount,
    ntbl_bitcnt,
    ntbl_bitcount,
    /*            btbl_bitcnt, DOESNT WORK*/
    BW_btbl_bitcount,
    AR_btbl_bitcount,
    bit_shifter
  };

  if (argc<3) {
    fprintf(stderr,"Usage: bitcnts <iterations> <out_file>\n");
    exit(-1);
  }
  iterations=atoi(argv[1]);

  FILE *fp;
  fp = fopen(argv[2], "w");

  for (i = 0; i < FUNCS; i++) {
    for (j = n = 0, seed = rand(); j < iterations; j++, seed += 13)
      n += pBitCntFunc[i](seed);
    fprintf(fp, "%ld\n", n);
  }

  fclose(fp);

  return 0;
}

static int CDECL bit_shifter(long int x)
{
  int i, n;

  for (i = n = 0; x && (i < (sizeof(long) * CHAR_BIT)); ++i, x >>= 1)
    n += (int)(x & 1L);
  return n;
}
