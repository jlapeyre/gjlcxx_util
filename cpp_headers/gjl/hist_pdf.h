// -*-c++-*-
#ifndef HIST_PDF_H
#define HIST_PDF_H

#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>

/*****************************************************
* Copyright 2014 John Lapeyre                        *
*                                                    *
* You can redistribute this software under the terms *
* of the GNU General Public License version 3        *
* or a later version.                                *
******************************************************/
/*
  1-d histogram class.

  Also makes probability density function.

  Examples are a bit outdated.

  HistPdf <double, double> hist(100,0.0,30.0);
  hist.add_count(x);
  auto counts = hist.counts();
  for(unsigned int i = 0; i < counts->size(); ++i)
    printf("%f %f\n", hist.center(i), hist.pdf(i));

  hist.clear();

  Template types
  cnt_t -- type of number of counts in each bin. "ordinate"
  bin_t -- type of data that we are counting. "coordinate"

  Example:
*/

/*
 * We have a vector v of objects. Each object has a HistPdf h as a member. We
 * want to merge all of the HistPdf members  v[1].h,... v[n-1].h
 * to the HistPdf v[0].h.
 * In the applications, v has several different HistPdf members. We call
 * this macro repeatedly. There is probably a better way to do this.
 */
#define HIST_PDF_MEMBER_MERGE(class_vec,hist_member)                    \
  for(size_t i=1; i<(class_vec).size(); ++i) {                          \
    (class_vec)[0].hist_member.merge((class_vec)[i].hist_member);       \
  }

namespace gjl {

  namespace hist_pdf {

    template <typename bin_t>
    inline bin_t histlog (const bin_t x) { return log10(x); }

    template <typename bin_t>
    inline bin_t histexp (const bin_t x) { return (pow(10.0,x));}

  }

template <typename cnt_t = double, typename bin_t = double>
class HistPdf {
public:
  typedef unsigned int ind_t;
  inline HistPdf() {};
  inline explicit HistPdf(size_t n);
  inline HistPdf(size_t n, bin_t min, bin_t max);
  inline HistPdf(size_t n, bin_t min, bin_t max, bool uselog );
  inline void constructor_helper();
  //  inline void init(size_t n, bin_t min, bin_t max, bool uselog = false);
  inline void init(size_t n, bin_t min, bin_t max);

  inline void copy_shape(const HistPdf<cnt_t,bin_t>  &other)
  {init(other.n_bins(), other.min(), other.max(), other.using_log());}

  // But this will not, as is, copy uselog !
  /*
    inline void copy_shape(const HistPdf<cnt_t,bin_t>  &other)
    {init(other.n_bins(), other.min(), other.max());}
  */

  inline bin_t pdf_integral () const;
  inline bin_t weighted_pdf_integral () const;
  inline bin_t width() const { return width_; }

  inline bin_t min() const { return maybe_exp(min_); }
  inline bin_t max() const { return maybe_exp(max_); }
  inline bin_t data_max() const { return maybe_exp(data_max_); }
  inline bin_t data_min() const { return maybe_exp(data_min_); }

  inline bin_t center(ind_t i) const { return maybe_exp(min_ + i * width_ + width_/2);}

  inline double logwidth(ind_t i) const {
    return hist_pdf::histexp(min_ + width_ * (i+1))*(1-hist_pdf::histexp(-width_));}
  inline bin_t pdf(ind_t i) const {
    if (using_log_) {
      double lw = logwidth(i);
      return counts_[i] / (n_counts_ * lw);
    }
    return counts_[i] / (n_counts_ * width_);
  }
  inline size_t n_counts () const { return n_counts_; }
  inline cnt_t  n_weighted_counts () const { return n_weighted_counts_; }
  inline size_t n_bins () const { return n_bins_; }
  inline size_t n_greater_than_max() const { return n_greater_than_max_; }
  inline void n_greater_than_max(size_t n) const { n_greater_than_max_ = n; }
  inline size_t n_less_than_min() const { return n_less_than_min_; }
  inline size_t num_merged_hists() const { return num_merged_hists_;}
  inline bin_t weighted_pdf (ind_t i) const { return counts_[i] / (n_weighted_counts_ * width_);}
  inline std::vector<cnt_t> * counts () { return & counts_; }  // why cant i use this ?
  /*
    Use & so it can be used as an rvalue. Google code standards doesn't like
    this. Use as an lvalue would be clumsy now use add_to_counts
  */
  inline cnt_t count(ind_t i) const { return counts_[i];}
  std::ostream& print_pdf_long_header (std::ostream& out) const;
  inline std::ostream& print_pdf_long_line (std::ostream& out, cnt_t count, bin_t cent, bin_t pdf) const;
  std::ostream& print_pdf_long (std::ostream& out) const;
  std::ostream& print_pdf_long () const { return this->print_pdf_long (std::cout);}
  void print_pdf_header (std::ostream& out) const;
  inline void print_pdf_line (std::ostream& out, cnt_t count, bin_t cent, bin_t pdf) const;
  void print_pdf (std::ostream& out) const;
  inline void print_pdf () const { this->print_pdf(std::cout);}

