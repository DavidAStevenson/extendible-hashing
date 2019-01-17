/*
=========================================================================================
Name     | IndexHolder                                                                  |
Purpose  | Hold an extendible index                                                     |
Date     | June 2000                                                                    |
Author   | David Stevenson                                                              |
=========================================================================================
*/
#include <iostream>

// For file system methods and constants 
#include <unistd.h>
#include <fcntl.h> 

#include "indexholder.h"
#include "bit_op_lib.h"

/*
=========================================================================================
Name     | Constructors
=========================================================================================
*/
IndexHolder::IndexHolder()
{
  _indexDepth = 1;
  _indexPointer = 0;
}

IndexHolder::IndexHolder(int initialDepth)
{
  // Set up an index of initialDepth
  if ( !( (initialDepth >= 1) && (initialDepth <= 32) ) ){
    initialDepth = 1;
  }
  _indexDepth = initialDepth;
  _indexPointer = CreateIndex( GetNumberOfAddresses() ); 
}
  
/*
=========================================================================================
Name     | Destructors
=========================================================================================
*/
IndexHolder::
~IndexHolder()
{
  delete _indexPointer;
  
}

bool
IndexHolder::
Load(int fileDescriptor
     )
{
  if (fileDescriptor < 0){
    return false;
  }

  // Just in case the user stuffs up
  delete _indexPointer;
  
  // Seek to the start of the index
  lseek(fileDescriptor, 0, SEEK_SET);

  // Read the index depth
  int dataRead = read(fileDescriptor, 
		      &_indexDepth, 
		      sizeof(_indexDepth)
		      );
  if ( dataRead != sizeof(_indexDepth) ){
    return false;
  }

  // Create space for the index
  _indexPointer = CreateIndex( GetNumberOfAddresses() );

  // Read the index  
  dataRead = read(fileDescriptor,                        // file to read from 
		  &*_indexPointer,                       // buffer to read to
		  (sizeof(int) * GetNumberOfAddresses()) // size of data to read
		  );
  if ( static_cast<size_t>(dataRead) != (sizeof(int) * GetNumberOfAddresses()) ){
    return false;
  }
  return true;
}

bool 
IndexHolder::
Write(int fileDescriptor
      )
{
  if (fileDescriptor < 0){
    return false;
  }
  if (_indexPointer == 0){
    return false;
  }
  
  // Seek to the start of the index
  lseek(fileDescriptor, 0, SEEK_SET);

  // Write the index depth
  int dataWrote = write(fileDescriptor, 
			&_indexDepth, 
			sizeof(_indexDepth));
  if ( dataWrote != sizeof(_indexDepth) ){
    return false;
  }

  // Write the index
  dataWrote = write(fileDescriptor, 
		    _indexPointer, 
		    (sizeof(int) * GetNumberOfAddresses()) 
		    );
  if ( static_cast<size_t>(dataWrote) != (sizeof(int) * GetNumberOfAddresses()) ){
    return false;
  }
  return true;

}

/*
=========================================================================================
Name     | IncreaseDepth
Purpose  | Increase the bit depth of the index by 1, and copy all the old pointer values
         | into the new index.
Notes    | Assuming the original index has a pointer at the index xxx. In the new index
         | both indexes 0xxx and 1xxx get assigned with the same pointer as xxx.
=========================================================================================
*/
void 
IndexHolder::
IncreaseDepth()
{
  if (_indexPointer == 0){
    return;
  }

  // Calculate new depth
  int newDepth = _indexDepth + 1;
  // New number of addresses is 2 times the old number
  int oldNumOfAddresses = GetNumberOfAddresses();
  int newNumOfAddresses = 2 * oldNumOfAddresses;

  // Allocate memory for new index
  int* tempIndex = CreateIndex(newNumOfAddresses);
  int extraBit = 1 << _indexDepth;
  // Copy old pointer values into the new index
  for (int i = 0; i < oldNumOfAddresses; i++){
    // Assign the 0xxx pointer
    tempIndex[i] = _indexPointer[i];
    // Assign the 1xxx pointer                          
    tempIndex[extraBit | i] = _indexPointer[i];            
  }

  // Update depth
  _indexDepth = newDepth;
  // The new index is assigned
  delete _indexPointer;
  _indexPointer = tempIndex;
}

