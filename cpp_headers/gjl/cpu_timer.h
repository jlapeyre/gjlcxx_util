// -*-c++-*-
#ifndef CPU_TIMER_H
#define CPU_TIMER_H

#include <time.h>
#include <ctime>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
//#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>

extern "C" {
#include "getRSS.h"
}

/*****************************************************
* Copyright 2014 John Lapeyre                        *
*                                                    *
* You can redistribute this software under the terms *
* of the GNU General Public License version 3        *
* or a later version.                                *
******************************************************/

/******************************************************************************
 * A class for measuring cpu time and clock time used by process.
 * Also get hostname, and memory use information.
 *
 * The system records the cpu time used by the process. We are interested in
 * timing a section of code (or part of the process, maybe the whole process.).
 * starting a timer, that is recording the current cpu time consumed, and
 * then recording the elapsed cpu time
 * during part of the code.
 ********************************************************/

class CpuTimer {
public:
  CpuTimer() {this->start();}
  CpuTimer(bool start) { if (start) this->start(); }
  CpuTimer(const std::string& timer_label) { this->timer_label(timer_label); this->start();}
  CpuTimer(const std::string& timer_label, bool start) {
    this->timer_label(timer_label);
    if (start) this->start();
  }
  void start ();
  void record (); // record elapsed time
  void check_start ();  // check if timer has been started
  void check_record (); // check if elapsed time has been recorded.
  double elapsed_seconds (); // return elapsed time at most recent record ()
  double seconds (); // record and return elapsed seconds
  void timer_label(const std::string& label) {  timer_label_ = label;}
  std::string& timer_label() { return timer_label_;}

  double to_milliseconds(double s);
  double to_minutes(double s);
  double to_hours(double s);

  double milliseconds() {return to_milliseconds(seconds()); }
  double minutes()  {return to_minutes(seconds()); }
  double hours() {return to_hours(seconds()); }

  double split_seconds() {return get_split_seconds_(); }
  double split_milliseconds() {return to_milliseconds(get_split_seconds_()); }
  double split_minutes() {return to_minutes(get_split_seconds_()); }
  double split_hours() {return to_hours(get_split_seconds_()); }
  void total_and_split_seconds(double *total, double *split);

  // return a string like "# cpu time = 1.0 s";
  std::string string_seconds() {return make_string(seconds(), "s");}
  std::string string_milliseconds() {return make_string(milliseconds(), "ms");}
  std::string string_minutes() {return make_string(minutes(), "m");}
  std::string string_hours() {return make_string(hours(), "h");}
  void time_string_prefix(std::string prefix) { cpu_time_string_prefix_ = prefix; }
  std::string time_string_prefix() { return cpu_time_string_prefix_; }

  //  std::ostream& print_seconds (std::ostream& out) const;
  std::ostream& print_seconds (std::ostream& out = std::cout) { out << string_seconds() << "\n"; return out; }
  std::ostream& print_milliseconds (std::ostream& out = std::cout) { out << string_milliseconds() << "\n"; return out; }
  std::ostream& print_minutes (std::ostream& out = std::cout) { out << string_minutes() << "\n"; return out; }
  std::ostream& print_hours (std::ostream& out = std::cout) { out << string_hours() << "\n"; return out; }

  // return a string like "# cpu time = 1.0 s";
  std::string string_split_seconds() {return make_split_string(split_seconds(), "s");}
  std::string string_split_milliseconds() {return make_split_string(split_milliseconds(), "ms");}
  std::string string_split_minutes() {return make_split_string(split_minutes(), "m");}
  std::string string_split_hours() {return make_split_string(split_hours(), "h");}
  void split_time_string_prefix(std::string prefix) { cpu_split_time_string_prefix_ = prefix; }
  std::string split_time_string_prefix() { return cpu_split_time_string_prefix_; }

  //  std::ostream& print_split_seconds (std::ostream& out) const;
  std::ostream& print_split_seconds (std::ostream& out = std::cout) { out << string_split_seconds() << "\n"; return out; }
  std::ostream& print_split_milliseconds (std::ostream& out = std::cout) { out << string_split_milliseconds() << "\n"; return out; }
  std::ostream& print_split_minutes (std::ostream& out = std::cout) { out << string_split_minutes() << "\n"; return out; }
  std::ostream& print_split_hours (std::ostream& out = std::cout) { out << string_split_hours() << "\n"; return out; }

  class OneSplit {
  public:
    OneSplit(CpuTimer *timer, const std::string& label) {
      label_ = std::string(label); // copy
      time_ = timer->get_split_timeval_();
    }
    OneSplit(CpuTimer *timer, size_t peakrss, size_t currentrss,
             const std::string& label) {
      label_ = std::string(label); // copy
      time_ = timer->get_split_timeval_();
      peakRSS_ = peakrss;
      currentRSS_ = currentrss;
    }
    void save_split_time(const struct timeval & t) {time_ = t;}
    void save_peakRSS(size_t peakrss) {peakRSS_ = peakrss;}
    void save_currentRSS(size_t currentrss) {currentRSS_ = currentrss;}

    void print_split(CpuTimer *timer, std::ostream& out = std::cout) {
      out << std::left << std::setw(11) << timer->timer_label() << " ";
      out.precision(3);
      if (timer->is_enabled_RSS()) {
        out << std::left << std::setw(5) << CpuTimer::rss_to_MB(peakRSS_) << " "
            << std::left << std::setw(5) << CpuTimer::rss_to_MB(currentRSS_) << " ";
      }
      //      out.precision(3);
      out << std::left << std::setw(6)  << std::scientific <<
        CpuTimer::timeval_to_seconds(time_) << "  " << label_ << "\n";
    }
  private:
    struct timeval time_;
    std::string label_ = "";
    size_t peakRSS_;
    size_t currentRSS_;
  };

