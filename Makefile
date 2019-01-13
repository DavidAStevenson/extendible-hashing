
LIBDIR  := lib
CATCH2  := catch2test
TESTDIR := test

INCDIRS := -I$(LIBDIR)

#
# build a catch2 test executable
#

# # bit_op_lib.cc  bit_op_lib.h  bit_op_lib.o  bit_op_lib_test.cc
CXXFLAGS = -std=c++14 -Wall -Wextra

default: $(CATCH2)

bit_op_lib.o: $(LIBDIR)/bit_op_lib.cc $(LIBDIR)/bit_op_lib.h
	$(CXX) $(CXXFLAGS) -c $<

CATCHSRC := $(wildcard $(TESTDIR)/*catch2_test.cc)
CATCHOBJ := $(patsubst $(TESTDIR)/%.cc,%.o,$(CATCHSRC))

$(CATCH2): bit_op_lib.o $(CATCHOBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(CATCHOBJ): $(CATCHSRC)
	$(CXX) $(CXXFLAGS) $(INCDIRS) -c $<

clean:
	$(RM) $(CATCH2) $(CATCHOBJ) *.o

