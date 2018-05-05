NO_OPTIMIZATION_YES_DEBUGGING = 0
USE_CLANG_NOT_GCC = 0

# google unit testing library
HAVE_GTEST = 0

############################################
# Section 1  Exectuables and source files
############################################

TEST_SRC = ./test_src
LIB_SRC = ./lib_src

# Executables each of which builds from a single source and object file.
# The rules for these are defined in Secton 3
EXECUTABLES_WITH_SINGLE_SOURCE = $(TEST_SRC)/simple_linear_regression_test \
    $(TEST_SRC)/test_hist_pdf $(TEST_SRC)/test_arr_irreg

ifeq ($(HAVE_GTEST), 1)
EXECUTABLES_WITH_SINGLE_SOURCE += hist_pdf_unittest
endif

BROKEN_AND_UNUSED1 = 

# Executables each of which builds from  multiple object files.
# Specify object files below in Section 2
EXECUTABLES_WITH_MULT_SOURCE_FILES = $(TEST_SRC)/test_cpu_timer $(TEST_SRC)/vec2d

# stem of other object files
OTHER_SOURCES = cpu_timer

# For util library
LIBGJLUTILS_SRC = $(shell find ./lib_src -name '*.cc' -o -name '*.c')
LIBGJLUTILS_TEMP = $(LIBGJLUTILS_SRC:%.cc=%.o)
LIBGJLUTILS_OBJ = $(LIBGJLUTILS_TEMP:%.c=%.o)


########################################################################################
# Section 2  Dependencies for executables with multiple object files
########################################################################################
# This will be the first rule, default target
$(TEST_SRC)/test_cpu_timer : $(TEST_SRC)/test_cpu_timer.o $(LIB_SRC)/cpu_timer.o
$(TEST_SRC)/vec2d : $(TEST_SRC)/vec2d.o $(LIB_SRC)/cpu_timer.o

########################################################################################
# Section 3  Build flags that we may want to change
########################################################################################
GTEST_SRC = /home/jlapeyre/software_source/gtest/gtest-1.7.0
INC_LOCAL = -I/usr/local/include/c++
INC_BUILD = -I./cpp_headers -I.
INC = $(INC_BUILD) $(INC_LOCAL) -I$(GTEST_SRC)/include
ifeq ($(NO_OPTIMIZATION_YES_DEBUGGING), 1)
OPTFLAGS =  -march=native -g
else
OPTFLAGS =  -march=native -O3
endif
STDFLAGS = -Wall
COLLFLAGS = $(STDFLAGS) $(OPTFLAGS) $(INC)
CFLAGS =  -Winline --inline-limit=700000 -DHAVE_SSE2 -std=c99 $(COLLFLAGS)
CXXFLAGS = -std=c++11 $(COLLFLAGS)  -fopenmp

########################################################################################
# Section 4  Build flags that we change less often
########################################################################################
#LDFLAGS = -lstdc++ -lm -fopenmp $(GTEST_SRC)/libgtest.a
ifeq ($(HAVE_GTEST), 1)
LDFLAGS =  -lstdc++ -lm -fopenmp -lgtest
LDLIBS = -L$(GTEST_SRC) -lgtest
else
LDFLAGS =  -lstdc++ -lm -fopenmp
endif
SHELL = /bin/sh

ifeq ($(USE_CLANG_NOT_GCC), 1)
CC = clang
CXX = clang++
else
CC = cc
CXX = c++
endif

AR = ar
ARFLAGS = rcs
########################################################################################
# Section 5  Set up automatic rules
#   All build rules are either built in or specified here.
########################################################################################
# generate rules for executables each with single source file defined in Section 1
$(foreach exe,$(EXECUTABLES_WITH_SINGLE_SOURCE), $(eval $(exe) : $(exe).o))

########################################################################################
# Section 6  Create filenames and groups of filenames.
########################################################################################
# all executables, from single and multiple sources. These are defined in Section 1
ALL_BINARY_EXECUTABLES = $(EXECUTABLES_WITH_SINGLE_SOURCE) $(EXECUTABLES_WITH_MULT_SOURCE_FILES) 
# all object files
OBJECT_FILES := $(patsubst %,%.o,$(ALL_BINARY_EXECUTABLES)) $(patsubst %,%.o,$(OTHER_SOURCES))
# Automatic dependency files. These are used in Section 9, but are disabled currently
DEPFILES := $(OBJECT_FILES:.o=.d)

ALL_LIBS = libgjlutils.a

# careful do not include in ALL_BINARY_EXECUTABLES, they will be removed
ALL_SCRIPTS = log_data_file gjl_gen_header

# This list is only used for installation
ALL_CPP_HEADERS = cpu_timer.h hist_pdf.h lodepng.h simple_linear_regression.h graph.h \
        seedfill.h filesystem.h num_util.h image.h percolation.h log_space.h simp_stat.h \
	data_header.h arr_irreg.h

# directory in this distribution
CPP_HEADERS_SRC = ./cpp_headers/gjl

MAIN_CPP_HEADER = gjlutils.h

ALL_C_HEADERS = getRSS.h

