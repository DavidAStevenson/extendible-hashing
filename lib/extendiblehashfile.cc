/*
=========================================================================================
Name	 | ExtendibleHashFile
Purpose	 | Extendible Hashing File implementation
=========================================================================================
*/

#include <string.h>

// Get extendible hash file header and constants
#include "ehfconsts.h"
#include "extendiblehashfile.h"

// Get extendible hash file bucket class
#include "ehfbucket.h"

// Get the hash function
#include "hash.h"

// Get the bit library operations
#include "bit_op_lib.h"

// For file system methods and constants
#include <unistd.h>
#include <fcntl.h>

// For FileSummary out put
#include <iostream>

// For power function calls
#include <math.h>

/*
=========================================================================================
Name	 | ExtendibleHashFile destructor
Purpose	 | Destructor
=========================================================================================
*/
ExtendibleHashFile::
ExtendibleHashFile()
{
  // File is not open initially
  _fileOpen = false;
  _bucketCount = 0;
  _bucketFileFD = -1;
  _indexFileFD = -1;
}

/*
=========================================================================================
Name	 | ExtendibleHashFile destructor
Purpose	 | Destructor
=========================================================================================
*/
ExtendibleHashFile::
~ExtendibleHashFile()
{
  if (_fileOpen){
    Close();
  }
}

/*
=========================================================================================
Name	 | Open
Purpose	 | Open a extendible hash file
Params	 | fileName - the name of the file to open
	 | openExisting - false to create a new file,
	 |		- true to open an existing file - this is the default
Returns	 | True if the file was opened successfully
=========================================================================================
*/
bool							// Successful or not
ExtendibleHashFile::
Open(char* fileName,					// File to open
     bool openExisting					// Open existing file
     )
{
  // Close file if one is already open
  if (_fileOpen){
    Close();
  }
  // Create a file name
  int strLength = 5 + strlen(fileName);
  char indexFileName[strLength];			// Index file name
  char bucketFileName[strLength];			// Bucket file name
  memset(bucketFileName, '\0', strLength);
  memset(indexFileName, '\0', strLength);
  strcpy(bucketFileName, fileName);
  strcpy(indexFileName, fileName);
  strcat(bucketFileName, ".ehf");			// Append the extension
  strcat(indexFileName, ".ehd");			// Append the extension

  if (openExisting){
    _fileOpen = OpenExistingFile(indexFileName, bucketFileName);
  } else {
    _fileOpen = CreateNewFile(indexFileName, bucketFileName);
  }
  return _fileOpen;
}

bool
ExtendibleHashFile::
IsOpen(){
  return _fileOpen;
}

/*
=========================================================================================
Name	 | Close
Purpose	 | Close an extendible hashing file
=========================================================================================
*/
void
ExtendibleHashFile::
Close()
{
  if (!_fileOpen){
    return;
  }
  // Write the index
  _index->Write(_indexFileFD);
  // Deallocate index memory
  delete _index;
  // Write bucket Count
  if (WriteBucketCount() != EHF_WROTEOK){
    // std::cout error
  }

  // Close the files
  close(_indexFileFD);
  close(_bucketFileFD);
  _indexFileFD = -1;
  _bucketFileFD = -1;
  _bucketCount = 0;
  _fileOpen = false;					// File is now closed
}

/*
=========================================================================================
Name	 | InsertRecord
Purpose	 | Insert a record into an extendible hashing file
Returns	 | EHF_INSERTED - Record was inserted successfully
	 | EHF_ALREADY_PRESENT - The record was already present
	 | EHF_FILENOTOPEN - The bucket file was not open
	 | EHF_WRITEERROR - The bucket was not rewritten correctly
	 | EHF_READERROR - The bucket was not read correctly
	 | EHF_POORHASHFUNCTION - The hash function is not spreading the key's evenly
Notes	 | If you get the poor hash function return then you should GET A BETTER HASH
	 | FUNCTION
=========================================================================================
*/
// the public method
int
ExtendibleHashFile::
InsertRecord(char* keyToAdd,				// Key value
	     char* recordToAdd				// Record to insert
	     )
{
  if (!_fileOpen){
    return EHF_FILENOTOPEN;
  }
  char key[IDSIZE+1];
  *((char *) mempcpy(key, keyToAdd, IDSIZE)) = '\0';
  //strlcpy(key, keyToAdd, IDSIZE+1);			// 1 for the null character
  // Get 32 bit hash value
  int hashValue = Hash(key);
  // Attempt to insert the record
  return InsertRecord(keyToAdd, recordToAdd, hashValue, 1);
}

