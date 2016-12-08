#include <stdio.h>

int main()
{
  char v[1024];
  int i;

  printf("BEFORE\n");

  for (i=0; i<1024; i++)
    v[i] = 0xFF;

  printf("AFTER\n");

  return 0;
}