########################################################################################
# Section 7  Set installation directories
########################################################################################

INSTALL_PREFIX = /usr/local
INSTALL_LIB = lib
INSTALL_BIN = bin
INSTALL_CPP_HEADERS = include/c++/gjl
INSTALL_MAIN_CPP_HEADER = include/c++
INSTALL_C_HEADERS = include

INSTALL_LIB_PATH = $(INSTALL_PREFIX)/$(INSTALL_LIB)
INSTALL_BIN_PATH = $(INSTALL_PREFIX)/$(INSTALL_BIN)
INSTALL_CPP_HEADERS_PATH = $(INSTALL_PREFIX)/$(INSTALL_CPP_HEADERS)
INSTALL_MAIN_CPP_HEADER_PATH = $(INSTALL_PREFIX)/$(INSTALL_MAIN_CPP_HEADER)
INSTALL_C_HEADERS_PATH = $(INSTALL_PREFIX)/$(INSTALL_C_HEADERS)

########################################################################################
# Section 8  Targets all and clean
#######################################################################################
#.PHONY: all
all : $(ALL_BINARY_EXECUTABLES) $(ALL_LIBS)
	@echo Built $(ALL_BINARY_EXECUTABLES) $(ALL_LIBS)

.PHONY: clean
clean:
	-rm -f $(ALL_BINARY_EXECUTABLES) $(ALL_LIBS) $(OBJECT_FILES) $(DEPFILES)

.PHONY: realclean
realclean : clean
	-rm -f *.o */*.o *.a *.so *_cmd.c *_cmd.h a.out

########################################################################################
# Section 9  Some explicit rules
########################################################################################

gtest_hist_pdf : hist_pdf_unittest
	./hist_pdf_unittest

libgjlutils.a : $(LIBGJLUTILS_OBJ)
#	cd $(LIBGJLUTILS_SRC)
	$(AR) $(ARFLAGS) $@ $(LIBGJLUTILS_OBJ)

install : $(ALL_LIBS)
	mkdir -p $(INSTALL_LIB_PATH)
	mkdir -p $(INSTALL_BIN_PATH)
	mkdir -p $(INSTALL_MAIN_CPP_HEADER_PATH)
	mkdir -p $(INSTALL_CPP_HEADERS_PATH)
	mkdir -p $(INSTALL_C_HEADERS_PATH)
	cp -a $(ALL_LIBS) $(INSTALL_LIB_PATH)
	cp -a $(MAIN_CPP_HEADER) $(INSTALL_MAIN_CPP_HEADER_PATH)
#	cp -a $(ALL_CPP_HEADERS) $(INSTALL_CPP_HEADERS_PATH)
	for header in $(ALL_CPP_HEADERS); \
          do echo cp -a $(CPP_HEADERS_SRC)/$$header $(INSTALL_CPP_HEADERS_PATH)/$$header;\
                  cp -a $(CPP_HEADERS_SRC)/$$header $(INSTALL_CPP_HEADERS_PATH)/$$header;\
          done
	cp -a $(ALL_C_HEADERS) $(INSTALL_C_HEADERS_PATH)
	for prog in $(ALL_SCRIPTS); \
          do echo cp -a ./scripts/$$prog $(INSTALL_BIN_PATH)/$$prog;\
                  cp -a ./scripts/$$prog $(INSTALL_BIN_PATH)/$$prog;\
          done

#	cp -a $(ALL_LIBS) /usr/local/lib
#	cp -a $(ALL_CPP_HEADERS) /usr/local/include/c++
#	cp -a $(ALL_C_HEADERS) /usr/local/include

test :
	./run_tests.pl

########################################################################################
# Section 10  Explicit dependencies
########################################################################################

test_hist_pdf.o : $(CPP_HEADERS_SRC)/hist_pdf.h

hkperc.o : hkcluster.h hkperc_2d_site.h walk_2d.h perc_2d_site.h walkperc_opts.h

hist_pdf_unittest.o : hist_pdf.h

cpu_timer.o : $(CPP_HEADERS_SRC)/cpu_timer.h

simple_linear_regression_test.o : $(CPP_HEADERS_SRC)/simple_linear_regression.h

########################################################################################
# Section 11  Rules to create .c and .h files from gnu gengetopt input files
########################################################################################

GENGETOPT = gengetopt --file-name $*  <  $<; ./fix_cmdline_parser.pl $*.c 
%.c : %.ggo
	$(GENGETOPT)
%.h : %.ggo
	$(GENGETOPT)

########################################################################################
# Section 12  Automatic dependencies
########################################################################################
# Not used. Disabled, because these create lots of junk.
# Comment this out when we do dependencies manually
# automatic dependencies fill the dir with lots of garbage
# "-" silences complaints
# -include $(DEPFILES)
# Needs refactoring
%.d: %.cc
	@set -e; rm -f $@; \
	$(CXX) -M $(CPPFLAGS) $(CXXFLAGS) $(INC) $(LDFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -M $(CPPFLAGS) $(CFLAGS) $(INC) $(LDFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
########################################################################################