// The private method (recursive)
int
ExtendibleHashFile::
InsertRecord(char* keyToAdd,				// Key value
	     char* recordToAdd,				// Record to insert
	     int   hashValue,				// keyToAdd's hash value
	     int   callNumber				// For infinite recursion check
	     )
{
  if (callNumber > 5){
    // The method has call itself multiple times, trying to insert the record
    // The hash function is not distributing the keys evenly enough.
    return EHF_POORHASHFUNCTION;
  }

  // Determine the address from the 32 bit hash value
  int address = GetLowestBits( hashValue, _index->GetDepth() );

  EHFBucket* bucket = new EHFBucket(_bucketFileFD, _index->GetAddress(address));

  int readResult = bucket->Read();
  if (readResult != EHF_READOK){
    delete bucket;
    return readResult;
  }

  int addResult = bucket->Add(keyToAdd, recordToAdd);	 // Attempt to add the record
  int bucketDepth;
  switch (addResult){
  case EHF_INSERTED:
      // Base case 1
      // The record was inserted
      bucket->Write();					 // Write bucket back to file
      delete bucket;					 // Deallocate memory for bucket
      return addResult;					 // Return EHF_INSERTED
      break;
  case EHF_ALREADY_PRESENT:
      // Base case 2
      // Bucket already contained key, thus nothing was added - no need to rewrite it
      delete bucket;					 // Deallocate memory for bucket
      return addResult;					 // Return EHF_ALREADY_PRESENT
      break;
  case EHF_FULLBUCKET:
      // Recursive case
      // Bucket was full so it must be split
      bucketDepth = bucket->Depth();			 // For AccomodateRecord call
      delete bucket;					 // Deallocate memory for bucket
      AccomodateRecord(address, bucketDepth);		 // Make room for the record
      // Recursive call - attempt to insert the record again
      return InsertRecord( keyToAdd, recordToAdd, hashValue, (callNumber+1) );
      break;
  default:
      // This should never happen
      return -1;
      break;
  } // switch
}

