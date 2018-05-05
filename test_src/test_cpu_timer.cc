#include <iostream>
#include <unistd.h>
#include "gjl/cpu_timer.h"
/*****************************************************
* Copyright 2014 John Lapeyre                        *
*                                                    *
* You can redistribute this software under the terms *
* of the GNU General Public License version 3        *
* or a later version.                                *
******************************************************/

/**********************************************************************
 *  Test cpu_timer code. Do not compile with optimization, because we
 *  want to consume cpu time.
 *********************************************************************/

typedef long int tint;

// Consume cpu time
tint  cpu_task ( tint n = 1 ) {
  tint running_total = 23;
  tint some_large_number = 50 * 1000 * 1000  * n;
  for (tint i=0; i < some_large_number; ++i)
    running_total = 37 * running_total + i;
  return running_total;
}

// Simplest use.
void test1 () {
  CpuTimer timer; // create and start timer
  cpu_task(2); // do some work
  timer.print_seconds(); // record and print elapsed seconds in format of comment line in data file.
  timer.print_minutes(); // or milliseconds, or hours.
}

void test2 () {
  CpuTimer timer; // create and start timer
  CpuTimer timer1(true); // also creates and starts timer
  CpuTimer timer2(false); // does not start timer
  timer2.start();         // in this case, the timer must be started manually.
  cpu_task(1); // do some work
  timer.print_seconds();
  std::cout << timer.string_seconds() << "\n";  // prints the same thing (or milliseconds,minutes,hours)
  timer2.print_seconds(std::cout); // specify output stream
  timer.print_milliseconds();
  timer.print_minutes();
  timer.print_hours();

  timer.time_string_prefix("The time is "); // change the output string
  timer.print_seconds();
}

void test3 () {
  CpuTimer timer;
  cpu_task(1);
  timer.record();  // record and retrieve elapsed time in two steps
  double t = timer.elapsed_seconds();
  std::cout << "elapsed time "  << t << " seconds\n";
}

void test4 () {
  CpuTimer timer;
  cpu_task(1);
  timer.print_seconds(); // cpu since timer started
  cpu_task(1);
  timer.print_seconds(); // cpu since timer started, i.e. sum of time for both calls to cpu_task()
  timer.start();
  timer.print_milliseconds(); // timer restarted, so elapsed time is zero
}

// Reading timer at multiple times and splits.
void test5 () {
  CpuTimer timer;
  cpu_task(1);
  timer.print_seconds(); // cpu since timer started
  cpu_task(1);
  timer.print_split_seconds(); // only time since last recorded elapsed time (ie print_seconds() )
  cpu_task(1);
  timer.print_split_seconds(); // only time since last split
  timer.print_seconds(); // cpu since timer started.
  timer.start();
  timer.print_milliseconds(); // timer restarted, so elapsed time is zero
}


int main () {
  test1();
  test2();
  test3();
  test4();
  test5();
}