  void print_pdf(std::string& filename) {
    std::ofstream out;
    out.open(filename);
    print_pdf(out);
    out.close();
  }

  void print_pdf(const char *filename) {
    std::string sfname = std::string(filename); // not allowed to create in argument list and pass by ref.
    print_pdf(sfname);
  }

  inline void zero_printing_off() { handle_printing_zeros_ = do_not_print_zeros;}
  inline void zero_printing_on() { handle_printing_zeros_ = print_zeros;}
  inline void zero_printing_commented() { handle_printing_zeros_ = print_commented_out_zeros;}

  template <typename Iter>
  inline void fill(Iter begin, Iter stop);
  inline void add_count(bin_t x);
  template <typename ...Tail>
  inline void add_count(double head, Tail&&... tail);
  inline void add_count();
  inline void add_count(const std::vector<double> &vect) {for(size_t i=0; i<vect.size(); ++i) add_count(vect[i]);}
  // Don't know a good way to do varargs here.
  inline void add_weighted_count(bin_t x, cnt_t weight);

  // for merging histograms, add_count is for adding a data point
  inline void add_to_counts(size_t i, bin_t x) { counts_[i] += x; }
  inline void clear();

  inline bool using_log () const { return using_log_ ; }

  inline void use_log(bool uselog) {
    using_log_ = uselog;
    min_ = maybe_log(lin_min_);
    max_ = maybe_log(lin_max_);
    constructor_helper();
  }

  inline void merge (const HistPdf<cnt_t,bin_t>  &other);

  inline void operator+=(const HistPdf<cnt_t,bin_t> & other) {this->merge(other);}

  template<typename Iter>
  inline void merge (Iter start, Iter end) {
    auto start0 = start;
    for(auto it = start+1; it < end; ++it)
      merge(*it);
  }

  inline void set_bin_val(size_t idx, bin_t r);

  inline void fit_to_data_min_max(double fraction);
  inline void fit_to_data_min_max() {fit_to_data_min_max(0);}
  inline void fit_to_data_max();

  inline bool is_same_shape(const HistPdf<cnt_t,bin_t> & other) const;
  inline bool operator==(const HistPdf<cnt_t,bin_t> & other) const;
  inline bool operator!=(const HistPdf<cnt_t,bin_t> & other) const
  { return !(*this == other); }

  inline void filename(const std::string fname) { filename_ = fname;}
  inline const std::string filename() const { return filename_; }

  inline void set_hist_name(std::string name) { hist_name_ = name; is_hist_name_enabled_ = true; }
  inline const std::string& hist_name() const { return hist_name_; }
  inline bool is_hist_name_enabled() const { return is_hist_name_enabled_; }

  inline size_t find_index(bin_t x);

  inline void increment_num_trials() { ++num_trials_; }
  inline size_t num_trials() const { return num_trials_; }

private:

  size_t n_bins_ =  0;
  size_t n_counts_ = 0; // total counts in all bins
  size_t n_greater_than_max_ = 0;
  size_t n_less_than_min_ = 0;
  cnt_t n_weighted_counts_;
  size_t num_merged_hists_ = 0;
  size_t num_trials_ = 0; // a convenience for the application

