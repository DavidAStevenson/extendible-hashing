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


//} // namespace extendible_hashing_bit_op_lib_internal

#endif // EXTENDIBLE_HASHING_BIT_OP_LIB_H_
