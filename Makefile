
LIBDIR  := lib

CATCH2  := catch2test
GTEST   := gtest

TESTDIR := test
OBJDIR  := obj

INCDIRS := -I$(LIBDIR)

# # bit_op_lib.cc  bit_op_lib.h  bit_op_lib.o  bit_op_lib_test.cc
CXXFLAGS = -std=c++14 -Wall -Wextra

dir_guard=@mkdir -p $(@D)

default: $(CATCH2) $(GTEST)

bit_op_lib.o: $(LIBDIR)/bit_op_lib.cc $(LIBDIR)/bit_op_lib.h
	$(CXX) $(CXXFLAGS) -c $<


#------------------------------------------------------------------------------
#
# build a catch2 test executable
#
CATCHSRC  := $(wildcard $(TESTDIR)/*catch2*.cc)
CATCHOBJ  := $(patsubst $(TESTDIR)/%.cc,$(OBJDIR)/%.o,$(CATCHSRC))

$(CATCH2): $(warning CATCHOBJ is $(CATCHOBJ)) bit_op_lib.o $(CATCHOBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(CATCHOBJ): $(OBJDIR)/%.o: $(TESTDIR)/%.cc
	$(dir_guard)
	$(CXX) $(CXXFLAGS) $(INCDIRS) -c $< -o $@

#------------------------------------------------------------------------------
#
# build a googletest executable
#
GTEST_DIR := test/gtest

GTESTSRC  := $(wildcard $(TESTDIR)/*gtest.cc)
GTESTOBJ  := $(patsubst $(TESTDIR)/%.cc,$(OBJDIR)/%.o,$(GTESTSRC))
GTESTDEPS := $(patsubst $(TESTDIR)/%.cc,$(OBJDIR)/%.d,$(GTESTSRC))

LDFLAGS := -pthread
$(GTEST): gtest-all.o bit_op_lib.o $(GTESTOBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

gtest-all.o: $(GTEST_DIR)/gtest-all.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

GTESTINC := -I$(GTEST_DIR)
$(GTESTOBJ): $(OBJDIR)/%.o: $(TESTDIR)/%.cc
	$(dir_guard)
	$(CXX) $(CXXFLAGS) $(INCDIRS) $(GTESTINC) -c $< -o $@

$(OBJDIR)/%.d: $(TESTDIR)/%.cc
	$(dir_guard)
	$(CXX) $(INCDIRS) -MM $< \
		| sed -e '1 s%^%$@ %' -e '1 s% % $(OBJDIR)/%' \
		> $@

-include $(GTESTDEPS)

clean:
	$(RM) $(CATCH2) $(CATCHOBJ) *.o
	$(RM) $(GTEST) $(GTESTOBJ) $(GTESTDEPS) gtest-all.o *.o

