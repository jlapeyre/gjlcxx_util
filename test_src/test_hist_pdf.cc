#include "gjl/hist_pdf.h"

typedef gjl::HistPdf<> hist_t;

static size_t n_fail = 0;
static size_t n_tests = 0;

void fail_msg(int c) {
  std::cerr << "*** test " << c << " failed.\n";
}

template <typename T>
void dotest(T func, int c) {
  if( ! func() ) {
    ++n_fail;
    fail_msg(c);
  }
  ++ n_tests;
}

// equality of uninitialized objs
bool test_1 () {
  hist_t a;
  hist_t b;
  return (a == b);
}

// equality of initialized objs
bool test_2 () {
  hist_t a(10,0,10);
  hist_t b(10,0,10);
  return (a == b);
}

// inequality of initialized objs of different shape
bool test_3 () {
  hist_t a(10,0,10);
  hist_t b(10,0,9);
  return (a != b);
}

bool test_4 () {
  hist_t a(10,0,10);
  hist_t b(9,0,10);
  return (a != b);
}

bool test_5 () {
  hist_t a(10,0,10);
  hist_t b(9,0,10);
  return ( ! a.is_same_shape(b) );
}

// inequality of linear and log spaced objs
bool test_6 () {
  hist_t a(10,0,10,true);
  hist_t b(10,0,10);
  return ( ! a.is_same_shape(b) );
}

// inequality of shape of linear and log spaced objs
bool test_7 () {
  hist_t a(10,0,10);
  hist_t b(10,0,10);
  a.use_log(true);
  return ( ! a.is_same_shape(b) );
}

// change to from linear and log spacing
bool test_8 () {
  hist_t a(10,0,10,true);
  hist_t b(10,0,10);
  a.use_log(false);
  return ( a.is_same_shape(b) );
}

bool test_9 () {
  hist_t a(10,0,10);
  hist_t b(10,0,10,true);
  b.use_log(false);
  return ( a.is_same_shape(b) );
}

// equality of objs with identical data
bool test_10 () {
  hist_t a(10,0,10);
  hist_t b(10,0,10);
  a.add_count(2.0);
  b.add_count(2.0);
  return ( a == b);
}

// equality of objs with identical data
// that results from different input
bool test_11 () {
  hist_t a(10,0,10);
  hist_t b(10,0,10);
  a.add_count(2.0);
  b.add_count(2.01);
  return ( a == b);
}

// inequality of objs with different data
bool test_12 () {
  hist_t a(10,0,10);
  hist_t b(10,0,10);
  a.add_count(2.0);
  b.add_count(3.01);
  return ( a != b);
}

// equality of shape of objs with different data
bool test_13 () {
  hist_t a(10,0,10);
  hist_t b(10,0,10);
  a.add_count(2.0);
  b.add_count(4.01);
  return ( a.is_same_shape(b) && a != b );
}

// add obj to another. Data and counts are updated
bool test_14 () {
  hist_t a(10,0,10);
  hist_t b(10,0,10);
  a.add_count(2.0);
  b += a;
  return ( a == b);
}

// Default copy constructor copies everything.
bool test_15 () {
  hist_t a(10,0,10);
  a.add_count(9.0);
  hist_t b = a;
  return ( a == b);
}

// Assignment in construction is really a copy
bool test_16 () {
  hist_t a(10,0,10);
  a.add_count(9.0);
  hist_t b = a;
  b.add_count(-43.0);
  return ( a != b);
}

// Test that pdf integrates to 1
bool test_17 () {
  hist_t a(10,0,10);
  a.add_count(5);
  return (  a.pdf_integral() == 1);
}

// Test that max is adjusted to data
bool test_18 () {
  hist_t a(10,0,10);
  auto max1 = a.max();
  a.add_count(8.8);
  a.fit_to_data_min_max();
  return (  a.max() == 8.8 && max1 == 10 );
}

// Test that max is adjusted to max data times factor
bool test_19 () {
  hist_t a(10,0,10);
  a.add_count(8);
  a.fit_to_data_min_max(0.2);
  return (  a.max() == 9.6);
}

// Add variable number of counts as parameters to add_cout
bool test_20 () {
  hist_t a(10,0,10);
  a.add_count(8,9.1,3.5,0.1);
  return ( a.n_counts() == 4 );
}


int main () {
  dotest(test_1,1);
  dotest(test_2,2);
  dotest(test_3,3);
  dotest(test_4,4);
  dotest(test_5,5);
  dotest(test_6,6);
  dotest(test_7,7);
  dotest(test_8,8);
  dotest(test_9,9);
  dotest(test_10,10);
  dotest(test_11,11);
  dotest(test_12,12);
  dotest(test_13,13);
  dotest(test_14,14);
  dotest(test_15,15);
  dotest(test_16,16);
  dotest(test_17,17);
  dotest(test_18,18);
  dotest(test_19,19);
  dotest(test_20,20);
  std::cout << n_fail << "/" << n_tests << " tests failed.\n";
  return 1;
}
