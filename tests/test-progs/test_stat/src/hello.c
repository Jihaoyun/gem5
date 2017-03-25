#include <stdio.h>

#include "util/m5/m5op.h"

int main()
{
  m5_dumpreset_stats(0, 0);
  printf("Hola Mundo!\n");
  m5_dumpreset_stats(0, 0);

  return 0;
}
