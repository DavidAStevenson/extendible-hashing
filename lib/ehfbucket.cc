/*
=========================================================================================
Name    | EHFBucket (Extendible Hashing File Bucket) implementation
Purpose | Bucket behaviour for an extendibile hash file
=========================================================================================
*/

#include "ehfbucket.h"
#include "ehfconsts.h"

#include <iostream>
#include <cstring>

#include <unistd.h>
#include <fcntl.h> 
#include <errno.h>

/*
=========================================================================================
Name    | EHFBucket constructor
Purpose | Construct an EHFBucket
Notes   | The first constructor should be called for buckets that will be read from the 
        | file. The second constructor should be called for brand new buckets whose
        | depth needs to be assigned
=========================================================================================
*/
// Constructor for existing bucket contained in the file.
EHFBucket::
EHFBucket(int fd,                                          // fd of open bucket file
	  int address                                      // Bucket number in the file
	  )
{
  _fileDescriptor = fd;
  _bucketAddress = address;
  // Initialise bucket buffer
  _bucketBuffer.numOfRecs = 0;  
  _bucketBuffer.depth = 1;                                 // dummy only
  std::memset(_bucketBuffer.records, '\0', RECSBUFFSIZE);
}

// Constructor for a bucket which has not yet been written to the file
EHFBucket::
EHFBucket(int fd,                                          // fd of open bucket file
	  int address,                                     // Bucket number in the file
	  int bitDepth                                     // Depth of address in bits
	  )
{
  _fileDescriptor = fd;
  _bucketAddress = address;
  // Initialise bucket buffer
  _bucketBuffer.numOfRecs = 0;  
  _bucketBuffer.depth = bitDepth;                          
  std::memset(_bucketBuffer.records, '\0', RECSBUFFSIZE);
}

/*
=========================================================================================
Name    | EHFBucket destructor
Purpose | Destruct an EHFBucket
=========================================================================================
*/
EHFBucket::
~EHFBucket()
{
  // It is the users responsibility to call write where neccessary.
  // Any information that was not written using Write() will be lost
  // when the LHFAddress goes out of scope
}

/*
=========================================================================================
Name    | Read
Purpose | Read in the EHFBucket from the file
Returns | EHF_READOK - if the bucket is read from the file as expected
        | EHF_FILENOTOPEN - if the file descriptor was invalid
        | EHF_READERROR - if there is an error reading the bucket from the file
=========================================================================================
*/
int 
EHFBucket::
Read()
{
  if (_fileDescriptor < 0){
    return EHF_FILENOTOPEN;
  }
  // Set the file position to the position of the bucket
  lseek(_fileDescriptor, BucketPosition(), SEEK_SET);
  // Attempt to read the bucket
  int dataRead = read(_fileDescriptor, &_bucketBuffer, sizeof(_bucketBuffer));
  if (dataRead == sizeof(_bucketBuffer)){
    return EHF_READOK;
  } else {
    return EHF_READERROR;
  }
}

/*
=========================================================================================
Name    | Write
Purpose | Write the EHFBucket to the file
Returns | EHF_WRITEOK - if the bucket is written to the file as expected
        | EHF_FILENOTOPEN - if the file descriptor was invalid
        | EHF_WRITEERROR - if there is an error writing the bucket to the file
=========================================================================================
*/
int
EHFBucket::
Write()
{
  if (_fileDescriptor < 0){
    return EHF_FILENOTOPEN;
  }
  // Set the file position to the position of the bucket
  lseek(_fileDescriptor, BucketPosition(), SEEK_SET);
 
  // Attempt to write the bucket
  int wrote = write(_fileDescriptor, &_bucketBuffer, sizeof(_bucketBuffer));
  if (wrote == sizeof(_bucketBuffer)){
    return EHF_WROTEOK;
  } else {
    return EHF_WRITEERROR;
  }
}

/*
=========================================================================================
Name    | BucketPosition
Purpose | Return the start position of this bucket in relation to the start of the file
Returns | The position in number of bytes from the start of the file, plus the size of
        | the file header
=========================================================================================
*/
int 
EHFBucket::
BucketPosition(){
  return ( (BUCKETSIZE * _bucketAddress) + FILEHEADERSIZE );
}

/*
=========================================================================================
Name    | NumOfRecs
Purpose | Return the number of records stored in this bucket
Returns | The number
=========================================================================================
*/
int
EHFBucket::
NumOfRecs(){
  return _bucketBuffer.numOfRecs;
}

/*
=========================================================================================
Name    | Depth
Purpose | Return the depth of the bucket
Returns | The number
=========================================================================================
*/
int
EHFBucket::
Depth(){
  return _bucketBuffer.depth;
}

/*
=========================================================================================
Name    | RecordPosition
Purpose | Return the record index of the record matching the key
Params  | The char array key to match records against
Returns | The index of the record if found
        | -1 if no matching record is present
Notes   | The key parameter can be null terminated or not
=========================================================================================
*/
int                                                     // Return the record index
EHFBucket::
RecordPosition(char* key
	       )
{
  char compKey[IDSIZE+1];
  strncpy(compKey, key, IDSIZE);
  compKey[IDSIZE] = '\0'; 
  int result;
  for (int index = 0; index < FULLBUCKET; ++index){
    int recordIndex = (index*RECORDSIZE)+IDPOSITION;
    result = strncmp(&_bucketBuffer.records[recordIndex], compKey, IDSIZE);
    if (result == 0){                                    
      // If string's were equal then return the index of the current record 
      return index;
    }
  }
  // Record not found in bucket
  return -1;
}
  
