#include <stdio.h>
#include <stdlib.h>

void check_malloc(void *p);
void mul(int A[][10], int B[][10], int C[][10],
unsigned int m, unsigned int p, unsigned int n);

int main(int argc, char **argv)
{
  FILE *fp;

  unsigned int m, n;  //m x n - Matrix A dimension
  unsigned int p;     //n x p - Matrix B dmension
                      //m x c - Matrix C dimension
  int A[10][10];            //A Matrix
  int B[10][10];            //B Matrix
  int C[10][10];            //C Matrix
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
  for (i=0; i<m; i++)
  {
    for (j=0; j<n; j++)
    {
      fscanf(fp, "%d", &A[i][j]);
    }
  }

  //Read B Matrix
  fscanf(fp, "%*d %d", &p);
  for (i=0; i<n; i++)
  {
    for (j=0; j<p; j++)
    {
      fscanf(fp, "%d", &B[i][j]);
    }
  }
  fclose(fp);

  //Product matrix computation
  mul(A,B,C,m,p,n);

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

  return 0;
}


void mul(int A[][10], int B[][10], int C[][10],
unsigned int m, unsigned int p, unsigned int n) {

    int i,j,k;
    for (i=0; i<m; i++) {
        for (j=0; j<p; j++) {
            C[i][j] = 0;
            for (k=0; k<n; k++) {
                C[i][j] += A[i][k]*B[k][j];
            }
        }
    }
}

void check_malloc(void *p)
{
  if (p == NULL)
  {
    fprintf(stderr, "Can't malloc!\n");
    exit(-3);
  }
}
