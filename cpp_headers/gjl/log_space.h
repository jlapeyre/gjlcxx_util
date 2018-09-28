// -*-c++-*-
#ifndef LOG_SPACE_H
#define LOG_SPACE_H

/*****************************************************
* Copyright 2014 John Lapeyre                        *
*                                                    *
* You can redistribute this software under the terms *
* of the GNU General Public License version 3        *
* or a later version.                                *
******************************************************/

/*
 * class gjl::LogSpace -- make a virtual array of values separated
 * by a constant factor. The log of the values are then separated by
 * a constant term.
 * It is a virtual array in the sense that the values are not stored, but
 * rather operator[] is overloaded to compute the required value.
 *
 */

namespace gjl {

  template< typename data_t = double>
  class LogSpace {
  public:
    typedef int ind_t;
    typedef size_t uint_t;
    LogSpace() {}
    LogSpace(data_t xa, data_t xb, uint_t n) { set_xa_xb_n(xa,xb,n); }

    inline void set_xa_xb_n(data_t xa, data_t xb, uint_t n) { xa_ = xa; xb_ = xb; number_of_elements_ = n; init();}

    inline ind_t compute_highest_index () {return number_of_elements_ + lowest_index_ - 1;}
    inline void save_highest_index() { highest_index_ = compute_highest_index();}

    inline ind_t lowest_index() const {return lowest_index_;}
    //    inline ind_t highest_index() const  {return number_of_elements_ + lowest_index_ - 1;}
    inline ind_t highest_index() const  {return highest_index_; }

    inline uint_t number_of_elements() const {return number_of_elements_;}

    inline void set_lowest_index_fixed_n(ind_t i) { lowest_index_ = i;}
    inline void set_num_elements_fixed_lowest_index(uint_t n) { number_of_elements_ = n; init();}
    inline void set_highest_index_fixed_lowest_index(ind_t ihigh) { number_of_elements_ = ihigh - lowest_index_ + 1; init();}

    inline void init() {set_fac(); save_highest_index();}

    inline void set_fac() { fac_ = pow((xb_ / xa_), 1.0/(number_of_elements_- 1));}

    inline data_t val(ind_t i) const {return xa_ * pow(fac_, i - lowest_index_);}

    inline data_t operator[] (ind_t i) const {return val(i);}

  private:
    data_t xa_ = 0;
    data_t xb_ = 0;
    data_t fac_ = 1;
    ind_t number_of_elements_ = 0;
    ind_t lowest_index_ = 0;
    ind_t highest_index_ = 0; // store, because this may be called in tight loops. not worth the complexity!

  }; /*** END class LogSpace */

}

#endif
