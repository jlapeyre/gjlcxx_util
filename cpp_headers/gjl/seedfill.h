// -*-c++-*-
#ifndef SEEDFILL_H
#define SEEDFILL_H

/*****************************************************
* Copyright 2014 John Lapeyre                        *
*                                                    *
* You can redistribute this software under the terms *
* of the GNU General Public License version 3        *
* or a later version.                                *
******************************************************/

/*
 * class gjl::SeedFill -- fill an image from a seed. A seed position
 * and a value and a window into the 2d data array aree supplied.
 * fill_one_value() changes all accesible sites to the input value.
 * fill_to_border() takes an additional magic value as input and
 * fills all accesible sites except the magic value.
 * accessible means checking (logically) recursively all neighboring sites
 * that do not satisfy the stopping condition.
 * A raster routine is used, rather than a recursive routine.
 *
 * The buffer may be too small. Setting its size would be easy to implement.
 */

/*
 * John Lapeyre, 2014
 * Modified the Heckbert K&R C code for C++-11
 *
 * A Seed Fill Algorithm
 * by Paul Heckbert
 * from "Graphics Gems", Academic Press, 1990
 *
 */

/*
 * fill.c : simple seed fill program
 *
 * Paul Heckbert	13 Sept 1982, 28 Jan 1987
 */


#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <unordered_set>
#include <iostream>
#include <gjl/cpu_timer.h>

#define SEG_T int

namespace gjl {

template <typename index_t,  typename site_t>
class SeedFill {
public:
  SeedFill ();
  void constructor_helper ();

  /* push new segment on stack */
  inline void push_seg(const SEG_T Y, const SEG_T XL, const SEG_T XR, const int DY); 
  inline void pop_seg(SEG_T  & Y, SEG_T & XL, SEG_T & XR, int  & DY);
  inline index_t site_index(const index_t x, const index_t y) const {return L_*x+y; }
  inline site_t & site(const index_t x, const index_t y) { return lattice_[site_index(x,y)]; }
  inline site_t & site(const index_t ind) { return lattice_[ind]; }

  typedef struct {long y, xl, xr, dy;} Segment;

  typedef struct {		/* window: a discrete 2-D rectangle */
    long x0, y0;			/* xmin and ymin */
    long x1, y1;			/* xmax and ymax (inclusive) */
  } Window;

  void window(const long x0, const long y0, const long x1, const long y1) {
    window_.x0 = x0;
    window_.y0 = y0;
    window_.x1 = x1;
    window_.y1 = y1;
  }

  inline index_t x0() const {return window_.x0;}
  inline index_t y0() const {return window_.y0;}
  inline index_t x1() const {return window_.x1;}
  inline index_t y1() const {return window_.y1;}

  // Return value does not work as I like
  inline std::ostream& print_window (std::ostream& out) {
    out << "Window (" <<  x0() << ","
        <<  y0() << ","
        <<  x1() << ","
        <<  y1() << ")";
    return out;
  }

  void set_all_but_magic_value(const site_t new_value, const site_t magic_value);

  void set_all_but_magic_value(const site_t new_value, site_t magic_value, std::vector<site_t> *set_values);

  /* Find old value at seed point. Find all points contiguous with seed point that
     have old value. Replace them with new value.  */
  void fill_one_value(const index_t x, const index_t y, const site_t new_value);

  /* Starting at seed point, find all points contiguous with seed point that
     do not have border value. Replace them with new value.  */
  void fill_to_border_value(const index_t x, const index_t y, const site_t new_value,
                            const site_t border_value);

  void fill_to_border_value2(const index_t x, const index_t y, const site_t new_value,
                             const site_t border_value,
                             site_t (*siteread) (index_t,index_t,void *), 
                             void (*sitewrite) (index_t,index_t, site_t nv, void *), 
                             void *p);

  void set_lattice(site_t *lattice) { lattice_ = lattice;}
  void set_L(const index_t L) { L_ = L; }

