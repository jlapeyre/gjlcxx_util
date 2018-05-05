#include <stdio.h>
#include <stdlib.h>

#define SITE(A,X,Y) (A[(X)*Ly+(Y)])

void print_array(int *a, int Lx, int Ly) {
  int i,j;
  for(i=0; i<Lx; ++i) {
    for(j=0; j<Ly; ++j) {
      SITE(a,i,j) = SITE(a,i,j) - 1;
      printf("%d\t", SITE(a,i,j));
    }
    printf("\n");
  }
  printf("\n");
}

void set_array(int *a, int N) {
  int i;
  for(i=0; i<N; ++i)
    if (drand48() < 0.5)
      a[i] = 1;
    else a[i] = 0;
}

void do_array() {
  int Lx = 3;
  int Ly = 4;
  int N = Lx * Ly;
  int *a = (int *) malloc(sizeof(int)*N);
  set_array(a,N);
  print_array(a,Lx,Ly);
  free(a);
}

/* Just to show it is compatible with C arrays */

#define LX 3
#define LY 4
void do_array2() {
  int Lx = 3;
  int Ly = 4;
  int N = Lx * Ly;
  int a[LX][LY];
  set_array((int *)a,N);
  print_array((int *)a,Lx,Ly);
}

int main () {
  do_array();
  do_array2();
}
