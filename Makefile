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
	$(CC) $(CFLAGS) -c bit_op_lib_test.cc

bit_op_lib.o: bit_op_lib.cc bit_op_lib.h
	$(CC) $(CFLAGS) -c bit_op_lib.cc

clean:
	$(RM) tester *.o *~

