
LIBDIR := lib
INCDIRS := -I$(LIBDIR)
#
# build a catch2 test executable
#

# # bit_op_lib.cc  bit_op_lib.h  bit_op_lib.o  bit_op_lib_test.cc
CC = g++
CFLAGS = -g -Wall -std=c++11

default: tester

tester: bit_op_lib.o bit_op_lib_test.o
	$(CC) $(CFLAGS) -o tester bit_op_lib.o bit_op_lib_test.o

bit_op_lib_test.o: bit_op_lib_test.cc
	$(CC) $(CFLAGS) $(INCDIRS) -c bit_op_lib_test.cc

bit_op_lib.o: $(LIBDIR)/bit_op_lib.cc $(LIBDIR)/bit_op_lib.h
	$(CC) $(CFLAGS) -c $<

clean:
	$(RM) tester *.o *~

