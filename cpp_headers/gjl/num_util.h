// -*-c++-*-
#ifndef GJL_NUM_UTIL_H
#define GJL_NUM_UTIL_H

#include <vector>

/*****************************************************
* Copyright 2014 John Lapeyre                        *
*                                                    *
* You can redistribute this software under the terms *
* of the GNU General Public License version 3        *
* or a later version.                                *
******************************************************/

/*
 * namespace gjl  -- some routines to perform common small, and not so small,
 * numerical tasks. There are also classes in this namespace in other header files.
 *
 * class gjl::Vector2D -- store a 2d array as a 1d vector. The data can be
 * accessed as either a 1D or 2D vector. Tests show that access times are as
 * fast, or nearly so, as directly indexing a 1d C array. The data storage is
 * compatible with C 2d arrays.
 *
 */

namespace gjl {

  /* Assign a value to all elements in a containter
   * Why does c++11 not have this ? 
   */
  template<typename container_t, typename val_t>
  inline void assign(container_t& v, val_t x) { v.assign(v.size(),x);}

  template<typename x_t, typename xmin_t>
  inline void update_x_xmin(const x_t  x, xmin_t * xmin) { if (x<*xmin) *xmin = x; }
  
  template<typename x_t, typename xmax_t>
  inline void update_x_xmax(const x_t  x, xmax_t * xmax) { if (x>*xmax) *xmax = x; }
  
  template<typename x_t, typename xmin_t, typename xmax_t>
  inline void update_xmin_xmax(const x_t  x, xmin_t * xmin, xmax_t * xmax) {
    update_x_xmin(x,xmin);
    update_x_xmax(x,xmax);
  }
  
  template<typename x_t> 
  inline x_t square(const x_t x) {return x*x;}

  template<typename x_t> 
  inline x_t sum_squares(const x_t x, const x_t y) {return x*x + y*y;}

  /* c++11 std lib rngs can be initialized in various ways. Can't find good
     docs. So we choose one type for rng seeds.
  */
  typedef long int seed_t;

  /*
  template<typename data_t>
  inline void print_pair(data_t x, data_t y) {
    for(int i=0; i< (int) x.size(); ++i) {
      out << x[i] << " " << y[i] << "\n";
    }
  }
  */

  /* *******************************************************************
   *  2D Vector class. The data is stored as 1D vector, so it accessible
   *  as a 1D vector as well. It seems to compare very well in speed to
   *  direct access to the pointer to the data.
  */
  template<typename data_t>
  class Vector2D {
  public:
    Vector2D() {}
    Vector2D(const size_t nx, const size_t ny) { nx_ = nx; ny_ = ny; resize(nx,ny);}
    
    inline size_t size() const { return n_;}
    inline size_t size_x() const { return nx_;}
    inline size_t size_y() const { return ny_;}
    inline void resize(const size_t nx, const size_t ny) { 
      nx_ = nx; ny_ = ny;
      set_size_();
      vec_.resize(size());
      data_ = vec_.data();
    }
    inline void clear() {vec_.clear(); nx_=ny_=0; set_size_();}

    inline data_t * data() { return vec_.data();}
    inline std::vector<data_t>& vector() {return vec_;}
    inline std::vector<data_t>* vectorp() {return &vec_;}
    
    inline typename std::vector<data_t>::iterator begin() {return vec_.begin();}
    inline typename std::vector<data_t>::iterator end() {return vec_.end();}
    /* This is about as fast direct access to data in at least some tests */
    inline data_t & operator()(const int i, const int j) {return vec_[i*ny_+j];}
    inline data_t & operator()(const int i) {return vec_[i];}
    inline data_t & operator[](const int i) {return vec_[i];}
    inline int index(const int i, const int j) const {return i*ny_+j;}
    
  private:
    inline void set_size_() { n_ = nx_ * ny_;}
    std::vector<data_t> vec_;
    size_t nx_ = 0;
    size_t ny_ = 0;
    size_t n_ = 0;
    data_t* data_;
  }; /*** END class Vector2D */
  
} /*** END namespace gjl */

/*******************************************************/

#endif