/*
=========================================================================================
Name	 | AccomodateRecord
Purpose	 | A helper for the InsertRecord methods. This method will split the address that
	 | is full by increasing the depth of the address by one. If the is no available
	 | space in the index for this increase of address depth, then the index space is
	 | doubled by adding an extra bit. The index is updated so that the addresses
	 | with the extra bit points at the new bucket which resulted from the split.
Notes    | Updating the index after the split can be a complicated process. Consider an
         | index of depth 10, and an address in which a record must be accomodated, and a
         | bucketDepth of 6. Say we have the address as 200. The bucket will contain
         | records which hashed to 001000, because the lower 6 bits of 200 in binary ->
         | 00000000 00000000 00000000 11001000, are 001000. In the index however, this
         | is being represented as all ten bit numbers ending in a 001000 pattern. Ie,
	 | 8   0000|001000
         | 72  0001|001000
         | 136 0010|001000
         | 200 0011|001000
         | ...
         | ...
         | 904 1110|001000
	 | 968 1111|001000
         |        (^ only the bits after here are required)
	 | All of these numbers are pointing at the same bucket. However, as the bucket
         | is full, it will be split in two, and an extra bit will be used to distinguish
         | between the records in the bucket. Thus the result will see
         | 8   000|0001000
         | 136 001|0001000
         | ...
         | 904 111|0001000
         | the above numbers pointing at the old bucket, (note that an extra bit is now
	 | needed to distinguish between records) and 
         | 72  000|1001000
         | 200 001|1001000
         | 328 010|1001000
         | ...
         | 840 110|1001000
         | 968 111|1001000
         | the above numbers all point at the new bucket. (again note that an extra bit
         | is being used here too)
         | Note that the difference in between these numbers is 128, or 2^7, or should
         | we say 2^(bucketDepth+1) (the old bucketDepth, not the new one).
         | The bit pattern above the newBucketDepth number of bits is 0, 1, 2, 3, up to
         | 2^unUsedBits.
	 | The way the for loop works:
         | Use i as a counter for the upper bits. Say i = 5 = 00000000 00000101
         | Move it along to be    00000010 10000000
	 | Or in another 1        00000010 11000000
	 | Or in the bucketValue  00000010 11001000
         | etc
=========================================================================================
*/
int                                                        // Return Code
ExtendibleHashFile::
AccomodateRecord(int address,				   // Address to expand
		 int bucketDepth			   // Current depth of address
		 )
{
  if ( bucketDepth >= 16){ // TODO
  //if ( bucketDepth >= MAXDEPTH ){
    return EHF_MAXTABLEDEPTH;
  }

  int bucketValue = GetLowestBits(address, bucketDepth);   // Bit pattern held by bucket 
  int newBucketNumber = SplitBucket(address, bucketDepth);

  //  int newAddress = GetLowestBits(address, bucketDepth);
  // int newBucketNumber = SplitBucket(address, bucketDepth);

  if ( bucketDepth == _index->GetDepth() ){
    // The case where the is only one address pointing at the bucket to split
    // Double the index - because we can't split one pointer into two
    _index->IncreaseDepth();
  }
  
  // Determine the number of bits of the index address that are not required in 
  // distinguishing between the new bucket and the old bucket
  int unUsedBits = ( _index->GetDepth() - (bucketDepth + 1) );
  
  int loopLimit = int(pow(2, unUsedBits));                 // Get 2 ** unUsedBits

  for (int i = 0; i < loopLimit; ++i){
    int tempAddress = i << (bucketDepth+1);                // Get the upper bit pattern
    tempAddress |= (1 << bucketDepth);                     // Get the extra one
    tempAddress |= bucketValue;                            // Or in the bucket Value
    _index->SetAddress(tempAddress, newBucketNumber);
  }
  //return somegoodcode;
  return 1; // TODO
}

/*
=========================================================================================
Name	 | SplitBucket
Purpose	 | Split a bucket in two
Returns	 | The relative bucket number of the "new" bucket that resulted from the split
	 | This is NOT the address contained by the bucket, but the bucket position in
	 | file.
Notes	 | 1) Splits the addressToSplit into two buckets.
	 | 2) Redistribute records between the old bucket and the new bucket.
	 | 3) Write the two buckets back to disc
	 | Creating a new address:
	 |  - This is done by adding another bit to the LEFT hand side of the current
	 | address.
	 | Example: Given the address 0010, the resulting buckets will have the
	 | addresses 00010, and 10010. Note that 00010 and the original 0010 addresses
	 | are essentially the same. This bucket is thus given the same relative bucket
	 | number as before the split. The 10010 address can be thought of as the "new"
	 | address. It is given the next available relative bucket number, ie, it will
	 | be appended to the end of the bucket file.
=========================================================================================
*/
int						    // The number of the new bucket
ExtendibleHashFile::
SplitBucket(int addressToSplit,			    // The address to split
	    int bucketDepth			    // Depth of bucket to split
	    )
{
  int oldAddress = GetLowestBits(addressToSplit, bucketDepth); 
                                                 // was int oldAddress = addressToSplit;
  int newAddress = oldAddress;// was int newAddress = addressToSplit;
  // Add the leading '1' to the new address
/*
  if ( !BitSet(newAddress, bucketDepth) ){
    // std::cout error
  }
*/
  BitSet(newAddress, bucketDepth);

  // Split the existing bucket into a new pair of buckets
  EHFBucket* existingBucket;			    // The existing bucket to be split
  EHFBucket* oldBucket;				    // The bucket with an added '0'
  EHFBucket* newBucket;				    // The bucket with an added '1'

  // Calculate the relative bucket positions of the two buckets in the file
  int oldBucketPos = _index->GetAddress(oldAddress);// The same as existingBucket
  int newBucketPos = _bucketCount;		    // Position at the end of the file
  // Update the number of buckets
  _bucketCount++;
  // Calculate the new bucket depth
  int newBucketDepth = (bucketDepth+1);

  existingBucket = new EHFBucket(_bucketFileFD,
				 _index->GetAddress(oldAddress)
				 );
  if (existingBucket->Read() != EHF_READOK){
    // std::cout error
  }

  oldBucket = new EHFBucket(_bucketFileFD,
			    oldBucketPos,	    // File position
			    newBucketDepth
			    );
  newBucket = new EHFBucket(_bucketFileFD,
			    newBucketPos,
			    newBucketDepth
			    );

  // Redistribute the records from the existing bucket to the new buckets
  char keyValue[IDSIZE+1];
  char record[RECORDSIZE+1];
  for (int i = 0; i < existingBucket->NumOfRecs(); i++){
    existingBucket->RetrieveRecAtIndex(i, keyValue, record);
    // Get 32 bit hash value
    int hashValue = Hash(keyValue);
    // Determine the address from the 32 bit hash value
    int result = GetLowestBits( hashValue, newBucketDepth );
    if (result == oldAddress){
      oldBucket->Add(keyValue, record);
    } else if (result == newAddress){
      newBucket->Add(keyValue, record);
    } else {
      // std::cout error - the address should match one of them!!!
      std::cout << "ERROR IN SPLIT BUCKET\n";
      exit(1);
    }
  }

  delete existingBucket;			       // All done with existing bucket

  // Write the two new buckets
  if (oldBucket->Write() != EHF_WROTEOK){
    // std::cout error
  }
  delete oldBucket;
  if (newBucket->Write() != EHF_WROTEOK){
    // std::cout error
  }
  delete newBucket;

  return newBucketPos;
}

