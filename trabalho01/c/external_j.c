#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define n 33000

int main(int argc, char *argv[])
{
  // Allocate resources
  struct timespec start, end;
  double **A = (double **)malloc(n * sizeof(double *));
  for (unsigned i = 0; i < n; i++)
    A[i] = (double *)malloc(n * sizeof(double));
  double *x = (double *)malloc(n * sizeof(double));
  double *b = (double *)malloc(n * sizeof(double));

  // Initialize A and x with random values between 0 and 1
  for (long long i = 0; i < n; i++)
  {
    x[i] = (double)(rand()) / RAND_MAX;
    for (unsigned j = 0; j < n; j++)
      A[i][j] = (double)(rand()) / RAND_MAX;
  }

  // Compute b = Ax
  clock_gettime(CLOCK_MONOTONIC_RAW, &start);
  for (long long i = 0; i < n; i++)
  {
    double s = 0;
    for (long long j = 0; j < n; j++)
      s += A[j][i] * x[j];
    b[i] = s;
  }
  clock_gettime(CLOCK_MONOTONIC_RAW, &end);
  unsigned long delta_ms = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1000000;

  printf("%lu\n", delta_ms);

  // Return OK
  return 0;
}
