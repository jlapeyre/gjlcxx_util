#include <iostream>
#include "gjl/num_util.h"
#include "gjl/arr_irreg.h"
#include "walk_2d.h"

/*
 * We also have a macro in arr_irreg.h
 * I have to be careful to pass by ref! (don't forget &)
 * Otherwise, even with inline, it is much slower because a copy or something is made.
 */
/* This has a bug. compare it to the macro, which is corrected
inline void check_arr_record_time(ArrIrreg<>& total_arr, Walk2D<>& walk) {
  if ( ! total_arr.over_max_ind() ) {
    auto curtime = walk.num_steps_cur();
    while (curtime  > total_arr.get_next_time()) {
      total_arr.record_arr(gjl::sum_squares(walk.xrelprev(), walk.yrelprev()));
    }
  }
}
*/

int main () {
  size_t L = 1000;
  Walk2D<> walk(L);
  ArrIrreg<> m;
  std::vector<int> arr;
  size_t N_record_times = 300;
  double Nsteps_per_walk = 1000;
  double tmax = Nsteps_per_walk/2;
  m.set_xa_xb_n(100,tmax,N_record_times);
  int Ntrials = 10000;
  arr.resize(Nsteps_per_walk);
  for (int j=0; j< Ntrials; ++j) {
    m.reset_index();
    walk.init_bound_walk(Nsteps_per_walk);
    for(int i=0; i < Nsteps_per_walk; ++i) {
      walk.do_one_bound_step();
      arr[i] += gjl::sum_squares(walk.xrelcur(),walk.yrelcur());
      ARR_IRREG_CHECK_RECORD(m,walk.num_steps_cur(),gjl::sum_squares(walk.xrelprev(),walk.yrelprev()));
      //check_arr_record_time(m,walk);
    }
  }
  //  m.print_arr();
  m.print_log_arr();
  for(size_t j=0; j < arr.size(); ++j) {
    //    std::cout << j << " " << arr[j]/Ntrials << "\n";
  }
  return 1;
}
