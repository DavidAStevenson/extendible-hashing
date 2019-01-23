#ifndef __BOOKRecoRDConSTaNTS_
#define __BOOKRecoRDConSTaNTS_
// define the number of bytes used for book infomation
// any changes to these constants will cause changes to FULLBUCKET
const int IDPOSITION = 0;
const int IDSIZE = 6;
const int TITLESIZE = 30;
const int AUTHORSIZE = 15;
const int CALLCODESIZE = 12;
const int RECORDSIZE = IDSIZE + TITLESIZE + AUTHORSIZE + CALLCODESIZE;

// the number of bytes that can be read in one disk access
const int SECTORSIZE = 1024;

// maximum records per sector
const int FULLBUCKET = 16; 

// the size of total book info in a bucket
const int RECSBUFFSIZE = RECORDSIZE*FULLBUCKET + 8;
// the extra 8 is just purely padding to keep each sector 1024 bytes.

// the size of a bucket including overhead info
const int POINTERSIZE = 4;

// one bucket consists of two integers and FULLBUCKET number of records.
// integer one stores the depth of the address of this bucket
// integer two stores the number of records in the bucket (initial zero)
const int BUCKETSIZE = RECSBUFFSIZE + 2 * POINTERSIZE; 

// Size of the header of the bucket file. One integer 
const int FILEHEADERSIZE = 4;


#endif
