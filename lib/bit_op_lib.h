#ifndef EXTENDIBLE_HASHING_BIT_OP_LIB_H_
#define EXTENDIBLE_HASHING_BIT_OP_LIB_H_

//namespace extendible_hashing_bit_op_lib_internal {

const int NUMBITS = 32;                                 // 32 bits per UNIX int

// set the designated bit (zero-indexed) to 0
void BitClear (int& intToChange, int bitToClear);

// set the designated bit (zero-indexed) to 1
void BitSet (int& intToChange, int bitToSet);

// return the value of the specified bit
int BitTest (int intToTest, int bitToTest);

// return the highest bits of the given integer (how many bits as given by numOfBits)
int GetHighestBits(int theInt, int numOfBits);

// return the highset set bit of the given integer
int GetHighestSetBit(int intToTest);

// return the lowest bits of the given integer (how many bits as given by numOfBits)
int GetLowestBits(int theInt, int numOfBits);

// what, no "GetLowestSetBit"?

// probably shouldn't be bool return
bool
IntInBinary(int intToConvert,                           // Integer to convert to string
            int numOfBits,                              // Number of bits to convert
            char* outString                             // Return binary interpretation
            );

// return an integer containing the given integer's bits, in reverse
int ReverseBits(int intToReverse, int numOfBits = NUMBITS);


//} // namespace extendible_hashing_bit_op_lib_internal

#endif // EXTENDIBLE_HASHING_BIT_OP_LIB_H_
