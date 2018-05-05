#include <iostream>
#include <vector>
#include <iomanip>
#include <gjl/cpu_timer.h>

//#include "gjl_num_util.h"

template<typename data_t>
class Vector2D {
public:
  Vector2D() {}
  Vector2D(const size_t nx, const size_t ny) { nx_ = nx; ny_ = ny; resize(nx,ny);}

  inline size_t size() { return n_;}
  inline size_t size_x() { return nx_;}
  inline size_t size_y() { return ny_;}
  inline data_t * data() { return vec_.data();}
  inline std::vector<data_t>& vector() {return vec_;}
  inline std::vector<data_t>* vectorp() {return &vec_;}

  inline typename std::vector<data_t>::iterator begin() {return vec_.begin();}
  inline typename std::vector<data_t>::iterator end() {return vec_.end();}

  inline void resize(const size_t nx, const size_t ny) { 
    nx_ = nx; ny_ = ny;
    set_size_();
    vec_.resize(size());
    data_ = vec_.data();
  }

  /* This seems pretty fast */
  inline data_t & operator()(const int i, const int j) {return vec_[i*ny_+j];}
  inline data_t & operator()(const int i) {return vec_[i];}

  inline int index(const int i, const int j) {return i*ny_+j;}
  
private:
  inline void set_size_() { n_ = nx_ * ny_;}

  std::vector<data_t> vec_;
  size_t nx_ = 0;
  size_t ny_ = 0;
  size_t n_ = 0;
  data_t* data_;
};

/********************************************************
  Test code
*/

typedef double test_t;

//#define SETVAL (1+i)*(1+j)

// With this, our () method is as fast as the data
//#define SETVAL i*j

// Also fast with this.
#define SETVAL i+j

void set_with_iterator ( Vector2D<test_t>& v) {
  int i = 0;
    for( auto it = v.begin(); it != v.end(); ++it)
      *it = i++;
}

void set_with_two_loops_fast ( Vector2D<test_t>& v) {
  std::cout << "\nset_with_two_loops_fast\n";
  size_t nx = v.size_x();
  size_t ny = v.size_y();
  for(size_t i=0; i<nx; ++i)
    for(size_t j=0; j<ny; ++j)
      (v(i,j)) = SETVAL;
}

void set_with_two_loops ( Vector2D<test_t>& v) {
  std::cout << "\nset_with_two_loops\n";
  for(size_t i=0; i<v.size_x(); ++i)
    for(size_t j=0; j<v.size_y(); ++j)
      (v(i,j)) = SETVAL;
}

void set_with_two_loops_data ( Vector2D<test_t>& v) {
  std::cout << "\nset_with_two_loops_data\n";
  test_t *x = v.data();
  size_t ny = v.size_y();
  for(size_t i=0; i<v.size_x(); ++i)
    for(size_t j=0; j<v.size_y(); ++j)
      x[i*ny+j] = SETVAL;
}

void set_with_two_loops_data_fast ( Vector2D<test_t>& v) {
  std::cout << "\nset_with_two_loops_data_fast\n";
  test_t *x = v.data();
  size_t ny = v.size_y();
  size_t nx = v.size_x();
  for(size_t i=0; i<nx; ++i)
    for(size_t j=0; j<ny; ++j)
      x[i*ny+j] = SETVAL;
}

/* This is 2 times slower than the others !! */
void set_with_two_loops_vector ( Vector2D<test_t>& v) {
  std::cout << "\nset_with_two_loops_vector\n";
  std::vector<test_t> vec = v.vector();
  size_t ny = v.size_y();
  for(size_t i=0; i<v.size_x(); ++i)
    for(size_t j=0; j<v.size_y(); ++j)
      vec[i*ny+j] = SETVAL;
}

/* Also 2 times slower than the others !!*/
void set_with_two_loops_vector_fast ( Vector2D<test_t>& v) {
  std::cout << "\nset_with_two_loops_vector_fast\n";
  std::vector<test_t> vec = v.vector();
  size_t ny = v.size_y();
  size_t nx = v.size_x();
  for(size_t i=0; i<nx; ++i)
    for(size_t j=0; j<ny; ++j)
      vec[i*ny+j] = SETVAL;
}

/* Fast as others */
void set_with_two_loops_vector_ptr ( Vector2D<test_t>& v) {
  std::cout << "\nset_with_two_loops_vector_ptr\n";
  std::vector<test_t> * vec = v.vectorp();
  size_t ny = v.size_y();
  size_t nx = v.size_x();
  for(size_t i=0; i<nx; ++i)
    for(size_t j=0; j<ny; ++j)
      (*vec)[i*ny+j] = SETVAL;
}

void set_with_one_loop (Vector2D<test_t>& v) {
  for(size_t i=0; i<v.size(); ++i)
    v(i) = i*i;
}

void print_with_loops (Vector2D<test_t>& v) {
  for(size_t i=0; i<v.size_x(); ++i) {
    for(size_t j=0; j<v.size_y(); ++j)
      std::cout << std::setw(4) << v(i,j) << " ";
    std::cout << "\n";
  }
  std::cout << "\n";
}


int main() {

  // Use large values for testing efficiency
  //  int Lx = 15000;
  //  int Ly = 15000;

  // Smaller values to run quickly
  int Lx = 5000;
  int Ly = 5000;

  //  Vector2D<double> v;
  //  v.resize(Lx,Ly);

  CpuTimer t;
  t.start();
  t.split_seconds();

  Vector2D<double> w(Lx,Ly);
  t.print_split_seconds();
  
  std::cout << "Go!\n";

  set_with_two_loops_vector(w);
  t.print_split_seconds();
 
  set_with_two_loops_vector_fast(w);
  t.print_split_seconds();

  set_with_two_loops_vector_ptr(w);
  t.print_split_seconds();

  set_with_two_loops_data(w);
  t.print_split_seconds();

  set_with_two_loops_data_fast(w);
  t.print_split_seconds();

  set_with_two_loops(w);
  t.print_split_seconds();

  set_with_two_loops_fast(w);
  t.print_split_seconds();

  //  set_with_one_loop(w);
  //  set_with_one_loop(w);
  //  set_with_iterator(w);

  //  print_with_loops(w);
  //  for(size_t i=0; i<w.size(); ++i)
  //    w(i) = (1+i)*(1+i);

  return 1;
}
