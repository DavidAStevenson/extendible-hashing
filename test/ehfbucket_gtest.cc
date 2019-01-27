#include "gtest/gtest.h"

#include "ehfconsts.h"
#include "ehfbucket.h"

#include <fcntl.h>

TEST(EHFBucketConstruction, Simple) {
  // TODO Interesting thing here is I need to pass a file handle
  // This is bad from perspective of testing this thing
  // Perhaps better design would be to abstract a file handle into an object,
  // and construct the EHFBucket with mock instances of the abstract file handler object
  // This way enable unit testing without bringing the real file system into it...
  // Maybe GMock is useful for that?

  int fd = open("ehfbucket.gtest", O_CREAT | O_TRUNC | O_RDWR, 0600);

  EHFBucket* b = new EHFBucket(fd, 0);
  ASSERT_EQ(b->NumOfRecs(), 0);
  ASSERT_EQ(b->Depth(), 1);

  delete b;
  b = nullptr;
  close(fd);
}

TEST(EHFBucketConstruction, InitialWrite) {
  int fd = open("ehfbucket.gtest", O_CREAT | O_TRUNC | O_RDWR, 0600);

  EHFBucket* b = new EHFBucket(fd, 0, 1);
  ASSERT_EQ(b->Write(), EHF_WROTEOK);
  b->ChangeAddress(1);
  ASSERT_EQ(b->Write(), EHF_WROTEOK);

  delete b;
  b = nullptr;
  close(fd);
}

TEST(EHFBucketConstruction, ExistingBucketRead) {
  int fd = open("ehfbucket.gtest", O_RDWR);

  EHFBucket* b = new EHFBucket(fd, 0);
  ASSERT_EQ(b->Read(), EHF_READOK);
  
  delete b;
  b = nullptr;
  close(fd);
}

// TODO gtest teardown? don't want to leave the test files sitting around

TEST(EHFBucketUsage, AddToEmptyBucketRetrieveThenDelete) {
  // TODO Better to create test file in some kind of test setup step
  int fd = open("ehfbucket.gtest", O_RDWR);

  EHFBucket* b = new EHFBucket(fd, 0);

  char key[IDSIZE+1];
  strcpy(key, "148000");
  char record_in[RECORDSIZE+1];
  strcpy(record_in, "148000A primer in data reduction : aEhrenberg, A. SQA276.12 E33");
  ASSERT_EQ(b->NumOfRecs(), 0);
  ASSERT_EQ(b->Add(key, record_in), EHF_INSERTED);
  ASSERT_EQ(b->NumOfRecs(), 1);
  ASSERT_EQ(b->RecordPosition(key), 0);

  { 
    char record_out[RECORDSIZE+1];
    ASSERT_EQ(b->Retrieve(key, record_out), EHF_RETRIEVED);
    ASSERT_EQ(strncmp(record_in, record_out, RECORDSIZE+1), 0);
  }

  { 
    char key_out[IDSIZE+1];
    char record_out[RECORDSIZE+1];
    b->RetrieveRecAtIndex(0, key_out, record_out);
    ASSERT_EQ(strncmp(key, key_out, IDSIZE+1), 0);
    ASSERT_EQ(strncmp(record_in, record_out, RECORDSIZE+1), 0);
  }

  ASSERT_EQ(b->Add(key, record_in), EHF_ALREADY_PRESENT);

  ASSERT_EQ(b->Delete(key), EHF_DELETED);
  {
    char record_out[RECORDSIZE+1];
    ASSERT_EQ(b->Retrieve(key, record_out), EHF_NOT_PRESENT);
  }
}

// TODO tests to max out a bucket
// TODO test writing / reading a maxed out bucket from disk

