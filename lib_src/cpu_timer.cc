#include "gjl/cpu_timer.h"
/*****************************************************
* Copyright 2014 John Lapeyre                        *
*                                                    *
* You can redistribute this software under the terms *
* of the GNU General Public License version 3        *
* or a later version.                                *
******************************************************/

  // multiply by t in seconds to get t in xxx
constexpr static double to_milliseconds_ = 1e3;
constexpr static double to_minutes_ = 1/60.0;
constexpr static double to_hours_ = to_minutes_/ 60.0;
//constexpr static double to_days_ = to_hours_ / 24; ?

/* Note this are not member variables! */
static double _to_milliseconds(double s) {return to_milliseconds_ * s; }
static double _to_minutes(double s) {return to_minutes_ * s ; }
static double _to_hours(double s) {return to_hours_ * s; }

double CpuTimer::to_milliseconds(double s) {return _to_milliseconds(s);}
double CpuTimer::to_minutes(double s) {return _to_minutes(s);}
double CpuTimer::to_hours(double s) {return _to_hours(s);}

/* Start the timer. This simply records the current total cpu usage for
 * the process so it can be subtracted from values recorded later.
 */

void CpuTimer::start () {
  struct rusage myrusage;
  getrusage(RUSAGE_SELF, &myrusage);
  start_cpu_time_ = myrusage.ru_utime;
  last_cpu_time_ = myrusage.ru_utime;
  timer_started_ = true;
}

void CpuTimer::check_start () {
  if (is_disabled()) return;
  if ( ! timer_started_ ) {
    std::cerr << "Error: Attempt to record elapsed time before starting cpu timer." << "\n";
    exit(1);
  }
}

void CpuTimer::check_record () {
  if (is_disabled()) return;
  check_start();
  if ( ! timer_recorded_ ) {
    std::cerr << "Error: Timer started but elapsed time not recorded." << "\n";
    exit(1);
  }
}

/*
 * Record the time elapsed since start was called.
 */
void CpuTimer::record () {
  if (is_disabled()) return;
  check_start();
  struct rusage myrusage;
  struct timeval *cpu_time;
  getrusage(RUSAGE_SELF, &myrusage);
  cpu_time = &(myrusage.ru_utime);
  last_cpu_time_ = total_cpu_time_;
  timeval_subtract_(*cpu_time, start_cpu_time_,&total_cpu_time_); // subtract start time
  timer_recorded_ = true;
}

double CpuTimer::seconds () {
  if (is_disabled()) return 0;
  record();
  return elapsed_seconds();
}

struct timeval CpuTimer::get_split_timeval_ () {
  struct timeval split_time;
  if (! is_disabled()) {
    record();
    timeval_subtract_(total_cpu_time_,
                      last_cpu_time_,&split_time);
  }
  return split_time;
}

double CpuTimer::get_split_seconds_ () {
  if (is_disabled()) return 0;
  return timeval_to_seconds(get_split_timeval_());
}

/* Return both total and split via arguments */
void CpuTimer::total_and_split_seconds (double *total , double *split) {
  if (is_disabled()) return;
  struct timeval return_time;
  record();
  timeval_subtract_(total_cpu_time_,
                    last_cpu_time_, &return_time);
  *split = timeval_to_seconds(return_time);
  timeval_subtract_(total_cpu_time_,
                    start_cpu_time_, &return_time);
  *total = timeval_to_seconds(return_time);
}

// const was legal and useful here, but I don't know how to declare it in .h w/o error.
// return to this later.
double CpuTimer::elapsed_seconds () {
  if (is_disabled()) return 0;
  check_record();
  return timeval_to_seconds(total_cpu_time_);
}

/****************************************************************************
 * Utility routines
 ****************************************************************************
 */

/* Not currently used, but may be useful */
/* does:  result += t  */
void CpuTimer::addto_timeval_ (const struct timeval &t, struct timeval *result) const {
  int usum;
  usum = result->tv_usec + t.tv_usec;
  if ( usum > 1000000) {
    result->tv_sec ++;
    usum -= 1000000;
  }
  result->tv_sec += t.tv_sec;
  result->tv_usec = usum;
}

/* grabbed this off net
 compute x - y --> result
*/
int CpuTimer::timeval_subtract_ (const struct timeval &x,
                                 const struct timeval &y, struct timeval *result) const
{
  auto yusec = y.tv_usec;
  auto ysec = y.tv_sec;
  /* Perform the carry for the later subtraction by updating y. */
  if (x.tv_usec < yusec) {
    int nsec = (yusec - x.tv_usec) / 1000000 + 1;
    yusec -= 1000000 * nsec;
    ysec += nsec;
  }
  if (x.tv_usec - yusec > 1000000) {
    int nsec = (x.tv_usec - yusec) / 1000000;
    yusec += 1000000 * nsec;
    ysec -= nsec;
  }
  /* Compute the time remaining to wait.
     tv_usec is certainly positive. */
  result->tv_sec = x.tv_sec - ysec;
  result->tv_usec = x.tv_usec - yusec;
  /* Return 1 if result is negative. */
  return x.tv_sec < ysec;
}

/* convert timeval struct to double representing seconds. */
double CpuTimer::timeval_to_seconds (const struct timeval &t) {
  return ( (double)(t.tv_sec) + (double)(t.tv_usec)/1000000);
}

std::string CpuTimer::make_string (double cpu_time, std::string units) {
  if (is_disabled()) return std::string("");
  std::ostringstream oss;
  oss << cpu_time_string_prefix_ << cpu_time << " " << units;
  return oss.str();
}

std::string CpuTimer::make_split_string (double cpu_time, std::string units) {
  if (is_disabled()) return std::string("");
  std::ostringstream oss;
  oss << cpu_split_time_string_prefix_ << cpu_time << " " << units;
  return oss.str();
}

/* Not used I think. Reall like a static function */
std::string ClockTimer::make_clock_string (double clock_time, std::string units) {
  //  if (is_disabled()) return std::string("");
  std::ostringstream oss;
  oss << clock_time_string_prefix_ << clock_time << " " << units;
  return oss.str();
}
