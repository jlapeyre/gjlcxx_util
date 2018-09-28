// -*-c++-*-
#ifndef WALK2D_H
#define WALK2D_H

#include <vector>
#include <random>
#include <iostream>
#include <gjlutils.h>

#include "walkperc_opts.h"
#ifndef WALK_2D_GENERATOR
#define WALK_2D_GENERATOR mt19937_64
#endif

template <typename coord_t = int, typename time_t = double>
class Walk2D {
public:

  inline Walk2D() { }
  inline Walk2D(coord_t Lx, coord_t Ly) { init(Lx,Ly); }
  inline explicit Walk2D(coord_t L) { Lx_=L; Ly_=L; Walk2D(L,L); }
  inline void init(coord_t L) {  init(L,L); }
  inline void init(coord_t Lx, coord_t Ly) { Lx_=Lx; Ly_=Ly; init_xycur(); }
  inline void clear();

  // already defined below !
  //  inline coord_t xrel(coord_t x) {return x - Lx_/2;}
  //  inline coord_t yrel(coord_t y) {return y - Ly_/2;}

  inline coord_t x0() const { return Lx_/2;}
  inline coord_t y0() const { return Ly_/2;}

  inline void init_xycur() { xcur_ = x0(); ycur_ = y0(); }
  inline size_t get_num_boundary_hits() const { return num_boundary_hits_; }
  inline void set_num_boundary_hits(size_t n) { num_boundary_hits_ = n; }
  inline size_t get_num_steps_total() const { return num_steps_total_; }
  inline void set_num_steps_total(size_t n) { num_steps_total_ = n; }
  inline size_t get_n_walks() const { return n_walks_; }
  inline void set_n_walks(size_t n) { n_walks_ = n; }

  inline coord_t xmin() const {return xmin_;}
  inline coord_t ymin() const {return ymin_;}
  inline coord_t xmax() const {return xmax_;}
  inline coord_t ymax() const {return ymax_;}

  // These can count during multiple walks.
  inline void reset_counters () {
    num_boundary_hits_ = 0;
    num_steps_total_ = 0;
    n_walks_ = 0;
    xmax_ = 0;
    ymax_ = 0;
    xmin_ = Lx_+1;
    ymin_ = Ly_+1;
  }

  inline void increment_num_of_walks() { ++n_walks_; }

  inline void init_bound_walk(size_t num_steps) {
    num_steps_to_do_ = num_steps;
    init_bound_walk();
  }

  inline void init_bound_walk() {
    num_steps_cur_ = 0;
    init_xycur();
  }

  std::vector<coord_t> &  xrec() {return xrec_;}
  std::vector<coord_t> &  yrec() {return yrec_;}

  std::vector<coord_t> &  xrec1() {return xrec1_;}
  std::vector<coord_t> &  yrec1() {return yrec1_;}

  /*************************************************************************
   * methods for initializing recording of x,y,t
   */
  inline void resize_xyrec (size_t n) {
    xrec_.clear();
    yrec_.clear();
    xrec1_.clear();
    yrec1_.clear();
    size_t nelem = n / rec_skip_;
    xrec_.resize(nelem);
    yrec_.resize(nelem);
  }
  inline void resize_xyrec () { resize_xyrec(num_steps_to_do_); }

  inline void reset_xyrec_its () {
    it_xrec_ = xrec_.begin();
    it_yrec_ = yrec_.begin();
  }

  inline void resize_trec (size_t n) {
    size_t nelem = n / rec_skip_;
    trec_.resize(nelem);
  }

  inline void resize_trec () { resize_trec(num_steps_to_do_); }

  /*************************************************************************/

  inline void merge ( Walk2D<coord_t,time_t> const &other) {
    num_boundary_hits_ += other.get_num_boundary_hits();
    num_steps_total_ += other.get_num_steps_total();
    n_walks_ += other.get_n_walks();
    gjl::update_x_xmin(other.xmin(), &xmin_);
    gjl::update_x_xmax(other.xmax(), &xmax_);
    gjl::update_x_xmin(other.ymin(), &ymin_);
    gjl::update_x_xmax(other.ymax(), &ymax_);
  }

  inline void set_number_of_steps (size_t num_steps, size_t skip) {
    rec_skip_ = skip;
    num_steps_to_do_ = num_steps;
    init_bound_walk();
  }

  inline void set_number_of_steps (size_t num_steps) {set_number_of_steps(num_steps,1); }
  inline size_t get_number_of_steps_to_do() const { return num_steps_to_do_; }

  inline coord_t xcur() const {return xcur_; }
  inline coord_t ycur() const {return ycur_; }
  inline coord_t xprev() const {return xprev_; }
  inline coord_t yprev() const {return yprev_; }
  inline coord_t num_steps_cur() const {return num_steps_cur_; }

  inline coord_t xrel(coord_t x) const { return x - x0();}
  inline coord_t yrel(coord_t y) const { return y - y0();}

  inline coord_t xrelcur() { return xrel(xcur());}
  inline coord_t yrelcur() { return yrel(ycur());}

  inline coord_t xrelprev() { return xrel(xprev());}
  inline coord_t yrelprev() { return yrel(yprev());}

  inline void do_one_bound_step();
  inline void do_bound_steps(size_t n);
  inline void do_bound_steps() { do_bound_steps( num_steps_to_do_ ); }
  inline void do_rec_bound_steps(size_t n);
  inline void do_rec_bound_steps() { do_rec_bound_steps( num_steps_to_do_ ); }
  inline void record_with_skip();
  inline void record_xy1();
  inline void seed(gjl::seed_t seed) { generator_.seed(seed); sample_.reset(); seed_ = seed;}
  inline gjl::seed_t  get_seed() { return seed_;}
  inline void print_stats(std::ostream& out = std::cout) const;

