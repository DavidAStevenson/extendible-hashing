
LIBDIR  := lib
CATCH2  := catch2test
TESTDIR := test
TESTSRC := $(wildcard $(TESTDIR)/*.cc)
TESTOBJ := $(patsubst $(TESTDIR)/%.cc,%.o,$(TESTSRC))

INCDIRS := -I$(LIBDIR)

#
# build a catch2 test executable
#

# # bit_op_lib.cc  bit_op_lib.h  bit_op_lib.o  bit_op_lib_test.cc
CC = g++
CFLAGS = -g -Wall -std=c++11

default: $(CATCH2)

$(CATCH2): bit_op_lib.o $(TESTOBJ)
	$(CC) $(CFLAGS) $^ -o $@

$(TESTOBJ): $(TESTDIR)/bit_op_lib_catch2_test.cc
	$(CC) $(CFLAGS) $(INCDIRS) -c $<

bit_op_lib.o: $(LIBDIR)/bit_op_lib.cc $(LIBDIR)/bit_op_lib.h
	$(CC) $(CFLAGS) -c $<

clean:
	$(RM) $(CATCH2) $(TESTOBJ) *.o

