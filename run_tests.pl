#!/usr/bin/env perl
use v5.14.0;
# John Lapeyre Sun Nov  9 15:28:03 CET 2014
# Run some test code.
# run_tests_for_stderr() should print the command line that runs each
# test program, and nothing else if there are no errors. Most of these
# programs do not check for correctness; they just run or do not.
# test_hist_pdf actually prints to stderr if tests fail.

my $test_dir = "./test_src";
my @tests = qw( test_hist_pdf simple_linear_regression_test test_cpu_timer vec2d test_arr_irreg);

sub dosys {
    my $c = shift;
    say STDOUT $c;
    system $c;
}

sub exepath {
    my ($exe) = @_;
    return $test_dir . '/' .  $exe;
}

sub one_test_for_stderr {
    my ($exe) = @_;
    dosys( exepath($exe) . ' > /dev/null');
}

sub run_tests_for_stderr {
    foreach my $test (@tests) {
        one_test_for_stderr($test);
    }
}

run_tests_for_stderr();
