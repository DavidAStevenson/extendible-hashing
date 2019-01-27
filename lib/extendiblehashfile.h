/*
=========================================================================================
Name    | Extendible Hash File                                                          |
Purpose | Extendible Hash File header                                                   |
Date    | June 2000                                                                     |
Author  | David Stevenson                                                               |
----------------------------------------------------------------------------------------|
Methods | Name                | Functionality                                           |
----------------------------------------------------------------------------------------|
        | ExtendibleHashFile  | Constructor                                             |
        | ~ExtendibleHashFile | Destructor                                              |
        | Open                | Open file with a given name, either existing or new     |
        | Close               | Close the file that was opened with the Open call       |
        | InsertRecord        | Insert a record into the file opened by the Open call   |
        | RetrieveRecord      | Retrieve record from the file matching the given key    |
        | DeleteRecord        | Delete record from the file matching the given key      |
----------------------------------------------------------------------------------------|
Notes   | This is an extendible hash file, that is, it grows and shrinks as records are |
        | inserted and deleted. The retrieve function is purely that, the file is not   |
        | affected by any retrieve operations                                           |
=========================================================================================
*/
#ifndef _ExTENdiBLEhAsHFilE__
#define _ExTENdiBLEhAsHFilE__ 

#include "indexholder.h"

class ExtendibleHashFile{
  /*
  =======================================================================================
   INTERFACE METHODS
  =======================================================================================
   */
 public:
  // Constructor
  ExtendibleHashFile();

  // Destructor
  ~ExtendibleHashFile();

  // Open an extendible hash file
  bool                                               // True if open is successful
  Open(char* fileName,                               // Filename of file to open 
       bool openExisting = true                      // If the file exists 
       );

  // Test if the file is open
  bool                                               // True if the file is open
  IsOpen();                                          

  // Close the extendible hash file opened by the Open call
  void 
  Close();

  // Get a summary of what is contained in the file
  void
  FileSummary();

  // Insert a record into the extendible hash file
  int                                                // Return code, see ehfconsts.h
  InsertRecord(char* keyToAdd,                       // Key of the record to add
	       char* recordToAdd                     // The record to add
	       );

  // Retrieve a record from the extendible hash file
  int                                                // Return code, see ehfconsts.h
  RetrieveRecord(char* keyToFind,                    // Key of the record to search for 
		 char* returnRecord                  // Return the record if found
		 );
  
  // Delete a record from the extendible hash file
  int                                                // Return code, see ehfconsts.h 
  DeleteRecord(char* keyToDelete                     // Key of the record to delete
	       );

  /*
  =======================================================================================
   IMPLEMENTATION METHODS
  =======================================================================================
   */
 private:
  int 
  InsertRecord(char* keyToAdd,                       // Key for the record to be added
	       char* recordToAdd,                    // Record to be added
	       int hashValue,                        // 
	       int callNumber
	       );

  int 
  AccomodateRecord(int address, 
		   int bucketDepth
		   );

  int 
  SplitBucket(int addressToSplit, 
	      int bucketDepth
	      );

  bool 
  OpenExistingFile(char* indexFileName, 
		   char* bucketFileName
		   );

  bool 
  CreateNewFile(char* indexFileName, 
		char* bucketFileName
		);

  int
  ReadBucketCount();
 
  int
  WriteBucketCount();
  
  // Members
  bool _fileOpen;                                       // True if the file is open
  int _indexFileFD;                                     // File descriptor of directory
  int _bucketFileFD;                                    // File descriptor of bucket file
  int _bucketCount;                                     // Number of buckets in the file
  IndexHolder* _index;                                  // Pointer to the index
};

#endif
