#ifndef __LHFConsTS__
#define __LHFConsTS__

// Normal returns
const int EHF_READOK = 1;                     // Read was successful
const int EHF_WROTEOK = 1;                    // Write was successful
const int EHF_INSERTED = 1;                   // Record was inserted successfully
const int EHF_DELETED = 1;                    // Record was deleted successfully
const int EHF_RETRIEVED = 1;                  // Record was retrieved successfully
// Logical error returns
const int EHF_ALREADY_PRESENT = 2;            // The record was already present
const int EHF_NOT_PRESENT = 3;                // The record was not present
const int EHF_FULLBUCKET = 4;                 // The bucket was full
const int EHF_MAXTABLEDEPTH = 5;              // The table has reached maximum depth

// File IO error returns
const int EHF_FILENOTOPEN = 6;
const int EHF_WRITEERROR = 7;
const int EHF_READERROR = 8;

#endif
