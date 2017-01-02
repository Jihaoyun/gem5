#include <stdio.h>
#include <stdlib.h>

void check_malloc(void *p);

int main(int argc, char **argv)
{
  FILE *fp;

  unsigned int m, n;  //m x n - Matrix A dimension
  unsigned int p;     //n x p - Matrix B dmension
                      //m x c - Matrix C dimension
  int **A;            //A Matrix
  int **B;            //B Matrix
  int **C;            //C Matrix
  int i,j,k;

  if (argc < 3)
  {
    fprintf(stderr, "Parameters error!\nUsage: "
                    "./matmul <inp_file> <out_file>\n");
    return -1;
  }

  if ((fp = fopen(argv[1], "r")) == NULL)
  {
    fprintf(stderr, "Can't open file %s\n", argv[1]);
    return -2;
  }

  //Read A Matrix
  fscanf(fp, "%d %d", &m, &n);
  A = malloc(m*sizeof(int *));
  check_malloc((void *)A);
  for (i=0; i<m; i++)
  {
    A[i] = malloc(n*sizeof(int));
    check_malloc((void *)A[i]);
    for (j=0; j<n; j++)
    {
      fscanf(fp, "%d", &A[i][j]);
    }
  }

  //Read B Matrix
  fscanf(fp, "%*d %d", &p);
  B = malloc(n*sizeof(int *));
  check_malloc((void *)B);
  for (i=0; i<n; i++)
  {
    B[i] = malloc(p*sizeof(int));
    check_malloc((void *)B[i]);
    for (j=0; j<p; j++)
    {
      fscanf(fp, "%d", &B[i][j]);
    }
  }
  fclose(fp);

  //Allocate C Matrix
  C = malloc(m*sizeof(int *));
  check_malloc((void *) C);
  for (i=0; i<m; i++)
  {
    C[i] = malloc(p*sizeof(int));
    check_malloc((void *)C[i]);
  }

  //Product matrix computation
  for (i=0; i<m; i++)
  {
    for (j=0; j<p; j++)
    {
      C[i][j] = 0;
      for (k=0; k<n; k++)
      {
        C[i][j] += A[i][k]*B[k][j];
      }
    }
  }

  if ((fp = fopen(argv[2], "w")) == NULL)
  {
    fprintf(stderr, "Can't create output file %s\n",
                    argv[2]);
    return -4;
  }

  for (i=0; i<m; i++)
  {
    for (j=0; j<p; j++)
    {
      fprintf(fp, "%d ", C[i][j]);
    }
    fprintf(fp, "\n");
  }

  fclose(fp);

  //Deallocate memory
  for (i=0; i<m; i++)
  {
    free(A[i]);
    free(C[i]);
  }
  free(A);
  free(C);
  for (i=0; i<n; i++)
    free(B[i]);
  free(B);
  return 0;
}

void check_malloc(void *p)
{
  if (p == NULL)
  {
    fprintf(stderr, "Can't malloc!\n");
    exit(-3);
  }
}
