#include "gtest/gtest.h"

#include "bit_op_lib.h"

TEST(ClearingBits, ClearLowestBit) {
  int results[] = {0,0,2,2,4,4,6,6};
  for (int i = 0; i < 8; i++) {
    int target = i;
    BitClear(target, 0);
    ASSERT_EQ(results[i], target);
  }
}

TEST(ClearingBits, ClearSecondLowestBit) {
  int results[] = {0,1,0,1,4,5,4,5};
  for (int i = 0; i < 8; i++) {
    int target = i;
    BitClear(target, 1);
    ASSERT_EQ(results[i], target);
  }
}

TEST(TestingBits, TestLowestBit) {
  int results[] = {0,1,0,1};
  for (int i = 0; i < 4; i++) {
    int actual = BitTest(i, 0);
    ASSERT_EQ(results[i], actual);
  }
}

TEST(TestingBits, TestSecondLowestBit) {
  int results[] = {0,0,1,1,0,0,1,1};
  for (int i = 0; i < 8; i++) {
    int actual = BitTest(i, 1);
    ASSERT_EQ(results[i], actual);
  }
}

TEST(SettingBits, SettingSingleBits) {
  int results[8] = {1,2,4,8,16,32,64,128};
  for (int bit = 0; bit < 8; bit++) {
    int target = 0;
    BitSet(target, bit);
    ASSERT_EQ(results[bit], target);
  }
  int target = 0;
  BitSet(target, 30);
  ASSERT_EQ(1073741824, target);

  target = 0;
  BitSet(target, 31);
  ASSERT_EQ(-2147483648, target);

//  target = 0;
//  BitSet(target, 31);
//  ASSERT_EQ(2147483648, target);
}

/*
TEST(GettingHighestBits, HighestTwoBitsOfFour) {
  int result = GetHighestBits(4, 2);
  ASSERT_EQ(2, result);
}

TEST(GettingHighestBits, HighestTwoBitsOfSix) {
  int result = GetHighestBits(6, 2);
  ASSERT_EQ(3, result);
}
*/

TEST(GettingHighestSetBit, HighestSetBitOfZero) {
  int actual = GetHighestSetBit(0);
  // -1 indicates no bits are set
  ASSERT_EQ(-1, actual);
}

TEST(GettingHighestSetBit, HighestSetBitOfOne) {
  int actual = GetHighestSetBit(1);
  ASSERT_EQ(0, actual);
}

TEST(GettingHighestSetBit, HighestSetBitOfTwo) {
  int actual = GetHighestSetBit(2);
  ASSERT_EQ(1, actual);
  actual = GetHighestSetBit(3);
  ASSERT_EQ(1, actual);
}

TEST(GettingHighestSetBit, HighestSetBitOfFour) {
  int actual = GetHighestSetBit(4);
  ASSERT_EQ(2, actual);
}

TEST(GettingHighestBits, SecondHighestBitof32bitInt)  {
  int target = 0;
  BitSet(target, 30);
  int actual = GetHighestSetBit(target);
  ASSERT_EQ(30, actual);
}

TEST(GettingHighestBits, HighestBitof32bitInt) {
  int target = 0;
  BitSet(target, 31);
  int actual = GetHighestSetBit(target);
  ASSERT_EQ(31, actual);
}

TEST(ReversingBits, ReverseBitsOfSix) {
  int actual = ReverseBits(6, 4);
  ASSERT_EQ(6, actual);

  actual = ReverseBits(6, 3);
  ASSERT_EQ(3, actual);
}
