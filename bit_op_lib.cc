/*			  					       		       
=================================================================================
Name      | BitOpLibrary                                                        |
Purpose   | Provide functionality for bit operations                            |
Date      | April 2000, Jan 2019                                                |
Author	  | D A Stevenson                                                       |
Notes 	  | Operates on 32 bit integers                                         |
          | Lowest bits are stored in the right                                 |
----------|-------------------|-------------------------------------------------|
Methods   | Name              | Functionality                                   |
----------|-------------------|-------------------------------------------------|
          | BitClear          | Clears a bit in a given integer                 |
          | BitSet            | Sets a bit in a given integer                   |
          | BitTest           | Test a bit in a given integer                   |
          | GetHighestBits    | Return the highest X bits of a given integer    |
          | GetHighestSetBit  | Determine the highest order bit which is set    |
          | GetLowestBits     | Return the lowest X bits of a given integer     |
          | IntInBinary       | Returns an integer in binary form via char*     |
          | ReverseBits       | Reverse the first X bits of a given integer     |
----------|-------------------|-------------------------------------------------|
Testing   | Testing program is found in testbitlib.cc                           |
=================================================================================
*/

#include <iostream>
#include <cstring>
#include <cassert>

#include "bit_op_lib.h"

//namespace extendible_hashing_bit_op_lib_internal {

/*
================================================================================
Name      | BitClear
Purpose   | Clear a bit in a given integer
Params    | & intToChange - the integer to be modified
          | bitToClear - the bit number to be cleared (zero-indexed)
================================================================================
*/
void BitClear (int& intToChange, int bitToClear) {
  assert((bitToClear >= 0) && (bitToClear < NUMBITS));

  int bitClearer = 0;
  BitSet(bitClearer, bitToClear);

  intToChange &= ~bitClearer;
}

/*
================================================================================
Name      | BitSet
Purpose   | Set a bit in a given integer
Params    | & intToChange - the integer to be modified
          | bitToSet - the bit number to be set
================================================================================
*/
void BitSet (int& intToChange, int bitToSet) {
  assert((bitToSet >= 0) && (bitToSet < NUMBITS));

  intToChange |= (1 << bitToSet);
}

/*
=================================================================================
Name      | BitTest
Purpose   | Test a bit in a given integer
Params    | intToTest - the integer to work on
          | bitToTest - the bit number to test
Returns   | 1 if the specified bit is set
          | 0 if the specified bit is not set
          | -1 if the function could not be performed (bitToTest out of range)
=================================================================================
*/
int BitTest(int intToTest, int bitToTest) {
  assert((bitToTest >= 0) && (bitToTest < NUMBITS));

  int tester = 0;
  BitSet(tester, bitToTest);
  if ((intToTest & tester) == tester) {
    return 1;
  } else {
    return 0;
  }
}


//} // namespace extendible_hashing_bit_op_lib_internal

