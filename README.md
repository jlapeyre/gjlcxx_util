## Utility routines and classes for C/C++

These tools are [were] for things I use repeatedly, mostly for Monte Carlo simulations. My need
for this code has been, for the most part, made obsolete by Julia.

See the comments in template headers [cpp_headers/](c_cpp_problems/) for descriptions
and usage.

Also, in [c_cpp_problems/](c_cpp_problems/) I have recorded the solution to problems
I encountered so that I don't have to solve them again.

### Use

See for instance `scripts/gjl_gen_header --man`. This perl script reads a specification
file of parameters (for, say, Monte Carlo) and writes a C++ class that allows setting the values and
printing them in a nice way in the header of a data file.

### Dependencies

Google C++ test framework. This is now available as a debian package

### Build and Install

Build with `make all`. Test with `make test`.

See INSTALL for changing the following

library:
We install a static library to
/usr/local/lib/libgjlutils.a

headers:
And put below /usr/local/include/

binaries:
/usr/local/bin
