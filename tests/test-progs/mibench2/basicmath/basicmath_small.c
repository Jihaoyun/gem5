#include <stdio.h>
#include "snipmath.h"
#include <math.h>

/* The printf's may be removed to isolate just the math calculations */

int main(int argc, char **argv)
{
  double  a1 = 1.0, b1 = -10.5, c1 = 32.0, d1 = -30.0;
  double  a2 = 1.0, b2 = -4.5, c2 = 17.0, d2 = -30.0;
  double  a3 = 1.0, b3 = -3.5, c3 = 22.0, d3 = -31.0;
  double  a4 = 1.0, b4 = -13.7, c4 = 1.0, d4 = -35.0;
  double  x[3];
  double X;
  int     solutions;
  int i;
  unsigned long l = 0x3fed0169L;
  struct int_sqrt q;
  long n = 0;

  FILE *fp;
  fp = fopen(argv[1], "w");
  if(fp == NULL){
    fprintf(fp ,"Can't create file\n");
    return -1;
  }

  /* solve soem cubic functions */
  fprintf(fp ,"********* CUBIC FUNCTIONS ***********\n");
  /* should get 3 solutions: 2, 6 & 2.5   */
  SolveCubic(a1, b1, c1, d1, &solutions, x);  
  fprintf(fp ,"Solutions:");
  for(i=0;i<solutions;i++)
    fprintf(fp ," %f",x[i]);
  fprintf(fp ,"\n");
  /* should get 1 solution: 2.5           */
  SolveCubic(a2, b2, c2, d2, &solutions, x);  
  fprintf(fp ,"Solutions:");
  for(i=0;i<solutions;i++)
    fprintf(fp ," %f",x[i]);
  fprintf(fp ,"\n");
  SolveCubic(a3, b3, c3, d3, &solutions, x);
  fprintf(fp ,"Solutions:");
  for(i=0;i<solutions;i++)
    fprintf(fp ," %f",x[i]);
  fprintf(fp ,"\n");
  SolveCubic(a4, b4, c4, d4, &solutions, x);
  fprintf(fp ,"Solutions:");
  for(i=0;i<solutions;i++)
    fprintf(fp ," %f",x[i]);
  fprintf(fp ,"\n");
  /* Now solve some random equations */
  for(a1=1;a1<10;a1++) {
    for(b1=10;b1>0;b1--) {
      for(c1=5;c1<15;c1+=0.5) {
	for(d1=-1;d1>-11;d1--) {
	  SolveCubic(a1, b1, c1, d1, &solutions, x);  
	  fprintf(fp ,"Solutions:");
	  for(i=0;i<solutions;i++)
	    fprintf(fp ," %f",x[i]);
	  fprintf(fp ,"\n");
	}
      }
    }
  }
  
  fprintf(fp ,"********* INTEGER SQR ROOTS ***********\n");
  /* perform some integer square roots */
  for (i = 0; i < 1001; ++i)
    {
      usqrt(i, &q);
			// remainder differs on some machines
     // fprintf(fp ,"sqrt(%3d) = %2d, remainder = %2d\n",
     fprintf(fp ,"sqrt(%3d) = %2d\n",
	     i, q.sqrt);
    }
  usqrt(l, &q);
  //fprintf(fp ,"\nsqrt(%lX) = %X, remainder = %X\n", l, q.sqrt, q.frac);
  fprintf(fp ,"\nsqrt(%lX) = %X\n", l, q.sqrt);


  fprintf(fp ,"********* ANGLE CONVERSION ***********\n");
  /* convert some rads to degrees */
  for (X = 0.0; X <= 360.0; X += 1.0)
    fprintf(fp ,"%3.0f degrees = %.12f radians\n", X, deg2rad(X));
  puts("");
  for (X = 0.0; X <= (2 * PI + 1e-6); X += (PI / 180))
    fprintf(fp ,"%.12f radians = %3.0f degrees\n", X, rad2deg(X));
  
  
  return 0;
}