  inline std::string rng_name() const { return generator_name_; }

  //  inline void print_rng() { std::cout << "# Walk2D rng generator = " << rng_name() << "\n";}

private:

  coord_t xcur_ = 0;
  coord_t ycur_ = 0;
  coord_t xprev_ = 0;
  coord_t yprev_ = 0;

  coord_t xmax_ = 0;
  coord_t ymax_ = 0;
  coord_t xmin_ = 0;
  coord_t ymin_ = 0;

  std::vector<coord_t> xrec_;
  std::vector<coord_t> yrec_;
  std::vector<time_t> trec_;

  std::vector<coord_t> xrec1_;
  std::vector<coord_t> yrec1_;

  typename std::vector<coord_t>::iterator it_xrec_;
  typename std::vector<coord_t>::iterator it_yrec_;

  size_t rec_skip_ = 1;
  size_t num_steps_to_do_ = 0;
  size_t num_steps_cur_ = 0;
  size_t num_boundary_hits_ = 0;
  size_t num_steps_total_ = 0;
  size_t n_walks_ = 0;

  coord_t Lx_ = 0;
  coord_t Ly_ = 0;

  gjl::seed_t seed_ = 1;

  std::WALK_2D_GENERATOR generator_;
  const std::string generator_name_ = std::string(WALKPERC_OPTS_STRINGIFY(WALK_2D_GENERATOR));

  //  std::mt19937_64 generator_;

  std::uniform_real_distribution<double> sample_ {0.0, 1.0};
  inline double urandom() { return sample_(generator_); }

}; /******** END class class Walk2D  */

/**********************************************/

template <typename coord_t, typename time_t>
inline void Walk2D<coord_t,time_t>::clear() {
  reset_counters();
  xcur_ = 0;
  ycur_ = 0;
  rec_skip_ = 1;
  num_steps_to_do_ = 0;
  num_steps_cur_ = 0;
  num_boundary_hits_ = 0;
  num_steps_total_ = 0;
  n_walks_ = 0;
}

template <typename coord_t, typename time_t>
inline void Walk2D<coord_t,time_t>::do_one_bound_step() {
  xprev_ = xcur_;
  yprev_ = ycur_;
  auto variate = urandom();
  coord_t newp;
  if ( variate < 0.25 ) {
    newp = xcur_ + 1;
    if ( newp < Lx_ ) xcur_++;
    else num_boundary_hits_ ++;
  }
  else if ( variate < 0.5 ) {
    newp = ycur_ + 1;
    if ( newp < Ly_ ) ycur_++;
    else num_boundary_hits_ ++;
  }
  else if ( variate < 0.75 ) {
    newp = xcur_ - 1;
    if ( newp > 0 ) xcur_--;
    else num_boundary_hits_ ++;
  }
  else {
    newp = ycur_ - 1;
    if ( newp >0 ) ycur_--;
    else num_boundary_hits_ ++;
  }
  PATCHWALK_CHECK_RANGE(num_steps_cur_, 0, num_steps_to_do_);
  ++ num_steps_cur_;
  ++ num_steps_total_;
  gjl::update_xmin_xmax(xcur_, &xmin_, &xmax_);
  gjl::update_xmin_xmax(ycur_, &ymin_, &ymax_);
}

template <typename coord_t, typename time_t>
inline void Walk2D<coord_t,time_t>::do_bound_steps(const size_t n) {
  for(size_t i=0; i < n; ++i)
    do_one_bound_step();
}

template <typename coord_t, typename time_t>
inline void Walk2D<coord_t,time_t>::record_with_skip() {
  if ( num_steps_cur_ % rec_skip_ == 0 ) {
    *it_xrec_ = xcur_;
    *it_yrec_ = ycur_;
    ++it_xrec_;
    ++it_yrec_;
  }
}

template <typename coord_t, typename time_t>
inline void Walk2D<coord_t,time_t>::record_xy1() {
  xrec1_.push_back(xcur_);
  yrec1_.push_back(ycur_);
}

template <typename coord_t, typename time_t>
inline void Walk2D<coord_t,time_t>::do_rec_bound_steps(const size_t n) {
  resize_xyrec(n);
  reset_xyrec_its();
  for(size_t i=0; i < n; ++i) {
    do_one_bound_step();
    record_with_skip();
  }
  ++n_walks_;
}

template <typename coord_t, typename time_t>
inline void Walk2D<coord_t,time_t>::print_stats(std::ostream& out)
  const {
  out << "### Walk2D\n";
  out << "# Lx = " << Lx_ << "\n";
  out << "# Ly = " << Ly_ << "\n";
  out << "# Number of walks = " << n_walks_ << "\n";
  out << "# number of steps per walk = " << num_steps_to_do_ << "\n";
  out << "# Total number of boundary hits = " << num_boundary_hits_ << "\n";
  out << "# Total number of steps = " << num_steps_total_ << "\n";
  out << "# xmax = " << xrel(xmax_) << "\n";
  out << "# ymax = " << yrel(ymax_) << "\n";
  out << "# xmin = " << xrel(xmin_) << "\n";
  out << "# ymin = " << yrel(ymin_) << "\n";
  out << "# Walk rng name  = " << rng_name() << "\n";
}

#endif
