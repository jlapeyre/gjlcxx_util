// -*-c++-*-
#ifndef ARR_IRREG_H
#define ARR_IRREG_H

#include <iostream>
#include <vector>
#include <cmath>
//#include <math>
#include <gjl/log_space.h>

/*
 * class ArrIrreg -- "irregular" array. We record values in an array
 * at times that are separated by a constant factor. It could be
 * generalized to times separated by arbitrary values, which would
 * actually be irregular. The virtual array of times is managed by
 * class LogSpace. Some process generates times and values. We insert
 * a time and value into the irregular array if the process time exceeds
 * the next time in the virtual array.
 *
 */

/*
 *  Check if it is time to record a value and, if so, record it.
 *  Several values may be recorded (probably the same value.), advancing
 *  time till we need to take more step.s
 *
 *  This macro evaluates current_time once. getval is evaluated each
 *  time the value is referenced.  But, in the current application, it
 *  has a constant value.
 */
// + 1e-6 is important. I spent hours looking for the bug.
#define ARR_IRREG_CHECK_RECORD(arr,current_time,getval)                      \
  if (! (arr).over_max_ind()) {                                              \
    auto curtime = (current_time) + 1e-10;                                   \
    while (curtime  >= (arr).get_next_time() && (! (arr).over_max_ind() ) )  \
      {                                                                      \
        (arr).record_arr((getval));                                          \
      }                                                                      \
  }

template< typename data_t = double>
class ArrIrreg {
public:
  ArrIrreg() {}
  ArrIrreg(data_t xa, data_t xb, size_t n) { set_xa_xb_n(xa,xb,n); }
  inline void set_xa_xb_n(data_t xa, data_t xb, size_t n) { time_.set_xa_xb_n(xa,xb,n); init_arr();}
  inline void init_arr() {
    arr_.clear(); counts_.clear();
    arr_.resize(number_of_elements());
    counts_.resize(number_of_elements());
    index_ = 0;
    highest_index_with_full_count_ = -1;
  }
  // only lowest_index = 0 is supported now.
  inline void set_lowest_index(int i) { time_.set_lowest_index_fixed_n(i);}
  inline int lowest_index() const { return time_.lowest_index_fixed();}
  inline int ind(int i) const { return i - time_.lowest_index();}

  inline int highest_index() const { return time_.highest_index(); }

  inline data_t get_time(int i) const {return time_[i];}
  inline data_t get_arr(int i) {auto c = counts(i); if (c>0) return arrsum(i) / c; return 0;}
  inline void arr_sum_add_to(int i, data_t arrval) { arrsum(i) += arrval;}
  inline void reset_index() { index_ = time_.lowest_index();}

  inline data_t& arrsum(int i) { return arr_[ind(i)];}
  inline size_t& counts(int i) { return counts_[ind(i)];}

  inline data_t get_next_time() const {return time_[index_];}

  inline void record_arr(data_t val) {
    ++counts_[index_];
    arr_sum_add_to(index_++, val);
  }

  inline int number_of_elements() const {return time_.number_of_elements();}

  inline bool over_max_ind() const {
    if (index_ >= number_of_elements()) return true;
    return false;
  }

  inline void merge(ArrIrreg<data_t>& other) {
    for(int i=0; i< (int) arr_.size(); ++i) {
      arr_sum_add_to(i,other.arrsum(i));
      counts_[i] += other.counts(i);
    }
  }

  inline void print_arr(std::ostream& out = std::cout) {
    for(int i=0; i < (int) arr_.size(); ++i)
      out << time_[i] << " " << get_arr(i) << "\n";
  }

  inline void print_log_arr(std::ostream& out = std::cout) {
    for(int i=0; i < (int) arr_.size(); ++i) {
      auto t = time_[i];
      auto m = get_arr(i);
      if (t>0 && m>0)
        out << log10(t) << " " << log10(m) << "\n";
    }
  }

  inline void print_arr_with_counts(std::ostream& out = std::cout) {
    for(int i=0; i < (int) arr_.size(); ++i)
      out << time_[i] << " " << get_arr(i) << " " << counts(i) << "\n";
  }

  inline void print_log_arr_with_counts(std::ostream& out = std::cout) {
    for(int i=0; i < (int) arr_.size(); ++i) {
      auto t = time_[i];
      auto m = get_arr(i);
      auto c = counts(i);
      if (t>0 && m>0 && c > 0)
        out << log10(t) << " " << log10(m) << " " << log10(c) << "\n";
    }
  }

  // rely on lowest index == 0
  inline void find_highest_index_with_full_count() {
    size_t full_count = counts(0);
    int i;
    for(i=0; i < (int) arr_.size(); ++i) {
      if (counts(i) != full_count) break;
    }
    highest_index_with_full_count_ = i - 1;
  }

  inline int highest_index_with_full_count() const { return highest_index_with_full_count_; }

  inline bool are_all_count_numbers_same () {
    if (highest_index_with_full_count() < 0)
      find_highest_index_with_full_count();
    return highest_index_with_full_count() == highest_index();
  }

  inline data_t max_time_with_full_counts()  {
    find_highest_index_with_full_count();
    int i = highest_index_with_full_count();
    return time_[i];
  }

private:
  std::vector<data_t> arr_;
  std::vector<size_t> counts_;
  gjl::LogSpace<data_t> time_;
  int index_ = 0;
  int highest_index_with_full_count_ = -1;
};  /*** END class ArrIrreg  */

#endif
