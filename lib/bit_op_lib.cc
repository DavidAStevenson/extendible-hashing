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

/*
=================================================================================
Name      | GetHighestSetBit
Purpose   | Determine the highest order bit which is set
Params    | intToTest - the integer to test
Returns   | Returns the bit number of the highest bit which is set
          | Returns -1 if no bits are set
=================================================================================
*/
int GetHighestSetBit(int intToTest) {
  int tester = 1 << (NUMBITS-1);                        // For testing each bit
  for (int i = (NUMBITS-1); i >= 0; --i){               // Test bits from right to left
    if ( (tester & intToTest) == tester ){              // Perform the test
      return i;                                         // Success, return the bit number
    }
    tester = tester >> 1;                               // Shift the 1 to the next column
  }
  return -1;                                            // No bits are set
}

/*
=========================================================================================
Name      | IntInBinary
Purpose   | Returns an integer in binary form via char*
Params    | intToConvert - the integer to be converted to binary
          | numOfBits - the number of bits to convert - these bits start at the low end
          | outString - where the end result will be inserted
Returns   | true of false - Function succeeded or Function failed
Notes     | It is the caller's responsibility to ensure the outString has room for
          | enough characters including the null character. This should be numOfBits+1.
          | The function should always return true. False indicates a bug
=========================================================================================
*/
bool IntInBinary(int intToConvert, int numOfBits, char* outString) {
  int bitIndx;                                          // The bit number 31 - 0
  int stringIndx;                                       // The string index 0 - 31
  memset(outString, '\0', numOfBits);                   // Blank the string
  int maxIndx = numOfBits - 1;                          // Top bit index

  // For the bits in the integer right of MaxIndx (move left to right)
  for (bitIndx = maxIndx, stringIndx = 0; bitIndx >= 0; --bitIndx, ++stringIndx){
    int bitValue;                                       // Value the bit holds, 1 or 0
    bitValue = BitTest(intToConvert, bitIndx);
    switch (bitValue) {
    case 0:
      outString[stringIndx] = '0';
      break;
    case 1:
      outString[stringIndx] = '1';
      break;
    default: // If -1 was returned then BitTest failed
      return false;
    }                                                   // switch
  }
  outString[numOfBits] = '\0';                          // Null terminate
  return true;
}

/*
=================================================================================
Name      | ReverseBits
Purpose   | Reverse the first X bits of a given integer
Params    | intToReverse - the integer who's bits are to be reversed
          | numOfBits - the number of bits to reverse, defaults to 32
Returns   | Returns the reversed result
Notes     | - If the numOfBits specified is over the maximum range, it is set to the
          | maximum and the function continues
          | - If the numOfBits is less than 0 then it is set to 0 and the function
          | continues
          | - The reversal occurs from the lowest order bits for numOfBits number of bits
          | for example, given the default numOfBits and the integer:
          | 01000000 00000100 11001111 01100111 the return would be
          | 11100110 11110011 00100000 00000010
          | If numOfBits is less than 32, say 16, then the return would be
          | 00000000 00000000 11100110 11110011
=================================================================================
*/
/*
  THIS FUNCTION NEEDS MORE TESTING DONE ON THE VARIABLE numOfBits CASE
 */

int ReverseBits(int intToReverse, int numOfBits) {
  // Ensure numOfBits is valid, that is, between 0 and 32
  if (numOfBits > NUMBITS) {
    numOfBits = NUMBITS;
  } else if (numOfBits < 0) {
    numOfBits = 0;
  }

  int reversedInt = 0;
  for (int i = 0; i < numOfBits; i++){
    reversedInt = reversedInt << 1;                     // Make room for the next bit
    int lowestBit = 0;                                  // For getting the lowest bit
    lowestBit = intToReverse & 1;                       // Get the lowest bit
    reversedInt = reversedInt | lowestBit;              // Insert lowest bit
    intToReverse = intToReverse >> 1;                   // Throw away the reversed bit
  }
  return reversedInt;
}


//} // namespace extendible_hashing_bit_op_lib_internal

