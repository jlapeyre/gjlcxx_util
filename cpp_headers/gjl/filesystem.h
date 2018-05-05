// -*-c++-*-
#ifndef GJL_FILESYSTEM_H
#define GJL_FILESYSTEM_H

#include <stdio.h>
#include <iostream>
#include <sys/stat.h>
#include <string>
#include <vector>
#include <gjl/num_util.h>

/*****************************************************
* Copyright 2014 John Lapeyre                        *
*                                                    *
* You can redistribute this software under the terms *
* of the GNU General Public License version 3        *
* or a later version.                                *
******************************************************/

/*
 * Functions and classes for filesystem operations for scientific data files.
 * This is GNU/Linux - centric code.
 *
 * gjl::fopen and gjl::mkdir -- are replacements that perform checks and give verbose error reports.
 * 
 * class gjl::Filename -- manages filenames for output data files. This
 * includes path, suffix (extension), numerical suffixes that may be
 * incremented when writing a series of data files. This includes support for an array of numerical
 * suffixes that may be incremented independently and appended, eg, as datafile.x.y.z
 *
 */


namespace gjl {

  const char dir_sep = '/';

  /*** fopen *************************************************************/

  inline FILE * fopen(const char *file, const char *mode) {
    FILE *fp = std::fopen(file,mode);
    if (fp) return fp;
    std::cerr<< "Can't open file '" << file << "' in mode '" << mode << "'\n";
    if (errno == EINVAL)
      std::cerr<< "Invalid mode (EINVAL).\n";
    abort();
  }

  inline FILE * fopen(const std::string& file, const char *mode) {return fopen(file.c_str(),mode);}

  /*** mkdir *************************************************************/

  const std::string mkdir_error_(int err) {
    if (err == EACCES) return std::string
    ("Search permission is denied on a component of the path prefix, or write "
     "permission is denied on the parent directory of the directory to be created.");

    if (err == ELOOP) return std::string
       ("A loop exists in symbolic links encountered during resolution of the path argument.");

    if (err == EMLINK) return std::string
      ("The link count of the parent directory would exceed {LINK_MAX}.");
 
   if (err == ENAMETOOLONG) return std::string
     ("The length of the path argument exceeds {PATH_MAX} or a pathname component is longer than {NAME_MAX}.");

    if (err == ENOENT) return std::string
                 ("A component of the path prefix specified by path does not "
                  "name an existing directory or path is an empty string.");

    if (err == ENOSPC) return std::string
     ("The file system does not contain enough space to hold the contents "
      "of the new directory or to extend the parent directory of the new directory.");

    if (err == ENOTDIR) return std::string
                         ("A component of the path prefix is not a directory.");

    if (err == EROFS) return std::string
                        ("The parent directory resides on a read-only file system.");

    if (err == EEXIST) return std::string
                         ("The named file exists.");

    return std::string("unknown error");    
  }

  inline int mkdir(const char *dir, int mode = S_IRWXU) { // check for a good default!
    auto result = ::mkdir(dir,mode);
    if (result == 0 || (result == -1 &&  errno == EEXIST)) return result;
    std::cerr << "Can't create directory '" << dir << "'.\n";
    std::cerr << mkdir_error_(errno) << "\n";
    abort();
  }

  inline int mkdir(const std::string& dir, int mode = S_IRWXU) {return mkdir(dir.c_str(),mode);}

  /* END  mkdir */

  /*** class Filename *************************************************************/
  
  class Filename {
  public:
    typedef int run_number_t;

    Filename() { set_number_of_run_numbers(0); }

    void set_number_of_run_numbers(size_t n) {
      run_numbers_.clear();
      run_numbers_.resize(n);
      initialize_run_numbers();
    }

    void initialize_run_numbers(run_number_t initial_value=0) {gjl::assign(run_numbers_, initial_value);}
    void increment_run_number(size_t ind=0) {++run_numbers_[ind];}

    void set_base(const std::string& s) { base_ = s;}
    void set_run_suffix(const std::string& s) { run_suffix_ = s;} // per run suffix
    void set_run_number(size_t ind, run_number_t n) { run_numbers_[ind] = n;}
    void set_run_number(run_number_t n) { set_run_number(0,n);}
    void set_suffix(const std::string& s) { suffix_ = s;} // final suffix

    void set_directory(const char * s) {set_directory(std::string(s));}
    void set_directory(const std::string& s) { 
      directory_ = s;
      if (directory_.back() != dir_sep)
        directory_.append(&dir_sep);
    }
    std::string& get_directory() { return directory_;}

    void set_base(const char * s) { set_base(std::string(s));}
    void set_run_suffix(const char * s) { set_run_suffix(std::string(s));}
    void set_suffix(const char * s) { set_suffix(std::string(s));}
    
    std::string full_pathname () {
      std::string run_number_str = "";
      for(size_t i=0; i < run_numbers_.size(); ++i) {
        run_number_str.append(run_number_sep_);
        run_number_str.append(std::to_string(run_numbers_[i]));
      }
      std::string run_suffix_full;
      if ( run_suffix_.length() == 0)  
        run_suffix_full = "";
      else  run_suffix_full = run_suffix_sep_ + run_suffix_;
      return directory_ + base_ + run_suffix_full  + run_number_str + suffix_;
    }

  private:
    std::string base_ = "";
    std::string run_suffix_ = "";
    std::string suffix_ = ".txt";
    std::string directory_ = "./";
    std::vector<int> run_numbers_;
    
    std::string run_suffix_sep_ = "_";
    std::string run_number_sep_ = "_";

  };  /* END class Filename */
  /*** END class Filename *************************************************************/

} /* END namespace gjl */

#endif
