// -*-c++-*-
#ifndef SIMP_STAT_H
#define SIMP_STAT_H

/*****************************************************
* Copyright 2014 John Lapeyre                        *
*                                                    *
* You can redistribute this software under the terms *
* of the GNU General Public License version 3        *
* or a later version.                                *
******************************************************/

/*
 * class gjl::SimpStat
 * Perform very simple statistical analysis. means, standard deviation, etc.
 * This is useful for MontCarlo simulations.
 */

//#include <algorithm>
//#include <vector>
//#include <iostream>

namespace gjl {

template <typename data_t = double >
class SimpStat {

public:
  inline void record (data_t x) {
    mean_tally_ += x;
    mean_sq_tally_ += square_(x);
    ++N_;
  }

  inline double mean () { return mean_tally_ / N_; }

  inline double variance () {
    double mean = mean();
    double variance = (mean_sq_tally_ / N_) - square_(mean);
    return variance;
  }

  inline double std_dev () {return sqrt(variance());}

  inline double error () {return sqrt(variance()/N_);}

  inline double N () {return N_;}

private:
  data_t square_(data_t x) { return x*x;}
  data_t mean_tally_ = 0;
  data_t mean_sq_tally_ = 0;
  size_t N_ = 0;

  //  data_t mean_ = 0;
  //  data_t mean_sq_ = 0;
  //  data_t std_dev_ = 0;
  //  data_t error_ = 0;

}; /*** END class SimpStat */

} /*** END namespace gjl */

#endif
