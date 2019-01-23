#ifndef _EHfBucKeT__
#define _EHfBucKeT__

#include "records.h"

class EHFBucket{
 public:
  EHFBucket(int fd, int address);
  EHFBucket(int fd, int address, int bitDepth);
  ~EHFBucket();
  int Read();
  int Write();
  int RecordPosition(char* key);
  int Add(char* keyToAdd, char* recordToAdd);
  int Retrieve(char* keyToFind, char* returnRecord);
  int Delete(char* keyToDelete);
  int NumOfRecs();
  int Depth();
  void ChangeAddress(int newAddress);
  void RetrieveRecAtIndex(int index, char* returnKey, char* returnRecord);

 private:
  // Private member functions
  int BucketPosition();
  
  // Private data members
  int _bucketAddress;                                      // Main bucket address
  int _fileDescriptor;                                     // File descriptor
  int _fileHeaderSize;                                     // Size of main file header

  struct {
    int numOfRecs;                                         // Number of records in bucket
    int depth;                                             // Depth of the bucket in bits
    char records[RECSBUFFSIZE];                            // The records
  } _bucketBuffer;
};

#endif
