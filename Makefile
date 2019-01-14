
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
CATCHSRC  := $(wildcard $(TESTDIR)/*catch2*.cc)
CATCHOBJ  := $(patsubst $(TESTDIR)/%.cc,%.o,$(CATCHSRC))

$(CATCH2): bit_op_lib.o $(warning CATCHOBJ is $(CATCHOBJ)) $(CATCHOBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(CATCHOBJ): %.o: $(TESTDIR)/%.cc
	$(warning catch2 object pattern commands)
	$(CXX) $(CXXFLAGS) $(INCDIRS) -c $<

#------------------------------------------------------------------------------
#
# build a googletest executable
#
GTEST_DIR := test/gtest

GTESTSRC  := $(wildcard $(TESTDIR)/*gtest.cc)
GTESTOBJ  := $(patsubst $(TESTDIR)/%.cc,%.o,$(GTESTSRC))
GTESTDEPS := $(patsubst $(TESTDIR)/%.cc,%.d,$(GTESTSRC))

LDFLAGS := -pthread
$(GTEST): gtest-all.o bit_op_lib.o $(GTESTOBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

gtest-all.o: $(GTEST_DIR)/gtest-all.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

GTESTINC := -Itest/gtest
$(GTESTOBJ): %.o: $(TESTDIR)/%.cc
	$(warning gtest object pattern commands)
	$(CXX) $(CXXFLAGS) $(INCDIRS) $(GTESTINC) -c $< -o $@

%.d: $(TESTDIR)/%.cc
	$(CXX) $(INCDIRS) -MM $< \
		| sed -e '1 s%^%$@ %' \
		> $@

-include $(GTESTDEPS)

clean:
	$(RM) $(CATCH2) $(CATCHOBJ) *.o
	$(RM) $(GTEST) $(GTESTOBJ) $(GTESTDEPS) gtest-all.o *.o

