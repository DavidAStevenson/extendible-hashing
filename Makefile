
LIBDIR  := lib

CATCH2  := catch2test
GTEST   := gtest

TESTDIR := test

INCDIRS := -I$(LIBDIR)

# # bit_op_lib.cc  bit_op_lib.h  bit_op_lib.o  bit_op_lib_test.cc
CXXFLAGS = -std=c++14 -Wall -Wextra

default: $(CATCH2) $(GTEST)

bit_op_lib.o: $(LIBDIR)/bit_op_lib.cc $(LIBDIR)/bit_op_lib.h
	$(CXX) $(CXXFLAGS) -c $<

#------------------------------------------------------------------------------
#
# build a catch2 test executable
#
CATCHSRC := $(wildcard $(TESTDIR)/*catch2_test.cc)
CATCHOBJ := $(patsubst $(TESTDIR)/%.cc,%.o,$(CATCHSRC))

$(CATCH2): bit_op_lib.o $(CATCHOBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(CATCHOBJ): $(CATCHSRC)
	$(CXX) $(CXXFLAGS) $(INCDIRS) -c $<

#------------------------------------------------------------------------------
#
# build a googletest executable
#
GTEST_DIR := test/gtest

$(GTEST): gtest-all.o

gtest-all.o: $(GTEST_DIR)/gtest-all.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(CATCH2) $(CATCHOBJ) *.o
	$(RM) gtest-all.o *.o