/*
=========================================================================================
Name	 | RetrieveRecord
Purpose	 | Retrieve a record from an extendible hashing file
Returns	 | EHF_RETRIEVED   - Record was retrieved successfully
	 | EHF_NOT_PRESENT - The record was not present
	 | EHF_FILENOTOPEN - The bucket file was not open
	 | EHF_READERROR   - The bucket was not read correctly
Notes	 | The strings that are returned are null-terminated. Thus, the caller must
	 | ensure there is enough room for both the key/record and a null terminator.
=========================================================================================
*/
int
ExtendibleHashFile::
RetrieveRecord(char* keyToFind,				 //
	       char* returnRecord
	       )
{
  if (!_fileOpen){
    return EHF_FILENOTOPEN;
  }
  char key[IDSIZE+1];
  *((char *) mempcpy(key, keyToFind, IDSIZE)) = '\0';
  //strlcpy(key, keyToFind, IDSIZE+1);
  int hashValue = Hash(key);
  // now have a 32 bit hash value, but only need so many bits
  int address = GetLowestBits( hashValue, _index->GetDepth() );

  EHFBucket bucket(_bucketFileFD, _index->GetAddress(address));

  int readResult = bucket.Read();
  if (readResult != EHF_READOK){
    return readResult;
  }

  return ( bucket.Retrieve(keyToFind, returnRecord) );

}


int
ExtendibleHashFile::
DeleteRecord(char* keyToDelete
	     )
{
  return 1;
}

bool
ExtendibleHashFile::
OpenExistingFile(char* indexFileName,
		 char* bucketFileName
		 )
{
  // Open existing files with read write privilages
  _bucketFileFD = open(bucketFileName, O_RDWR);	      // Open bucket file
  _indexFileFD = open(indexFileName, O_RDWR);	      // Open index file

  if ( (_bucketFileFD >= 0) && (_indexFileFD >= 0) ){
    // Set up the index
    _index = new IndexHolder();
    _index->Load(_indexFileFD);
    if (ReadBucketCount() != EHF_READOK){
      return false;
    }
    return true;
  } else {
    return false;
  }
}



