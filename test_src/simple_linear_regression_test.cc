#include <vector>
#include <array>
#include <list>
#include <iostream>
#include <math.h>
#include <random>
#include "gjl/simple_linear_regression.h"

#define UINT unsigned int

/*
  compile eg with:
  g++ -std=c++11 simple_linear_regression_test.cc

*/

template <class Iterx, class Itery >
void fill_data( Iterx xstart, Iterx xend, Itery ystart, double slope, double intercept ) {

  double noise_range = 10;
  //  std::default_random_engine generator;
  std::mt19937_64 generator;
  std::uniform_real_distribution<double> distribution(-noise_range,noise_range);

  unsigned int i = 0;
  for( auto xit = xstart, yit = ystart; xit != xend; ++xit, ++yit ) {
    *xit = i++ * 1.2;
    double noise = distribution(generator);
    *yit = slope * *xit + intercept + noise;
  }
}

template <class Datax, class Datay >
void test_a_fit ( Datax & x, Datay & y ) {
  const double m_in = 3.4;
  const double b_in = 2.1;
  const unsigned int N = x.size();

  fill_data(x.begin(),x.end(),y.begin(), m_in, b_in);  

  simple_linear_regression<double> linfit;

  std::cout << "Fitting first half of the data: " << N/2 << " points.\n";
  linfit.fit_range(x.begin(), x.end() - N/2, y.begin());
  linfit.report(std::cout);

  std::cout << "\nFitting all of the data: " << N << " points.\n";
  linfit.fit(x,y);
  linfit.report(std::cout);
  std::cout << "\n";
}


template <class Iterx, class Itery >
void fill_data_weighted( Iterx xstart, Iterx xend, Itery ystart, Itery wstart,
                 double slope, double intercept ) {
  double noise_range = 10;
  //  std::default_random_engine generator;
  std::mt19937_64 generator;
  std::uniform_real_distribution<double> distribution(-noise_range,noise_range);

  unsigned int i = 0;
  for( auto xit = xstart, yit = ystart, wit = wstart; xit != xend; ++xit, ++yit, ++wit ) {
    *xit = i++ * 1.2;
    double noise = distribution(generator);
    *yit = slope * *xit + intercept + noise;
    *wit = 1/i; // can try different things here.
  }
}

template <class Datax, class Datay >
void test_a_fit_weighted ( Datax & x, Datay & y, Datay & w ) {
  const double m_in = 3.4;
  const double b_in = 2.1;
  const unsigned int N = x.size();

  fill_data(x.begin(),x.end(),y.begin(), m_in, b_in);  

  simple_linear_regression<double> linfit;

  std::cout << "Fitting first half of the data: " << N/2 << " points.\n";
  linfit.fit_range(x.begin(), x.end() - N/2, y.begin());
  linfit.report(std::cout);

  std::cout << "\nFitting all of the data: " << N << " points.\n";
  linfit.fit(x,y);
  linfit.report(std::cout);
  std::cout << "\n";
}


int main () {

  const UINT N = 10000;

  std::vector<double> x1(N);
  std::vector<double> y1(N);

  std::cout << " -- Two double vectors.\n";
  test_a_fit(x1,y1);

  std::vector<double> w1(N);

  std::cout << " -- weighted fit.\n";
  test_a_fit_weighted(x1,y1,w1);

  std::vector<double> x2(N);
  std::vector<long double> y2(N);

  std::cout << " -- double vector x, long double vector y.\n";
  test_a_fit(x2,y2);

  std::cout << " -- std::array x and y.\n";
  std::array<double, N > x3;
  std::array<long double, N > y3;
  test_a_fit(x3,y3);

  std::cout << " -- mix array and vector.\n";
  test_a_fit(x2,y3);

  /* Note x.end() - N/2 above does not work with std::list */
  std::cout << " -- mix vector and doubly linked list.\n";
  std::list<double> y4(N);
  test_a_fit(x2,y4);

  return 0;
}
