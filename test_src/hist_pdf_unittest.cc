#include <limits.h>
#include "gjl/hist_pdf.h"
#include "gtest/gtest.h"

typedef gjl::HistPdf<> hist_t;

// equality of uninitialized objs
TEST(EQTests, EmptyConstructor) {
  hist_t a;
  hist_t b;
  EXPECT_EQ(a,b);
}

TEST(EQTests, ShapedConstructor) {
  hist_t a(10,0,10);
  hist_t b(10,0,10);
  EXPECT_EQ(a,b);
}

TEST(EQTests, DifferentShapesNeq) {
  hist_t a(10,0,10);
  hist_t b(9,0,10);
  EXPECT_NE(a,b);
}

TEST(EQTests, IsSameShape) {
  hist_t a(10,0,10);
  hist_t b(9,0,10);
  EXPECT_TRUE( ! a.is_same_shape(b) );
}

TEST(EQTests, FindIndex) {
  hist_t h(10,0,10);
  for(size_t i=0; i < h.n_bins(); ++i)
    h.set_bin_val(i,i*1.0);
  size_t idx = h.find_index(2.01);
  EXPECT_TRUE( idx == 2 );
}


/////////////////////////////////////////////////////
#include <stdio.h>
GTEST_API_ int main(int argc, char **argv) {
  printf("Running main() from gtest_main.cc\n");
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