bool
ExtendibleHashFile::
CreateNewFile(char* indexFileName,
	      char* bucketFileName
	      )
{
  // Create new files with read write privilages, and truncate any existing files
  _bucketFileFD = open(bucketFileName, O_RDWR | O_CREAT | O_TRUNC, 0600);
  _indexFileFD = open(indexFileName, O_RDWR | O_CREAT | O_TRUNC, 0600);

  // Test if open was successful or not.
  if ( (_bucketFileFD >= 0) && (_indexFileFD >= 0) ){

    // Set up the index
    _index = new IndexHolder(1);		      // Initial index has depth 1

    _bucketCount = 2;				      // There are two buckets initially
    if ( WriteBucketCount() != EHF_WROTEOK){
      return false;
    }

    // Create an empty bucket. This dummy is use to setup the file
    EHFBucket* bucket = new EHFBucket( _bucketFileFD, 0, _index->GetDepth() );

    // Write bucket 0
    if ( bucket->Write() != EHF_WROTEOK){
      return false;
    }

    bucket->ChangeAddress(1);			      // Change address from 0 to 1
    // Write bucket 1
    if ( bucket->Write() != EHF_WROTEOK){
      return false;
    }

    _index->SetAddress(0,0);			      // Address 0 points at bucket 0
    _index->SetAddress(1,1);			      // Address 1 points at bucket 1

    return true;
  } else {
    return false;
  }
}

/*
=========================================================================================
Name	| ReadBucketCount
Purpose | Read in the file information (how many buckets are there?)
=========================================================================================
*/
int
ExtendibleHashFile::
ReadBucketCount(){
  if (_bucketFileFD < 0){
    return EHF_FILENOTOPEN;
  }

  lseek(_bucketFileFD, 0, SEEK_SET);
  int amount = read(_bucketFileFD, &_bucketCount, sizeof(_bucketCount));
  if (amount == sizeof(_bucketCount)){
    return EHF_READOK;
  } else {
    return EHF_READERROR;
  }
}

/*
=========================================================================================
Name	| WriteBucketCount
Purpose | Write the file information (how many buckets are there?)
=========================================================================================
*/
int
ExtendibleHashFile::
WriteBucketCount(){
  if (_bucketFileFD < 0){
    return EHF_FILENOTOPEN;
  }

  lseek(_bucketFileFD, 0, SEEK_SET);
  int amount = write(_bucketFileFD, &_bucketCount, sizeof(_bucketCount));
  if (amount == sizeof(_bucketCount)){
    return EHF_WROTEOK;
  } else {
    return EHF_WRITEERROR;
  }
}

/*
=========================================================================================
Name	| FileSummary
Purpose | Print a summary of the file's contents on the screen
=========================================================================================
*/
void
ExtendibleHashFile::
FileSummary(){
  if (!_fileOpen){
    return;
  }

  int loopCount = _index->GetNumberOfAddresses();

  std::cout << "Address			    Record\n";
  std::cout << "=======================================\n";

  char tempAdr[33];
  char tempRec[64];
  char tempKey[7];

  for (int i = 0; i < loopCount; i++){
    IntInBinary(i, 32, tempAdr);
    std::cout << tempAdr;

    // Get the bucket with address i
    EHFBucket* bucket = new EHFBucket(_bucketFileFD, _index->GetAddress(i));
    int readResult = bucket->Read();
    if (readResult != EHF_READOK){
      delete bucket;
      std::cout << "Read bucket error in FileSummary();" << std::endl;
    }
    int numRecs = bucket->NumOfRecs();
    for (int x = 0; x < numRecs; x++){
      bucket->RetrieveRecAtIndex(x, tempKey, tempRec);
      std::cout << " " << tempKey << std::endl;
      if (x != (numRecs-1)){
	// print alignment spacing for the next one
	std::cout << "				";
      }
    }
    delete bucket;
    std::cout << "Records in bucket "<<i<<" : "<< numRecs<<std::endl;
    std::cout << "---------------------------------------\n";
    std::cout << std::endl;
  }
}