  std::vector<cnt_t> counts_;
  std::vector<bin_t> bins_;
  std::vector<bin_t> custom_bins_;

  bin_t max_ = 1;
  bin_t min_ = 0;

  bin_t lin_max_ = 1; // store values in case we switch between log and linear
  bin_t lin_min_ = 0;

  bin_t width_ = 0;
  bin_t data_max_ = 0;
  bin_t data_min_ = 0;

  bool using_log_ = false;

  std::string filename_ = "";

  std::string hist_name_ = "";
  bool is_hist_name_enabled_ = false;

  enum handle_printing_zeros_t { print_zeros, do_not_print_zeros, print_commented_out_zeros };

  handle_printing_zeros_t handle_printing_zeros_ = do_not_print_zeros;

  //  Moved to a namespace at top of file
  //  inline bin_t histlog (const bin_t x) const { return log10(x); }
  //  inline bin_t histexp (const bin_t x) const { return (pow(10.0,x));}

  inline bin_t maybe_log (const bin_t x) const {
    if (using_log_) return hist_pdf::histlog(x);
    else return x;
  }

  inline bin_t maybe_exp (const bin_t x) const {
    if (using_log_) return hist_pdf::histexp(x);
    else return x;
  }

  /* We don't exp these even if we have log spacing. */
  inline bin_t data_max_internal() const { return data_max_; }
  inline bin_t data_min_internal() const { return data_min_; }

}; /*** END class HistPdf */

/*
  Constructors
*/

/* g++ did not inline this */
template <typename cnt_t, typename bin_t>
inline void HistPdf<cnt_t,bin_t>::constructor_helper() {
  width_ = (max_- min_)/n_bins_;
  counts_.resize(n_bins_,0);
  bins_.resize(n_bins_+1,0);
  custom_bins_.resize(n_bins_+1,0);
  n_counts_ = 0;
}

template <typename cnt_t, typename bin_t>
inline HistPdf<cnt_t,bin_t>::HistPdf(size_t n_bins, bin_t min, bin_t max) {
  n_bins_ = n_bins;
  lin_min_ = min;
  lin_max_ = max;
  min_ = maybe_log(min);
  max_ = maybe_log(max);
  constructor_helper();
}

template <typename cnt_t, typename bin_t>
inline HistPdf<cnt_t,bin_t>::HistPdf(size_t n_bins, bin_t min, bin_t max, bool uselog) {
  n_bins_ = n_bins;
  lin_min_ = min;
  lin_max_ = max;
  use_log(uselog);
}

template <typename cnt_t, typename bin_t>
inline void HistPdf<cnt_t,bin_t>::init(size_t n_bins, bin_t min, bin_t max) {
//inline void HistPdf<cnt_t,bin_t>::init(size_t n_bins, bin_t min, bin_t max, bool uselog) {
  n_bins_ = n_bins;
  lin_min_ = min;
  lin_max_ = max;
  min_ = maybe_log(min);
  max_ = maybe_log(max);
  constructor_helper();
  num_merged_hists_ = 0;
}

template <typename cnt_t, typename bin_t>
inline HistPdf<cnt_t,bin_t>::HistPdf(size_t n) {
  n_bins_ = n;
  constructor_helper();
}

/********************************
  END Constructors
********************************/

template <typename cnt_t, typename bin_t>
inline void HistPdf<cnt_t,bin_t>::add_count(bin_t x) {
  x = maybe_log(x);
  auto bin_idx = (int)((x - min_) / width_);
  if ( bin_idx < 0 ) bin_idx = 0;
  if ( bin_idx >= (int) counts_.size() ) bin_idx = (int) counts_.size() - 1;
  ++counts_[bin_idx];
  ++n_counts_;
  if ( n_counts_ == 1 ) {
    data_min_ = x;
    data_max_ = x;
  }
  else {
    if ( x > data_max_ ) data_max_ = x;
    if ( x < data_min_ ) data_min_ = x;
  }
  if ( x > max_ ) ++n_greater_than_max_;
  if ( x < min_ ) ++n_less_than_min_;
}

template <typename cnt_t, typename bin_t>
inline void HistPdf<cnt_t,bin_t>::add_count() { }

template <typename cnt_t, typename bin_t>
template <typename ...Tail>

void HistPdf<cnt_t,bin_t>::add_count(double head, Tail&&... tail)
{
  add_count(head);
  add_count(std::forward<Tail>(tail)...);
}

template <typename cnt_t, typename bin_t>
inline void HistPdf<cnt_t,bin_t>::add_weighted_count(bin_t x, cnt_t weight) {
  auto bin_idx = (int)((x - min_) / width_);
  if ( bin_idx < 0 ) bin_idx = 0;
  if ( bin_idx >= (int) counts_.size() ) bin_idx = (int) counts_.size() - 1;
  counts_[bin_idx] += weight;
  ++n_counts_;
  n_weighted_counts_ += weight;
  if ( n_counts_ == 1 ) {
    data_min_ = x;
    data_max_ = x;
  }
  else {
    if ( x > data_max_ ) data_max_ = x;
    if ( x < data_min_ ) data_min_ = x;
  }
  if ( x > max_ ) ++n_greater_than_max_;
  if ( x < min_ ) ++n_less_than_min_;
}

template <typename cnt_t, typename bin_t>
inline void HistPdf<cnt_t,bin_t>::clear() {
  n_counts_ = 0;
  n_weighted_counts_ = 0;
  n_greater_than_max_ = 0;
  n_less_than_min_ = 0;
  num_merged_hists_ = 0;
  std::fill(counts_.begin(),counts_.end(), 0);
}

/*
 not finished
*/
template <typename cnt_t, typename bin_t>
template <typename Iter>
inline void HistPdf<cnt_t,bin_t>::fill(Iter begin, Iter stop) {
  auto minmax_pair = std::minmax_element (begin, stop);
  min_ = minmax_pair.first();
  max_ = minmax_pair.second();
  width_ = (max_-min_)/n_bins_;
}

/*
  Integrate the PDF to make sure that the result is 1.
*/
template <typename cnt_t, typename bin_t>
bin_t HistPdf<cnt_t,bin_t>::pdf_integral () const {
  bin_t sum=0;
  if (using_log_) {
    for (ind_t i=0; i<n_bins_; ++i)
      sum += logwidth(i) * pdf(i);
  }
  else
    for (ind_t i=0; i<n_bins_; ++i)
      sum += width_ * pdf(i);
  return sum;
}

template <typename cnt_t, typename bin_t>
bin_t HistPdf<cnt_t,bin_t>::weighted_pdf_integral () const {
  bin_t sum=0;
  for (ind_t i=0; i<n_bins_; ++i)
    sum += width_ * weighted_pdf(i);
  return sum;
}

/* g++ did not inline this. not a big deal */
template <typename cnt_t, typename bin_t>
inline std::ostream& HistPdf<cnt_t,bin_t>::print_pdf_long_header (std::ostream& out) const {
  out << "# Ncounts " << n_counts_ << "\n";
  out << "# Min data " << data_min() << "\n";
  out << "# Max data " << data_max() << "\n";
  out << "# log(center)  log(pdf)  center  pdf  counts\n";
  return out;
}