/*
=========================================================================================
Name    | Add
Purpose | Add a record to the bucket
Params  | keyToAdd - The char array key for the record
        | recordToAdd - The char array containing the record
Returns | EHF_INSERTED - If the record is inserted correctly into the bucket
        | EHF_ALREADY_PRESENT - If the key is already present in the bucket
        | EHF_FULLBUCKET - If the bucket is already full
Notes   | Paramaters can be either null-terminated or not null-terminated. 
=========================================================================================
*/
int 
EHFBucket::
Add(char* keyToAdd,
    char* recordToAdd
    )
{
  if (_bucketBuffer.numOfRecs == FULLBUCKET){
    return EHF_FULLBUCKET;  
  }
  if ( RecordPosition(keyToAdd) != -1 ){
    // Key is already in the file
    return EHF_ALREADY_PRESENT;
  }
 
  int recordPosition = NumOfRecs() * RECORDSIZE;
  // I believe strcpy may be inappropriate because it will append a null terminator
  //strcpy(&_bucketBuffer.records[recordPosition], recordToAdd);
  // memmove is not supposed to copy the terminators as well, so it is used here
  memmove(&_bucketBuffer.records[recordPosition], recordToAdd, RECORDSIZE);
  _bucketBuffer.numOfRecs++;
  return EHF_INSERTED;
}

/*
=========================================================================================
Name    | Retrieve
Purpose | Retrieve a record from the bucket
Params  | keyToFind - The char array key for the record to be found
        | returnRecord - Return the record if it is found
Returns | EHF_RETRIEVED - If a matching record is found and is returned
        | EHF_NOT_PRESENT - If no matching record can be found in the bucket
Notes   | The returnRecord param must be big enough to hold a record, as well as a null
        | terminator character. The string returned will be null terminated.
=========================================================================================
*/
int 
EHFBucket::
Retrieve(char* keyToFind, 
	 char* returnRecord
	 )
{
  int recordNumber = RecordPosition(keyToFind);
  if (recordNumber == -1){
    // Key is not in the bucket
    return EHF_NOT_PRESENT;
  } else {
    int recordPos = recordNumber * RECORDSIZE;
    //strncpy(returnRecord, &_bucketBuffer.records[recordPos], RECORDSIZE);
    memmove(returnRecord, &_bucketBuffer.records[recordPos], RECORDSIZE);
    returnRecord[RECORDSIZE] = '\0';
    return EHF_RETRIEVED;
  }
}

/*
=========================================================================================
Name    | RetrieveRecAtIndex
Purpose | Retrieve a record and its key from the bucket at a specific index
Notes   | Return values are null terminated
=========================================================================================
*/
void
EHFBucket::
RetrieveRecAtIndex(int index,                              // The index of the record
		   char* returnKey,                        // Return the key 
		   char* returnRecord                      // Return the record
		   )
{
  int recordPos = index * RECORDSIZE;
  // Get the ID out of the record 
  strncpy(returnKey, &_bucketBuffer.records[recordPos + IDPOSITION], IDSIZE);
  returnKey[IDSIZE] = '\0';
  // Get the record
  strncpy(returnRecord, &_bucketBuffer.records[recordPos], RECORDSIZE);
  returnRecord[RECORDSIZE] = '\0';  
}

/*
=========================================================================================
Name    | Delete
Purpose | Delete a record matching keyToDelete from the bucket
Returns | EHF_DELETED - record matching ketToDelete was successfully deleted
        | EHF_NOT_PRESENT - no records matching the keyToDelete were found
=========================================================================================
*/
int
EHFBucket::
Delete(char* keyToDelete
       )
{
  int recordNumber = RecordPosition(keyToDelete);
  if (recordNumber == -1){
    // Key is not in the bucket
    return EHF_NOT_PRESENT;
  } else {
    int recPos = recordNumber * RECORDSIZE;                // Pos of record to delete
    //int nextRecPos = recPos + RECORDSIZE;                  // Pos of next record
    int amountToMove = (NumOfRecs() * RECORDSIZE) - recPos;// Size of following records 
    // Move records after record to delete forward
    memmove(&_bucketBuffer.records[recPos],                // Destination
	    &_bucketBuffer.records[recPos+RECORDSIZE],     // Source
	    amountToMove);                                 // Amount of data to move
    std::memset(&_bucketBuffer.records[(NumOfRecs()-1)*RECORDSIZE], '\0', RECORDSIZE);
    _bucketBuffer.numOfRecs--;
    return EHF_DELETED;
  }
}

/*
=========================================================================================
Name    | ChangeAddress
Purpose | Change the address of this bucket
Params  | newAddress - the new address of the bucket (relative bucket number)
=========================================================================================
*/
void
EHFBucket::
ChangeAddress(int newAddress
	      )
{
  _bucketAddress = newAddress;
}
