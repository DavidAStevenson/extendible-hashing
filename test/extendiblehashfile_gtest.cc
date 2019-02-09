#include <string>

#include "gtest/gtest.h"

#include "ehfconsts.h"
#include "extendiblehashfile.h"

TEST(EHFConstruction, SimpleOpenNewClose) {
  ExtendibleHashFile* ehf = new ExtendibleHashFile();
  ASSERT_EQ(ehf->IsOpen(), false);

  char filename[30];
  strcpy(filename, "ehf.gtest");
  // Open a file that doesn't exist
  ASSERT_EQ(ehf->Open(filename, false), true);
  ASSERT_EQ(ehf->IsOpen(), true);
  ehf->Close();

  ASSERT_EQ(ehf->Open(filename), true);
  ASSERT_EQ(ehf->IsOpen(), true);
  ehf->Close();

  strcpy(filename, "ehf.gtest-fail");
  ASSERT_EQ(ehf->Open(filename), false);
  ASSERT_EQ(ehf->IsOpen(), false);

  delete ehf;
  ehf = nullptr;
}

TEST(EHFInsert, SimpleInsert) {
  ExtendibleHashFile* ehf = new ExtendibleHashFile();
  char filename[30];
  strcpy(filename, "ehf.gtest");
  char key[7];
  strcpy(key, "123456");
  char record[1024];
  strcpy(record, "123456Record for 123456");
  ASSERT_EQ(ehf->Open(filename, false), true);
  int retval = ehf->InsertRecord(key, record);
  ASSERT_EQ(retval, EHF_INSERTED);
  ehf->Close();
  delete ehf;
  ehf = nullptr;
}

TEST(EHFInsertAndRetrive, SimpleInsertAndRetrieve) {
  ExtendibleHashFile* ehf = new ExtendibleHashFile();
  char filename[30];
  strcpy(filename, "ehf.gtest");
  ASSERT_EQ(ehf->Open(filename), true);
  char key[7];
  strcpy(key, "123456");
  char record[1024];
  int retval = ehf->RetrieveRecord(key, record);
  ASSERT_EQ(retval, EHF_RETRIEVED);
  ASSERT_EQ(strcmp("123456Record for 123456", record), 0);
  ehf->Close();
  delete ehf;
  ehf = nullptr;
}

TEST(EHFInsertRetriveAndDelete, SimpleInsertRetrieveAndDelete) {
  ExtendibleHashFile* ehf = new ExtendibleHashFile();
  char filename[30];
  strcpy(filename, "ehf.gtest");
  ASSERT_EQ(ehf->Open(filename), true);
  char key[7];
  strcpy(key, "123456");
  int retval = ehf->DeleteRecord(key);
  ASSERT_EQ(retval, EHF_DELETED);
  ehf->Close();
  delete ehf;
  ehf = nullptr;
}

TEST(EHFInsertForty, InsertAndCauseExpansion) {
  ExtendibleHashFile* ehf = new ExtendibleHashFile();
  char filename[30];
  strcpy(filename, "ehf.gtest");
  ASSERT_EQ(ehf->Open(filename, false), true);

  char key[7];
  char record[1024];
  for (int i = 40; i >= 0; i--) {
    std::string recNo = std::to_string(i);
	while (recNo.length() < 6) {
	  recNo = '0' + recNo;
	}
	std::string recordStr = recNo + "Record for " + recNo;

    strcpy(key, recNo.data());
    strcpy(record, recordStr.data());
    int retval = ehf->InsertRecord(key, record);
    ASSERT_EQ(retval, EHF_INSERTED);
  }

  ehf->Close();
  delete ehf;
  ehf = nullptr;
}

TEST(EHFRetrieveForty, RetrieveFortyFromExpandedFile) {
  ExtendibleHashFile* ehf = new ExtendibleHashFile();
  char filename[30];
  strcpy(filename, "ehf.gtest");
  ASSERT_EQ(ehf->Open(filename), true);
  char key[7];
  char record[1024];
  for (int i = 40; i >= 0; i--) {
    std::string recNo = std::to_string(i);
	while (recNo.length() < 6) {
	  recNo = '0' + recNo;
	}
    strcpy(key, recNo.data());
    int retval = ehf->RetrieveRecord(key, record);
    ASSERT_EQ(retval, EHF_RETRIEVED);
	std::string recordStr = recNo + "Record for " + recNo;
    ASSERT_EQ(strcmp(recordStr.data(), record), 0);
  }
  ehf->Close();
  delete ehf;
  ehf = nullptr;
}