  class SaveSplits {
  public:
    void save_split(CpuTimer *timer, const std::string& label) {
      if ( timer->is_enabled_save_splits() ) {
        auto sp = OneSplit(timer,label);
        if ( timer->is_enabled_RSS() ) {
          sp.save_peakRSS(getPeakRSS());
          sp.save_currentRSS(getCurrentRSS());
        }
        splits_.push_back(sp);
        if (timer->is_enabled_print_immediately()) sp.print_split(timer);
      }
    }
    void print_splits(CpuTimer *timer, std::ostream& out = std::cout) {
      if(timer->is_enabled_save_splits()) {
      out << "\nSplit times: " << timer->timer_label() << "\n";
      for( OneSplit & sp : splits_ ) sp.print_split(timer); }
    }
  private:
    std::vector<OneSplit> splits_;
  };

  void no_save_split() {if (is_enabled_save_splits()) get_split_timeval_(); }
  void save_split(const std::string& label) {saved_splits_.save_split(this,label);}
  void print_splits() { saved_splits_.print_splits(this);}

  inline void enable() {disable_save_splits(); disable_ = false;}
  inline void disable() {disable_save_splits(); disable_ = true;}
  inline bool is_disabled() {return disable_;}

  void enable_save_splits() { enable_save_splits_ = true;}
  void disable_save_splits() { enable_save_splits_ = false;}
  bool is_enabled_save_splits() { return enable_save_splits_;}

  void enable_print_immediately() { enable_print_immediately_ = true;}
  void disable_print_immediately() { enable_print_immediately_ = false;}
  bool is_enabled_print_immediately() { return enable_print_immediately_;}

  void enable_RSS() { enable_RSS_ = true;}
  void disable_RSS() { enable_RSS_ = false;}
  bool is_enabled_RSS() { return enable_RSS_;}

  // or ordinary functions in a namespace
  static double timeval_to_seconds (const struct timeval &);
  static double rss_to_MB (size_t r) {return ((double)r)/1e6;} // RSS in bytes to megabytes

private:

  void   addto_timeval_ (const struct timeval &, struct timeval *  ) const;
  int timeval_subtract_ (const struct timeval &, const struct timeval &,
                         struct timeval *) const;
  double get_split_seconds_();
  struct timeval get_split_timeval_();
  std::string make_string (double cpu_time, std::string units);
  std::string make_split_string (double cpu_time, std::string units);

  bool timer_started_ = false;
  bool timer_recorded_ = false;
  struct timeval start_cpu_time_;
  struct timeval total_cpu_time_;
  struct timeval last_cpu_time_;
  std::string cpu_time_string_prefix_ = "# cpu time = ";
  std::string cpu_split_time_string_prefix_ = "# cpu split time = ";
  SaveSplits saved_splits_;
  std::string timer_label_ = "";
  bool enable_save_splits_ = true; // if disabled, calls to save do nothing
  bool enable_print_immediately_ = true; // don't just store them for later, print when stored
  bool enable_RSS_ = true;
  bool disable_ = false;

}; /* End class CpuTimer  */

class ClockTimer {
public:
  void start() { wall_clock_start_ = std::time(NULL); }
  void record() { wall_clock_stop_ = std::time(NULL); }
  std::time_t elapsed() { return wall_clock_stop_ - wall_clock_start_; }

  double elapsed_minutes() { return elapsed() / 60.0; }

  std::string clock_start_string() const { return ctime(&wall_clock_start_); }
  std::string clock_stop_string() const { return ctime(&wall_clock_stop_); }

  /*
    double clock_difference () const {return difftime(wall_clock_stop_, wall_clock_start_);}
    double clock_record_elapsed_seconds() const {wall_clock_stop_record();
    return difftime(wall_clock_stop_, wall_clock_start_);}
  */

  /*
  std::string clock_string_seconds() {return make_clock_string(seconds(), "s");}
  std::string clock_string_milliseconds() {return make_clock_string(milliseconds(), "ms");}
  std::string clock_string_minutes() {return make_clock_string(minutes(), "m");}
  std::string clock_string_hours() {return make_clock_string(hours(), "h");}
  */

private:
  std::time_t wall_clock_start_;
  std::time_t wall_clock_stop_;
  std::string clock_time_string_prefix_ = "# clock time = ";
  std::string make_clock_string (double clock_time, std::string units);

};  /* End class ClockTimer  */

class Timer {
public:
  Timer() {this->start();}
  Timer(bool start) { if (start) this->start(); }
  void start () { cpu_.start(); }
  CpuTimer & cpu () { return cpu_; }
  ClockTimer & clock () { return clock_; }

  // Return hostname, but only up to dot, if there is one
  const std::string &  hostname() {
    gethostname(hostname_char_,1000);
    auto hn = std::string(hostname_char_);
    auto pos = hn.find(".");
    if (pos > hn.length()) {
      hostname_string_.assign(hn);
      return  hostname_string_;
    }
    auto short_string = hn.substr(0,pos);
    hostname_string_.assign(short_string);
    return  hostname_string_;
  }

private:
  CpuTimer cpu_;
  ClockTimer clock_;
  char hostname_char_[1000];
  std::string hostname_string_;
}; /* End class Timer  */

#endif
