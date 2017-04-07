#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UNLIMIT
#define MAXARRAY 60000 /* this number, if too large, will cause a seg. fault!! */
#define LINE 128

int compare(const void *elem1, const void *elem2)
{
  int result;

  result = strcmp((char *)elem1, (char *)elem2);

  return (result < 0) ? 1 : ((result == 0) ? 0 : -1);
}


int main(int argc, char *argv[]) {
  char array[MAXARRAY][LINE];
  FILE *fp;
  int i,count=0;

  if (argc<3) {
    fprintf(stderr,"Usage: qsort_small <inp_file> <out_file>\n");
    exit(-1);
  }
  else {
    fp = fopen(argv[1],"r");

    while ((fscanf(fp, "%s", &array[count][0]) == 1) && (count < MAXARRAY)) {
      count++;
    }
  }

  qsort(array, count, sizeof(char)*LINE, compare);

  fclose(fp);
  fp = fopen(argv[2], "w");

  for (i=0;i<count;i++)
  {
    fprintf(fp, "%s\n", array[i]);
  }
  fclose(fp);

  return 0;
}