  /* This is log ?? */
template <typename cnt_t, typename bin_t>
inline std::ostream& HistPdf<cnt_t,bin_t>::print_pdf_long_line (std::ostream& out, cnt_t count,
                                                         bin_t cent, bin_t pdf) const {
    if ( count > 0 )
      out << log10(cent) << " " << log10(pdf) << " " <<
        cent << " " << pdf << " " << count << std::endl;
    return out;
}

template <typename cnt_t, typename bin_t>
std::ostream& HistPdf<cnt_t,bin_t>::print_pdf_long (std::ostream& out) const {
  this->print_pdf_long_header(out);
  for(ind_t i=0; i < counts_.size(); ++i)
    print_pdf_long_line(out, this->counts_[i], this->center(i), this->pdf(i));
  return out;
}

template <typename cnt_t, typename bin_t>
inline void HistPdf<cnt_t,bin_t>::print_pdf_header (std::ostream& out) const {
  out << "####  Histogram\n";
  if ( is_hist_name_enabled() ) out << "# name: " << hist_name() << "\n";
  out << "# Ncounts " << n_counts_ << "\n";
  out << "# Min data " << data_min() << "\n";
  out << "# Max data " << data_max() << "\n";
  out << "# Min bin " << min() << "\n";
  out << "# Max bin " << max() << "\n";
  out << "# Nbins " << n_bins() << "\n";
  out << "# n greater than max " << n_greater_than_max_ << "\n";
  out << "# n less than min " << n_less_than_min_ << "\n";
  out << "# num_merged_hists " << num_merged_hists() << "\n";
  out << "# num_trials " << num_trials() << "\n";
  out << "# Log spacing = " << (using_log() ? "true" : "false") << "\n";
  out << "#\n# center pdf counts\n";
  out << "#\n";
}

template <typename cnt_t, typename bin_t>
inline void HistPdf<cnt_t,bin_t>::print_pdf_line (std::ostream& out, cnt_t count,
                                                           bin_t cent, bin_t pdf) const {
  if ( handle_printing_zeros_ == print_zeros || count > 0 )
      out <<  cent << " " << pdf << " " << count << std::endl;
  else if (handle_printing_zeros_ == print_commented_out_zeros)
    out <<  "# " << cent << " " << pdf << " " << count << std::endl;
}

template <typename cnt_t, typename bin_t>
void HistPdf<cnt_t,bin_t>::print_pdf (std::ostream& out) const {
  this->print_pdf_header(out);
  for(ind_t i=0; i < counts_.size(); ++i)
    print_pdf_line(out, this->counts_[i], this->center(i), this->pdf(i));
}

template <typename cnt_t, typename bin_t>
bool HistPdf<cnt_t,bin_t>::is_same_shape(const HistPdf<cnt_t,bin_t> & other) const {
  if ( n_bins() != other.n_bins()
       || using_log() != other.using_log()
       || max() != other.max()
       || min() != other.min())
    return false;
  else return true;
}

template <typename cnt_t, typename bin_t>
bool HistPdf<cnt_t,bin_t>::operator==(const HistPdf<cnt_t,bin_t> & other) const {
  if ( ! is_same_shape(other) ) return false;
  for(size_t i=0; i < n_bins(); ++i)
    if (count(i) != other.count(i)) return false;
  return true;
}

template <typename cnt_t, typename bin_t>
void HistPdf<cnt_t,bin_t>::fit_to_data_min_max(double fraction) {
  init(n_bins(),data_min() * (1-fraction), data_max() * (1+fraction));
}

template <typename cnt_t, typename bin_t>
inline void HistPdf<cnt_t,bin_t>::fit_to_data_max() {
  init(n_bins(),min(),data_max());
}

template <typename cnt_t, typename bin_t>
inline void HistPdf<cnt_t,bin_t>::merge(const HistPdf<cnt_t,bin_t>  &other) {
  if (! is_same_shape(other)) {
    std::cerr << "*** hist_pdf: cannot merge histograms of different shapes.\n";
    abort();
  }
  for(size_t i=0; i<n_bins_; ++i)
    add_to_counts(i,other.count(i));
  this->n_counts_ += other.n_counts();
  this->n_greater_than_max_ += other.n_greater_than_max();
  this->num_trials_ += other.num_trials_;
  double omin = other.data_min_internal();
  double omax = other.data_max_internal();
  if (omin < this->data_min()) this->data_min_ = omin;
  if (omax > this->data_max()) this->data_max_ = omax;
  ++num_merged_hists_;
}

template <typename cnt_t, typename bin_t>
inline size_t HistPdf<cnt_t,bin_t>::find_index(bin_t x) {
  size_t upper = n_bins_ ;
  size_t lower = 0 ;

  while (upper - lower > 1)
    {
      size_t mid = (upper + lower) / 2 ;
      //      std::cerr << mid << "\n";
      //      std::cerr << " val " << bins_[mid] << "\n";
      if (x >= custom_bins_[mid])
        {
          lower = mid ;
        }
      else
        {
          upper = mid ;
        }
    }
  std::cerr << lower << "\n";
  return lower;
}

template <typename cnt_t, typename bin_t>
inline void HistPdf<cnt_t,bin_t>::set_bin_val(size_t idx, bin_t r) {
  custom_bins_[idx] = r;
}

} /*** END namespace gjl */

#endif