/*
=========================================================================================
Name     | DecreaseDepth
Purpose  | Decrease the bit depth of the index by 1, and copy all the old pointer values
         | into the new index.
Notes    | If a decrease is possible then it must hold that the values in the first half
         | of the index match those values in the second half of the index (in the exact
         | same order). Thus when the index size is decreased we have to simply copy one
         | half of the old index into the new one.
=========================================================================================
*/
bool 
IndexHolder::
DecreaseDepth()
{
  if ( DepthDecreasePossible() ){
    // Calculate new depth
    int newDepth = _indexDepth - 1 ;
    // New number of addresses is half of the old number
    int oldNumOfAddresses = GetNumberOfAddresses();
    int newNumOfAddresses = oldNumOfAddresses / 2;
    // And allocate memory for new index
    int* tempIndex = CreateIndex(newNumOfAddresses);
    
    // Copy the old pointer values back into the smaller index
    for (int i = 0; i < oldNumOfAddresses; i++){
      // Assign the xxx pointer
      //std::cout << "depth decrease " << i << std::endl << std::flush;
      tempIndex[i] = _indexPointer[i];
    }
    // Update depth
    _indexDepth = newDepth;
    // The new index is assigned
    delete _indexPointer;
    _indexPointer = tempIndex;
    
    return true;
  } else {
    // Depth decrease not possible
    return false;
  }
}

/*
=========================================================================================
Name     | SetAddress
Purpose  | Set an address of an index value
=========================================================================================
*/
void 
IndexHolder::
SetAddress(int index,                                   // Index whose address to set 
	   int address                                  // The address value to be given
	   )
{
  if (_indexPointer == 0){
    return;
  }

  if ( (index >= 0) && (index < GetNumberOfAddresses()) ){
    _indexPointer[index] = address;
  } 
}

/*
=========================================================================================
Name     | GetAddress
Purpose  | Return the address for a given index
Returns  | The address for the given index, or -1 if the index value is out of range
=========================================================================================
*/
int                                                     // The address at the index
IndexHolder:: 
GetAddress(int index                                    // Index whose address to return
	   )
{
  if ( (index >= 0) && (index < GetNumberOfAddresses()) ){
    return _indexPointer[index];
  } else {
    return -1;
  }
}

void
IndexHolder::
Print()
{
  if (_indexPointer == 0){
    return;
  }
  char posString[_indexDepth + 1];                      // Get integers in binary string
  for (int i = 0; i < GetNumberOfAddresses(); i++){
    if (IntInBinary(i, _indexDepth, posString)){
      std::cout << posString << "->" << _indexPointer[i] << std::endl << std::flush;
    } else {
      std::cout << "IntInBinary error..." << std::endl << std::flush;
    }
  }
}

int 
IndexHolder::
GetNumberOfAddresses()
{
  int result = 1;
  for (int i = 0; i < _indexDepth; i++){
    result = result * 2;
  }
  return result;
}

int
IndexHolder::
GetDepth()
{
  return _indexDepth;
}

/*
  Private member functions
*/

/*
=========================================================================================
Name    | CreateIndex
Purpose | Creates an index with the specified number of addresses
Returns | A pointer to the new index
=========================================================================================
*/
int*
IndexHolder::
CreateIndex(int numOfAddresses
	    )
{
  // Allocate memory for new addresses
  int* tempIndex = new int[numOfAddresses];
  
  // Point each element to the zeroth address
  for (int i = 0; i < numOfAddresses; i++){
    tempIndex[i] = 0;
  }  
  return tempIndex;
}

/*
=========================================================================================
Name     | DepthDecreasePossible
Purpose  | Determine whether the index can be decreased or not.
Returns  | True if the index can be decreased, False otherwise
Notes    | The index size can be decreased iff each pair of buddies has the same value
         | Assume the index contains 4 indexes, 00, 01, 10, 11. The "buddy" values are
         | 00 & 10, 01 & 11
         | This is because other than their leftmost bit, the bit patterns are the
         | same. (ie, 00 & 10 both end in 0, and 01 & 11 both end in 1)
=========================================================================================
*/
bool
IndexHolder::
DepthDecreasePossible()
{
  if ( !(_indexDepth > 1) ){
    return false;
  }
  int halfway = GetNumberOfAddresses() / 2;
  int topBit = 1 << (_indexDepth - 1);
  // Check that all the buddies are pointing at the same thing
  for (int i = 0; i < halfway; i++){
    if (_indexPointer[topBit | i] != _indexPointer[i]){
      return false;
    } 
  }
  return true;
}