  inline void sitewrite(const index_t x, const index_t y, const site_t newval) {
    if (save_old_values_) {
      auto ind = site_index(x,y);
      indices_of_old_values_.push_back(ind);
      old_values_.push_back(site(ind));
    }
    site(x,y) = newval;
  }

  /*
    Must restore values in reverse order, because some values were written more than once.
     We need FILO. We could also fix the seed fill code to fill each site no more than once.
  */
  inline void restore_old_values () {
    timer_no_save_split();
    for(int i=indices_of_old_values_.size()-1; i >=0 ; --i)
      site(indices_of_old_values_[i]) = old_values_[i];
    timer_save_split("restored " + std::to_string(indices_of_old_values_.size())  + " values");
  }

  /* Don't need to use these */
  void timer_no_save_split() { timer_.no_save_split(); }
  void timer_save_split(const std::string& label) { timer_.save_split(label); }
  void timer_print_splits() { timer_.print_splits();}
  void timer_disable_save_splits() { timer_.disable_save_splits();}
  /* Use this */
  CpuTimer * timer() {return &timer_ ;}

private:
  index_t xseed_, yseed_;
  Window window_;
  size_t stack_size_ = 10000; // no accessors yet
  int l_, x1_, x2_, dy_;
  std::vector<Segment> stack_;
  Segment * sp_;
  Segment * stackp_; // = &(stack_[0]);
  int stack_counter_ = 0;
  site_t * lattice_;
  index_t L_;
  size_t fill_count_;
  bool save_old_values_ = true;
  std::vector<index_t> indices_of_old_values_;
  std::vector<site_t> old_values_;
  CpuTimer timer_;
  
}; /* End class SeedFill */

/************************************************************
 *  Constructor
 ************************************************************/
template <typename index_t,  typename site_t>
SeedFill<index_t,site_t>::SeedFill() : timer_(std::string("SeedFill") ,true)  {
  constructor_helper();
  //  timer_disable_save_splits();
}

template <typename index_t,  typename site_t>
void SeedFill<index_t,site_t>::constructor_helper() {
  stack_.resize(stack_size_);
}

/************************************************************
 ***  class SeedFill Member functions
 ************************************************************/

template <typename index_t,  typename site_t>
void SeedFill<index_t,site_t>::push_seg(SEG_T  Y, SEG_T XL, SEG_T XR, int DY) {
  if (sp_<stackp_+stack_size_ && Y+(DY)>= window_.y0 && Y+(DY)<= window_.y1)
    {
      sp_->y = Y; sp_->xl = XL; sp_->xr = XR; sp_->dy = DY; sp_++;
      ++stack_counter_;
    }
}

template <typename index_t,  typename site_t>
void SeedFill<index_t,site_t>::pop_seg(SEG_T & Y, SEG_T & XL, SEG_T & XR, int  & DY) {
  sp_--; Y = sp_->y+(DY = sp_->dy); XL = sp_->xl; XR = sp_->xr;
  --stack_counter_;
}

template <typename index_t,  typename site_t>
void SeedFill<index_t,site_t>::set_all_but_magic_value(site_t new_value, site_t magic_value) {
  timer_no_save_split();
  for(index_t x=x0(); x<=x1(); ++x)
    for(index_t y=y0(); y<=y1(); ++y)
      if (site(x,y) != magic_value) site(x,y) = new_value;
  timer_save_split("set all but magic value.");
}