/*
These are the first 58 records contained in the CompBookData file
Each has been given it's relative record number, from 0 - 57

00 148000A primer in data reduction : aEhrenberg, A. SQA276.12 E33
01 058200Advanced programming : a practBarron, D. W. (QA76.6 B277
02 099900Almost sure convergence       Stout, William QA273.5 S889
03 058900Automated reasoning : introducWos, Larry.    QA76.9 A96 A
04 099800Computer case histories;      Summersbee, StuQA76 S955 C
05 101100Computers and data processing.Dopping, Olle, QA76.5 D692
06 040000Concurrent Euclid, The UNIX syHolt, R. C. (RiQA76.73 C64
07 157500Controlling and auditing smallGaston, S. J.  QA76.5 G256
08 108600Coroutines : a programming metMarlin, ChristoQA76.6 M348
09 060000Distributed computing systems Paker, Yakup.|VQA76.9 D5 D6
10 057100Educated guessing : how to copKotz, Samuel.|SQA273 K87 E
11 096100Factor analysis of data matricHorst, Paul, 19QA276 H819 F
12 079800Fifth generation computers : cBishop, Peter, QA76.5 B622
13 068000Foiling the system breakers : Lobel, Jerome. QA76.9 A25 L
14 012300Handbook of successful data prPescow, Jerome HF5548.2 P47
15 061800Handbook of systems analysis :Miser, Hugh J.,T57.6 H236
16 036100Implementation : the key to suLucas, Henry C.HF5548.2 L93
17 112800Inference and decision /      Menges, Gunter.QA276 I43
18 105200Information processing systemsDavis, William QA76 D265 I
19 016200Information systems for modernMurdick, RobertHF5548.2 M97
20 209700Logic-based decision support :Jeroslow, RoberT58.62 J56 L
21 011100Management of computer-based d		      HF5548.2 M26
22 132600Mathematics of statistics /   Kenney, John F.QA276 K36 M
23 096900Multichannel time series analyRobinson, EnderQA276 R659 M
24 040200Principles of data security / Leiss, Ernst L.QA76.9 A25 L
25 054300Probability and random processGrimmett, GeoffQA273 G864 P
26 217900Probability and statistics: anDwass, Meyer.  QA273 D989 P
27 160600Radically elementary probabiliNelson, Edward,QA274.5 N425
28 057200Security, IFIP/Sec'83 : proceeFak, Viiveke, 1QA76.9 A25 S
29 095900Statistical inference	       Li, Jerome C. RQA276 L693 S
30 110200Statistical tables for the socPowell, F. C. (QA276.25 S79
31 123400Stopped random walks : limit tGut, Allan, 194QA274.73 G98
32 182200Structural equation modeling wHayduk, Leslie QA278.3 H416
33 096600The analysis of variance.     Huitson, Alan. QA276 H911 A
34 040400The social impact of micro-ele		      QA76.9 C66 R
35 070301Algebraic approaches to prograManes, Ernest GQA76.7 M274
36 023801An introduction to database syDate, C. J.    QA76.9 D3 D2
37 210001Application of computer technoBecker, Joseph.Z678.9 B395
38 210101Automation guidelines for publVickers, P. H. Z678.9 G786
39 208501Computational algorithms for cBruell, Steven T57.9 B889 C
40 157401Computer system performance   Hellerman, HerbQA76.5 H477
41 166101Concurrent prolog : collected Shapiro, Ehud YQA76.73 P76
42 009301Data processing for business aThierauf, RoberHF5548.2 T43
43 103801Design and analysis of experimMontgomery, DouQA279 M787 D
44 167101Dynamic graphics for statisticCleveland, WillQA276.3 D997
45 071001Managing with micros : managemLewis, C. D. (CHF5548.2 L67
46 099501Modular programming.	       Maynard, Jeff. QA76.6 M471
47 201801Non-uniform random variate genDevroye, Luc.  QA274 D514 N
48 055401Nonparametric functional estimPrakasa Rao, B.QA278.8 P898
49 149701Probability theory and mathemaFisz, Marek.   QA273 F543 P
50 101401Quantitative analysis methods DeFrancesco, HeQA276 D316 Q
51 097901Queuing theory; a study of waiPanico, Joseph QA273 P192 Q
52 099901Random integral equations withTsokos, Chris PQA274.27 T88
53 004401SNOBOL3 primer; an introductioForte, Allen.  QA76.73 S6 F
54 062201Spatial data analysis by exampUpton, Graham JQA278.2 U71
55 112901Structured concurrent programmHolt, Richard CQA76.6 S927
56 063501The Carnegie-Mellon curriculumShaw, Mary, 194QA76.27 C289
57 022001The anatomy of business : an iGeorge, F. H. (HF5548.2 G34
*/
