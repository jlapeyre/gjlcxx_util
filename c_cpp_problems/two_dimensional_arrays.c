#include <stdio.h>
#include <stdlib.h>

/*
  Test 2D arrays. Define, pass to function, set, and access.
  Test different ways to do this.

  1. Dynamic array of nx pointers to pointers to arrays of ny data_t's
     Called below p to p model.

  2. static array allocated on the stack:
    data_t a[nx][ny];   (actually a one d model)

  3. Dynamic array of nx*ny data_t's (explicit one d model)

  Both methods 2 and 3 use a one block of memory and element (i,j) is
  stored in location i*nx + j. These two are equivalent and routines
  using them can be mixed (They need casting to silence the compiler.)

  Method 1 is different from 2 and 3. Routines should not be
  interchangeable.  In fact sometimes they are in a way, but it is
  dangerous and wrong.  If we allocate storage with one method (p to
  p, or one d), but then consistently store and retrieve with another
  method, we may get lucky and not try to access an illegal location.
  This happens sometimes below. The segfault only happens when freeing
  the memory, but the calculations were correct.

  On the other hand, mixing the one d and p to p models when
  *storing* and *retrieving* rather than allocating always gives
  nonsense and often segfault.

  Below, we declare, allocate, write, and read arrays. We sum the
  values when printing and compare to the expected result. Combinations
  that crash or give incorrect results are commented out.

  There is a file pointer fp that can be set to print the matrix
  output or to send it to /dev/null.

 */


/*
  Set the size of the array for all tests.
  Static arrays must be initialized with constants. So these
  are macros.
*/
#define NX 100
#define NY 90

/* Sum of array values. If a test array does not sum to this, it
   is wrong.
*/
int SUM;

// Whether to free memory. One test crashes when freeing
int free_array_flag = 1;

/******************************************/
// The data type, and format for all arrays.
typedef int data_t;
#define MYFORM %d\t

//typedef float data_t;
//#define MYFORM %f\t
/******************************************/

/* For using string macros to set output format string */
#define STR(x) #x
#define STRINGIFY(x) STR(x)

/* For printing arrays. Set in main() to print either
to /dev/null or stdout.  */
FILE *fp;

/******************************************************
 * Allocate a dynamic 2d array implemented via an array of pointers
 * to pointers.
 */
data_t ** alloc_array (int nx, int ny) {
  data_t **m = (data_t **) malloc(sizeof(data_t *) * nx);
  int i;
  for(i=0;i<nx;i++) m[i]  = malloc(sizeof(data_t) * ny);
  return m;
}

void free_array (data_t **m, int nx) {
    int i;
    if ( free_array_flag ) {
      for(i=0;i<nx;i++) free(m[i]);
      free(m);
    }
}

/******************************************************
 * Allocate a dynamic 2d array implemented as a single array
 * of type data_t.
 */

data_t * alloc_array_oned (int nx, int ny) {
  data_t *m = (data_t *) malloc(sizeof(data_t) * nx * ny);
  return m;
}

void free_array_oned (data_t *m) {
  free(m);
}
/******************************************************/

/* Macros that write the test functions */

#define CLEAR_PTOP(PARAM, EXPR)                      \
  (data_t PARAM, int nx, int ny) {                   \
    int i,j;                                         \
    for(i=0;i<nx;i++)                                \
      for(j=0;j<ny;j++)                              \
        EXPR = 0;                                    \
  }



#define SET_ARRAY(PARAM, EXPR)                      \
  (data_t PARAM, int nx, int ny) {                 \
    int i,j;                                       \
    for(i=0;i<nx;i++)                              \
      for(j=0;j<ny;j++)   {                        \
        EXPR = (i+1)*(j+1);                        \
        }   \
  }

#define PRINT_ARRAY(PARAM, EXPR)                                         \
  (data_t PARAM, int nx, int ny) {                                   \
    int i,j;                                                         \
    int count = 0;                                                   \
    for(i=0;i<nx;i++) {                                              \
      for(j=0;j<ny;j++) {                                             \
        fprintf(fp,STRINGIFY(MYFORM), EXPR);                          \
        count += EXPR;                                                \
      }                                                               \
      fprintf(fp,"\n");                                                \
    }                                                                \
  if ( count != SUM ) {                                              \
    fprintf(stderr, "*** Array sum incorrect. Got %d\n",count);      \
    abort();}                                                        \
  fprintf(fp,"\n");                                                  \
}

