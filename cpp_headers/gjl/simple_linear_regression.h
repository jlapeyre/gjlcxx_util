// -*-c++-*-
#ifndef GJL_SIMPLE_LINEAR_REGRESSION_H
#define GJL_SIMPLE_LINEAR_REGRESSION_H

/*****************************************************
* Copyright 2014 John Lapeyre                        *
*                                                    *
* You can redistribute this software under the terms *
* of the GNU General Public License version 3        *
* or a later version.                                *
******************************************************/

/*

  C++11
  Linear least squares fit. Data is in two containers, x and y.
  x and y can be containers of different types and data. For example,
  you can mix float and double, array and vector.

  This is the only file required for using this class.

  Usage:

    // The type here is used for slope, intercept, etc. It is not the input data type
    simple_linear_regression<double> linfit;

    // Fit data in containers x and y, skipping first 100 points
    linfit.fit_range(x.begin() + 100, x.end(), y.begin() + 100);
    // Write human readable report to standard error
    linfit.report(std::cerr);

    // Fit all data in containers x and y
    linfit.fit(x,y);
 */

#include <iostream>

template <typename output_t>
class simple_linear_regression {
  output_t slope_data_, intercept_data_, r_data_;
  size_t n_data_;
  //  output_t s_slope, s_intercept;
 public:
  template <typename Iterx, typename Itery>
    void fit_range(Iterx xbegin, Iterx xend, Itery ybegin );
  template <typename Iterx, typename Itery>
  void fit_range_weighted(Iterx xbegin, Iterx xend, Itery ybegin, Itery wbegin );

  template <typename Datax, typename Datay>
    void fit( Datax & x, Datay & y) {
    fit_range(x.begin(),x.end(),y.begin());
  }

  template <typename Datax, typename Datay>
  void fit_weighted( Datax & x, Datay & y, Datay & w) {
    fit_range_weighted(x.begin(),x.end(),y.begin(), w.begin());
  }

  inline output_t slope() const { return slope_data_; };
  inline output_t intercept() const { return intercept_data_; };
  inline output_t r() const { return r_data_; };
  inline size_t n() const { return n_data_; };
  void report( std::ostream & stream );

}; /*** END class simple_linear_regression */

template <class output_t>
template <class Iterx, class Itery>
 void simple_linear_regression<output_t>::fit_range( Iterx xstart, Iterx xend, Itery ystart) {

  //  auto Sx = *xstart - *xstart; // gotta be a better way
  typename std::iterator_traits<Iterx>::value_type Sx = 0;
  typename std::iterator_traits<Itery>::value_type Sy = 0;
  auto Sxy = Sx;
  auto Sxx = Sx;
  auto Syy = Sy;
  size_t n = 0;
  for (auto xit = xstart, yit = ystart; xit != xend; ++xit, ++yit) {
    auto x = *xit;
    auto y = *yit;
    Sx += x;
    Sy += y;
    Sxx += x*x;
    Syy += y*y;
    Sxy += x*y;
    ++n;
  }
  if ( n == 0 ) {
    std::cerr << "*** simple_linear_regression: size of x,y is zero!\n";
    abort();
  }
  n_data_ = n;
  auto Dx = n * Sxx - Sx * Sx;
  auto Dy = n * Syy - Sy * Sy;
  auto Dxy = n * Sxy - Sx * Sy;
  slope_data_ = Dxy / Dx;
  intercept_data_ = (Sy - slope_data_ * Sx)/n;
  r_data_ = Dxy / sqrt(Dx * Dy);

  /*
  auto se2 = (Dy-m*m*Dx)/(n*(n-2));
  auto sm2 = n * se2 / Dx;
  auto sb2 = sm2 * Sxx/ n;
  */
}

template <class output_t>
template <class Iterx, class Itery>
  void simple_linear_regression<output_t>::fit_range_weighted( Iterx xstart, Iterx xend,
                                                         Itery ystart, Itery wstart) {

  //  auto Sx = *xstart - *xstart; // gotta be a better way
  typename std::iterator_traits<Iterx>::value_type Sx = 0;
  typename std::iterator_traits<Itery>::value_type Sy = 0;
  auto Sxy = Sx;
  auto Sxx = Sx;
  auto Syy = Sy;
  auto W = Sy;
  size_t n = 0;
  for (auto xit = xstart, yit = ystart, wit = wstart ; xit != xend; ++xit, ++yit, ++wit) {
    auto x = *xit;
    auto y = *yit;
    auto w = *wit;
    Sx += x * w;
    Sy += y * w;
    Sxx += x*x*w;
    Syy += y*y*w;
    Sxy += x*y*w;
    W += w;
    ++n;
  }
  if ( n == 0 ) {
    std::cerr << "*** simple_linear_regression: size of x,y is zero!\n";
    exit(-1);
  }
  n_data_ = n;
  auto Dx = W * Sxx - Sx * Sx;
  auto Dy = W * Syy - Sy * Sy;
  auto Dxy = W * Sxy - Sx * Sy;
  slope_data_ = Dxy / Dx;
  intercept_data_ = (Sy - slope_data_ * Sx)/W;
  r_data_ = Dxy / sqrt(Dx * Dy);
}

template < class output_t >
void simple_linear_regression<output_t>::report ( std::ostream & stream ) {
  stream << "Fit " << n() << " points, Slope m: "
            << slope() << ",  intercept b: " << intercept() << "\n";
  stream << "Pearson r: " << r() << "\n";
}

#endif