  /* 
   * Set all values to new_value, except those which are magic_value.
   * Return in set_values a list of labels which were changed. This list includes neither
   * new_value nor magic_value.
   */
template <typename index_t,  typename site_t>
void SeedFill<index_t,site_t>::set_all_but_magic_value(site_t new_value, site_t magic_value, 
                                                       std::vector<site_t> *set_values) {
  timer_no_save_split();
  std::unordered_set<site_t> setvals;
  for(index_t x=x0(); x<=x1(); ++x)
    for(index_t y=y0(); y<=y1(); ++y) {
      auto old_value = site(x,y);
      if (old_value != magic_value && old_value != new_value) {
        site(x,y) = new_value;
        setvals.insert(old_value);
      }
    }
  for( auto it : setvals)
    set_values->push_back(it);
  timer_save_split("set all but magic value: " + std::to_string(set_values->size()) + " unique site values set");
}


/* In principle, index_t could be unsigned. But code fails in this case. */
template <typename index_t,  typename site_t>
void SeedFill<index_t,site_t>::fill_one_value(index_t xin, index_t yin, 
                                              site_t new_value)
{
  index_t x = xin;
  index_t y = yin;
  SEG_T left, x1, x2;
  int dy = 0;
  site_t old_value;	/* old site value */
  size_t fill_count = 0;

  stackp_ = &(stack_[0]); // need this.
  sp_ = stackp_;

  old_value = site(x,y); /* read pv at seed point */
  if (old_value==new_value || x<window_.x0 || x>window_.x1 
      || y<window_.y0 || y>window_.y1) return;
  push_seg(y, x, x, 1);			/* needed in some cases */
  push_seg(y+1, x, x, -1);		/* seed segment (popped 1st) */
  while (sp_>stackp_) {
    std::cerr << "sc " << stack_counter_ << "\n";
    /* pop segment off stack and fill a neighboring scan line */
    pop_seg(y, x1, x2, dy);
    /*
     * segment of scan line y-dy for x1<=x<=x2 was previously filled,
     * now explore adjacent sites in scan line y
     */
    for (x=x1; x>=window_.x0 && site(x,y) == old_value; x--) {
      site(x,y) = new_value;
      fill_count++;
    }
    if (x>=x1) goto skip;
	left = x+1;
	if (left<x1) push_seg(y, left, x1-1, -dy);		/* leak on left? */
	x = x1+1;
	do {
          for (; x<=window_.x1 && site(x,y) == old_value; x++) {
            site(x,y) = new_value;
            fill_count++;
          }
          push_seg(y, left, x-1, dy);
          if (x>x2+1) push_seg(y, x2+1, x-1, -dy);	/* leak on right? */
skip:	    for (x++; x<=x2 && site(x,y) !=old_value; x++);
          left = x;
	} while (x<=x2);
  }
  fprintf(stderr, "number sites filled %ld\n",fill_count);
}

template <typename index_t,  typename site_t>
void SeedFill<index_t,site_t>::fill_to_border_value(index_t xin, index_t yin, 
                                                    site_t new_value, site_t border_value)
{
  timer_no_save_split();
  index_t x = xin;
  index_t y = yin;
  SEG_T left, x1, x2;
  int dy = 0;
  site_t old_value;	/* old site value */
  size_t fill_count = 0;

  stackp_ = &(stack_[0]); // need this.
  sp_ = stackp_;

  if (x<window_.x0 || x>window_.x1 || y<window_.y0 || y>window_.y1) {
    std::cerr << "SeedFill::fill_to_border_value: seed point (" << x << ", " << y  <<
      ") is outside of window ";
    print_window(std::cerr);
    std::cerr << ".\n";
    abort();
  }

  old_value = site(x,y); /* read pv at seed point */
  if (old_value == border_value ) {
    std::cerr << "SeedFill::fill_to_border_value: value at seed point (" << x << ", " << y  << ")"
              << " is equal to border_value.\n";
    abort();
  }

  push_seg(y, x, x, 1);	 /* Does not seem to help GJL 2014. needed in some cases */
  push_seg(y+1, x, x, -1);		/* seed segment (popped 1st) */
  while (sp_ > stackp_ ) {
    //    std::cerr << "sc " << stack_counter_ <<  " " << (int) (sp_ - stackp_) <<  "\n";
    if (stack_counter_ == 0) break;
    /* pop segment off stack and fill a neighboring scan line */
    pop_seg(y, x1, x2, dy);
    /*
     * segment of scan line y-dy for x1<=x<=x2 was previously filled,
     * now explore adjacent sites in scan line y
     */
    for (x=x1; x>=window_.x0 && site(x,y) != border_value; x--) {
      //    for (x=x1; x>=window_.x0 && site(x,y) == old_value; x--) {
      //      site(x,y) = new_value;
      sitewrite(x,y,new_value);
      fill_count++;
    }
    if (x>=x1) goto skip;
	left = x+1;
	if (left<x1) push_seg(y, left, x1-1, -dy);		/* leak on left? */
	x = x1+1;
	do {
          //          for (; x<=window_.x1 && site(x,y) == old_value; x++) {
          for (; x<=window_.x1 && (site(x,y) != border_value && site(x,y) != new_value) ; x++) {
            //            site(x,y) = new_value;
            sitewrite(x,y,new_value);
            fill_count++;
          }
          push_seg(y, left, x-1, dy);
          if (x>x2+1) push_seg(y, x2+1, x-1, -dy);	/* leak on right? */
          //        skip:	    for (x++; x<=x2 && (site(x,y) != old_value) ; x++);
        skip:	    for (x++; x<=x2 && (site(x,y) == border_value || site(x,y) == new_value) ; x++);
          left = x;
	} while (x<=x2);
  }
  //  fprintf(stderr, "number sites filled %ld\n",fill_count);
  timer_save_split("filled sites. number of sites filled " + std::to_string(fill_count));
}

template <typename index_t,  typename site_t>
void SeedFill<index_t,site_t>::fill_to_border_value2
  (index_t xin, index_t yin, site_t new_value, site_t border_value,
   site_t (*siteread) (index_t,index_t,void *), 
   void (*sitewrite) (index_t,index_t, site_t nv, void *), void *p )
{
  //  std::cerr << "Entered seedfill\n";
  index_t x = xin;
  index_t y = yin;
  SEG_T left, x1, x2;
  int dy = 0;
  site_t old_value;	/* old site value */
  fill_count_ = 0;

  stackp_ = &(stack_[0]); // need this.
  sp_ = stackp_;




  old_value = (*siteread)(x,y,p); /* read pv at seed point */
  if (old_value == border_value || x<window_.x0 || x>window_.x1 
      || y<window_.y0 || y>window_.y1) {
    std::cerr << "seedfill: seed site outside window or on border\n";
    std::cerr << "value at seed site ("  << x << "," << y << ") is "
               << old_value << ".\nBorder value is "
               << border_value << ".\nWindow ("
               <<  window_.x0 << ","
               <<  window_.y0 << ","
               <<  window_.x1 << ","
               <<  window_.y1 << ").\n";
    exit(-1);
  }
    std::cerr << "value at seed site ("  << x << "," << y << ") is "
               << old_value << ".\nBorder value is "
               << border_value << ".\nWindow ("
               <<  window_.x0 << ","
               <<  window_.y0 << ","
               <<  window_.x1 << ","
               <<  window_.y1 << ").\n";
  push_seg(y, x, x, 1);	 /* Does not seem to help GJL 2014. needed in some cases */
  push_seg(y+1, x, x, -1);		/* seed segment (popped 1st) */
  while (sp_ > stackp_ ) {
    //    std::cerr << "sc " << stack_counter_ <<  " " << (int) (sp_ - stackp_) <<  "\n";
    if (stack_counter_ == 0) break;
    pop_seg(y, x1, x2, dy);
    for (x=x1; x>=window_.x0 && (*siteread)(x,y,p) != border_value; x--) {
      (*sitewrite)(x,y,new_value,p);
      fill_count_++;
      //      std::cerr << "number sites filled " << fill_count_ << "\n";
    }
    if (x>=x1) goto skip;
	left = x+1;
	if (left<x1) push_seg(y, left, x1-1, -dy);		/* leak on left? */
	x = x1+1;
	do {
          for (; x<=window_.x1 && ((*siteread)(x,y,p) != border_value 
                                   && (*siteread)(x,y,p) != new_value) ; x++) {
            (*sitewrite)(x,y,new_value,p);
            fill_count_++;
          }
          push_seg(y, left, x-1, dy);
          if (x>x2+1) push_seg(y, x2+1, x-1, -dy);	/* leak on right? */
skip:	    for (x++; x<=x2 && ((*siteread)(x,y,p) == border_value || (*siteread)(x,y,p) == new_value) ; x++);
          left = x;
	} while (x<=x2);
  }
  std::cerr << "number sites filled " << fill_count_ << "\n";
}

/************************************************************
 ***  END class SeedFill Member functions
 ************************************************************/

/**************************************************************************
 *
 *  Non-OO version below
 *
 **************************************************************************/

#define SITEREAD (lattice[L*x+y])
#define SITEWRITE (lattice[L*x+y] = nv)

typedef struct {		/* window: a discrete 2-D rectangle */
  long x0, y0;			/* xmin and ymin */
  long x1, y1;			/* xmax and ymax (inclusive) */
} Window;

#define MAX 10000		/* max depth of stack */
//int MAX = 10000;		/* max depth of stack */


#define PUSH(Y, XL, XR, DY)	/* push new segment on stack */ \
    if (sp<stack+MAX && Y+(DY)>=win->y0 && Y+(DY)<=win->y1) \
    {sp->y = Y; sp->xl = XL; sp->xr = XR; sp->dy = DY; sp++;}

#define POP(Y, XL, XR, DY)	/* pop segment off stack */ \
    {sp--; Y = sp->y+(DY = sp->dy); XL = sp->xl; XR = sp->xr;}

/*
 * fill: set the site at (x,y) and all of its 4-connected neighbors
 * with the same site value to the new site value nv.
 * A 4-connected neighbor is a site above, below, left, or right of a site.
 */

template <typename index_t,  typename site_t>
void fill(index_t x, index_t y, Window * win, site_t nv, size_t L, site_t *lattice)
// int x, y;	seed point
// Window *win;	screen window
// site_t nv;   new site value
{
  /*
   * Filled horizontal segment of scanline y for xl<=x<=xr.
   * Parent segment was on line y-dy.  dy=1 or -1
   */
  typedef struct {index_t y, xl, xr, dy;} Segment;

  index_t l, x1, x2, dy;
  site_t ov;	/* old site value */

  //  Segment *stack = (Segment *) malloc(MAX * sizeof(Segment));
  Segment stack[MAX];
  Segment  *sp = stack;	/* stack of filled segments */
  size_t fill_count = 0;

  ov = SITEREAD;		/* read pv at seed point */
  if (ov==nv || x<win->x0 || x>win->x1 || y<win->y0 || y>win->y1) return;
  PUSH(y, x, x, 1);			/* needed in some cases */
  PUSH(y+1, x, x, -1);		/* seed segment (popped 1st) */
  
  while (sp>stack) {
    /* pop segment off stack and fill a neighboring scan line */
    POP(y, x1, x2, dy);
    /*
     * segment of scan line y-dy for x1<=x<=x2 was previously filled,
     * now explore adjacent sites in scan line y
     */
    for (x=x1; x>=win->x0 && SITEREAD ==ov; x--) {
      SITEWRITE;
      fill_count++;
    }
    if (x>=x1) goto skip;
	l = x+1;
	if (l<x1) PUSH(y, l, x1-1, -dy);		/* leak on left? */
	x = x1+1;
	do {
          for (; x<=win->x1 && SITEREAD ==ov; x++) {
            SITEWRITE;
            fill_count++;
          }
	    PUSH(y, l, x-1, dy);
	    if (x>x2+1) PUSH(y, x2+1, x-1, -dy);	/* leak on right? */
skip:	    for (x++; x<=x2 && SITEREAD !=ov; x++);
	    l = x;
	} while (x<=x2);
    }
    fprintf(stderr, "number sites filled %ld\n",fill_count);
}

#undef PUSH
#undef POP
#undef SITEREAD
#undef SITEWRITE

} /*** END namespace gjl */

#endif