#define PLAIN_ARRAY                          m[i][j]
#define DEREF_TWICE                    *(*(m+i) + j)
#define DEREF_TWICE_ALT                  *(m[i] + j)
#define DEREF_ONCE        *((data_t *) m + i*ny + j)

#define PP_PARAM         **m
#define ARRAY_PARAM  m[][NY]
#define ONE_D_PARAM       *m



void set_ptop_plain_array SET_ARRAY(PP_PARAM, PLAIN_ARRAY);
void print_ptop_plain_array PRINT_ARRAY(PP_PARAM, PLAIN_ARRAY);

void set_ptop_deref_twice SET_ARRAY(PP_PARAM, DEREF_TWICE);
void print_ptop_deref_twice PRINT_ARRAY(PP_PARAM, DEREF_TWICE);

void set_ptop_deref_twice_alt SET_ARRAY(PP_PARAM, DEREF_TWICE_ALT);
void print_ptop_deref_twice_alt PRINT_ARRAY(PP_PARAM, DEREF_TWICE_ALT);

void set_ptop_deref_once SET_ARRAY(PP_PARAM,  DEREF_ONCE);
void print_ptop_deref_once PRINT_ARRAY(PP_PARAM, DEREF_ONCE);

void set_static_plain_array SET_ARRAY(ARRAY_PARAM, PLAIN_ARRAY);
void print_static_plain_array PRINT_ARRAY(ARRAY_PARAM, PLAIN_ARRAY);
void clear_static_plain_array CLEAR_PTOP(ARRAY_PARAM, PLAIN_ARRAY);

void set_static_dref_twice SET_ARRAY(ARRAY_PARAM, DEREF_TWICE);
void print_static_dref_twice PRINT_ARRAY(ARRAY_PARAM, DEREF_TWICE);

void set_static_dref_once SET_ARRAY(ARRAY_PARAM,  DEREF_ONCE);
void print_static_dref_once PRINT_ARRAY(ARRAY_PARAM, DEREF_ONCE);

void set_array_dynamic_oned SET_ARRAY(ONE_D_PARAM,  DEREF_ONCE);
void print_array_dynamic_oned PRINT_ARRAY(ONE_D_PARAM, DEREF_ONCE);

// Allocate, set, print, free a dynamic array implemented
// as a pointer to a pointer and heap storage.

#define TEST_PTOP(NUM,VAR, SNAME,PNAME)                \
  fprintf(stderr,"Dynamic test number %d ",NUM);      \
  VAR = alloc_array(nx,ny);                            \
  fprintf(stderr, "Allocated. ");                      \
  SNAME(VAR,nx,ny);                                    \
  fprintf(stderr, "Set. ");                            \
  PNAME(VAR, nx,ny);                                   \
  fprintf(stderr, "Printed. ");                        \
  free_array(VAR,nx);                                  \
  fprintf(stderr, "Free.\n");

void test_dynamic () {
  int nx = NX;
  int ny = NY;
  data_t **m;

  TEST_PTOP(1,m,set_ptop_plain_array, print_ptop_plain_array);
  TEST_PTOP(2,m,set_ptop_deref_twice, print_ptop_deref_twice);
  TEST_PTOP(3,m,set_ptop_deref_twice_alt, print_ptop_deref_twice_alt);

  /* 
     Fails when freeing, but otherwise succeeds. This is because we are consistently
     misuing the storage allocated, and we are lucky not to get a segfault till the
     end.
  */
  free_array_flag = 0;
  TEST_PTOP(4,m,set_ptop_deref_once, print_ptop_deref_once);
  free_array_flag = 1;

  /* 
     The following two fail because we are mixing access methods.
  */
  /*
  fprintf(stderr,"Dynamic test number %d\n",5);
  m = alloc_array(nx,ny);
  set_ptop_plain_array(m,nx,ny);
  print_ptop_deref_once(m,nx,ny);
  free_array(m,nx);
  */

  /*
  fprintf(stderr,"Dynamic test number %d\n",6);
  m = alloc_array(nx,ny);
  set_ptop_deref_once(m,nx,ny);
  print_ptop_plain_array(m,nx,ny);
  free_array(m,nx);
  */

}


