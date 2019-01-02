#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include "bit_op_lib.h"

TEST_CASE("Set lowest bit of 0 results in 1", "[single-file]") {
	int value = 0;
	BitSet(value, 0);
	REQUIRE(value == 1);

	value = 0;
	BitSet(value, 1);
	REQUIRE(value == 2);

	value = 0;
	BitSet(value, 2);
	REQUIRE(value == 4);

	value = 0;
	BitSet(value, 3);
	REQUIRE(value == 8);

	value = 0;
	BitSet(value, 4);
	REQUIRE(value == 16);

	value = 0;
	BitSet(value, 5);
	REQUIRE(value == 32);

	value = 0;
	BitSet(value, 6);
	REQUIRE(value == 64);

	value = 0;
	BitSet(value, 7);
	REQUIRE(value == 128);

	value = 0;
	BitSet(value, 30);
	REQUIRE(value == 1073741824);

	value = 0;
	BitSet(value, 31);
	REQUIRE(value == -2147483648);

	// 32 is out-of-range, and causes an assert triggered abort
	/*
	value = 0;
	BitSet(value, 32);
	REQUIRE(value == 0);
	*/
}

TEST_CASE("Clear lowest bit of 1 results in 0", "[single-file]") {
	int value = 1;
	BitClear(value, 0);
	REQUIRE(value == 0);
}

TEST_CASE("Clear lowest bit of 0 results in 0 (idempotent)", "[single_file]") {
	int value = 0;
	BitClear(value, 0);
	REQUIRE(value == 0);
}

TEST_CASE() {
  int result = BitTest(0, 0);
  REQUIRE(result == 0);

  result = BitTest(1, 0);
  REQUIRE(result == 1);
  result = BitTest(1, 1);
  REQUIRE(result == 0);

  result = BitTest(2, 0);
  REQUIRE(result == 0);
  result = BitTest(2, 1);
  REQUIRE(result == 1);

  result = BitTest(3, 0);
  REQUIRE(result == 1);
  result = BitTest(3, 1);
  REQUIRE(result == 1);
}