void test_dynamic_oned () {
  int nx = NX;
  int ny = NY;
  data_t *m;

  fprintf(stderr,"Dynamic oned test number %d\n",1);
  m = alloc_array_oned(nx,ny);
  set_array_dynamic_oned(m,nx,ny);
  print_array_dynamic_oned(m,nx,ny);
  free_array_oned(m);

  /* One d array works like static 2d array, but I can't silence warnings
     because I can't cast to a C array.
  */
  fprintf(stderr,"Dynamic oned test number %d\n",2);
  m = alloc_array_oned(nx,ny);
  set_static_plain_array(m,nx,ny);
  print_static_plain_array(m,nx,ny);
  free_array_oned(m);

  fprintf(stderr,"Dynamic oned test number %d\n",3);
  m = alloc_array_oned(nx,ny);
  set_static_dref_twice(m,nx,ny);
  print_static_dref_twice(m,nx,ny);
  free_array_oned(m);

  fprintf(stderr,"Dynamic oned test number %d\n",4);
  m = alloc_array_oned(nx,ny);
  set_static_dref_once(m,nx,ny);
  print_static_dref_once(m,nx,ny);
  free_array_oned(m);

  /* 
     In the following two, I mix access routines in setting
     and printing. This works, because static arrays and
     one d implmentation are the same.
  */
  fprintf(stderr,"Dynamic oned test number %d\n",5);
  m = alloc_array_oned(nx,ny);
  set_static_dref_once(m,nx,ny);
  print_array_dynamic_oned(m,nx,ny);
  free_array_oned(m);

  fprintf(stderr,"Dynamic oned test number %d\n",6);
  m = alloc_array_oned(nx,ny);
  set_array_dynamic_oned(m,nx,ny);
  set_static_dref_once(m,nx,ny);
  free_array_oned(m);


}

void test_static ()  {
  data_t a[NX][NY];

  fprintf(stderr,"Static test number %d\n",1);
  set_ptop_deref_once((data_t **)a,NX,NY);
  print_ptop_deref_once((data_t **)a,NX,NY);

  /* 
     Calling set_array_pointer_to_pointers_x for 1 through 3
     causes a segmentation fault.

     E.g.:

     set_ptop_deref_twice_alt((data_t **)a,NX,NY);
     print_ptop_deref_twice_alt((data_t **)a,NX,NY);

  */

  fprintf(stderr,"Static test number %d\n",2);
  set_static_plain_array(a,NX,NY);
  print_static_plain_array(a,NX,NY);

  fprintf(stderr, "Static test number %d\n",3);
  set_static_dref_twice(a,NX,NY);
  print_static_dref_twice(a,NX,NY);

  fprintf(stderr, "Static test number %d\n",4);
  set_static_dref_once(a,NX,NY);
  print_static_dref_once(a,NX,NY);

}




void set_SUM () {
  int i,j;
  SUM = 0;
  for(i=0;i<NX;i++)
    for(j=0;j<NY;j++)
      SUM += (i+1)*(j+1);
}


int main()
{

  fp = fopen("/dev/null","w");
  //  fp = stdout; // Choose this to see output

  set_SUM();

  test_dynamic_oned();
  test_static();
  test_dynamic();

  fclose(fp);
  return 1;
}

/*
void print_2 (data_t **m) {
  int i,j;
  for(i=0;i<NX;i++) {
    for(j=0;j<NY;j++) {
      //      printf("%d %d  ", i,j);
      //      printf(STRINGIFY(MYFORM), m[i][j]);
      //      printf(STRINGIFY(MYFORM), *(*(m+i) + j));
      printf(STRINGIFY(MYFORM), *((data_t *) m + i*NY + j));
    }
    printf("\n");
  }
  printf("\n");
}

*/
