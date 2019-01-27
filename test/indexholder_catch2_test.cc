#include <catch2/catch.hpp>

#include <unistd.h>
#include <fcntl.h>

#include "indexholder.h"


////
// bool DecreaseDepth();

//void SetAddress(int index, int address);
//int GetAddress(int index);

// bool Load(int fileDescriptor);
// bool Write(int fileDescriptor);
////
TEST_CASE("Create an indexholder (default depth)", "[IndexHolder construction]") {
  IndexHolder* ih = new IndexHolder();

  REQUIRE(ih->GetDepth() == 1);

  delete ih;
  ih = nullptr;
}

// This usage doesn't really make sense - constructor arg of zero suggests
// that GetDepth ought be 0 too, but it's currently 1
TEST_CASE("Create an indexholder with depth = 0", "[IndexHolder construction]") {
  IndexHolder* ih = new IndexHolder(0);

  REQUIRE(ih->GetDepth() == 1);

  delete ih;
  ih = nullptr;
}

TEST_CASE("Create an indexholder with depth = 1", "[IndexHolder construction]") {
  IndexHolder* ih = new IndexHolder(1);

  REQUIRE(ih->GetDepth() == 1);

  delete ih;
  ih = nullptr;
}

TEST_CASE("Create indexholders with depth < 20", "[IndexHolder construction]") {
  // Going to 31 results in 2GB of allocation
  // the code doesn't handle it gracefully, so that's a problem...
  // Keep it modest at 20
  int max_depth = 20;
  for (int i = 2; i < max_depth; i++) {
    int depth = i;
    IndexHolder* ih = new IndexHolder(depth);

    REQUIRE(ih->GetDepth() == depth);
    REQUIRE(ih->GetNumberOfAddresses() == (1 << (depth)));

    delete ih;
    ih = nullptr;
  }
}

TEST_CASE("Increase index depth", "[IndexHolder-increase-depth]") {
  int depth = 1;
  IndexHolder* ih = new IndexHolder(depth);
  REQUIRE(ih->GetDepth() == depth);
  REQUIRE(ih->GetNumberOfAddresses() == (1 << (depth)));

  int target_depth = 5;
  do {
    ih->IncreaseDepth();
    depth++;
    REQUIRE(ih->GetDepth() == depth);
    REQUIRE(ih->GetNumberOfAddresses() == (1 << (depth)));
  } while (depth < target_depth);
}

TEST_CASE("Decrease index depth", "[IndexHolder-decrease-depth]") {
  int depth = 3;
  IndexHolder* ih = new IndexHolder(depth);
  REQUIRE(ih->GetDepth() == depth);
  REQUIRE(ih->GetNumberOfAddresses() == (1 << (depth)));

  REQUIRE(ih->DecreaseDepth() == true);
  depth--;
  REQUIRE(ih->GetDepth() == depth);
  REQUIRE(ih->GetNumberOfAddresses() == (1 << (depth)));
  delete ih;
  ih = nullptr;
}

TEST_CASE("Increase/Decrease index depth - basic", "[IndexHolder-dynamism]") {
  IndexHolder* ih = new IndexHolder(1);
  REQUIRE(ih->GetDepth() == 1);
  REQUIRE(ih->GetNumberOfAddresses() == (1 << 1));

  ih->IncreaseDepth();
  REQUIRE(ih->GetDepth() == 2);
  REQUIRE(ih->GetNumberOfAddresses() == (2 << 1));

  ih->DecreaseDepth();
  REQUIRE(ih->GetDepth() == 1);
  REQUIRE(ih->GetNumberOfAddresses() == (1 << 1));
  delete ih;
  ih = nullptr;
}

TEST_CASE("Increase/Decrease index depth", "[IndexHolder-dynamism]") {
  int depth = 2;
  IndexHolder* ih = new IndexHolder(depth);

  ih->IncreaseDepth();
  depth++;
  REQUIRE(ih->GetDepth() == depth);
  REQUIRE(ih->GetNumberOfAddresses() == (1 << (depth)));

  REQUIRE(ih->DecreaseDepth() == true);
  depth--;
  REQUIRE(ih->GetDepth() == depth);
  REQUIRE(ih->GetNumberOfAddresses() == (1 << (depth)));

  ih->DecreaseDepth();
  depth--;
  REQUIRE(ih->GetDepth() == depth);
  REQUIRE(ih->GetNumberOfAddresses() == (1 << depth));

  ih->IncreaseDepth();
  depth++;
  REQUIRE(ih->GetDepth() == depth);
  REQUIRE(ih->GetNumberOfAddresses() == (1 << (depth)));
  delete ih;
  ih = nullptr;
}

TEST_CASE("Set Addreses then increase depth", "[IndexHolder-dynamism]") {
  IndexHolder* ih = new IndexHolder(1);
  ih->SetAddress(0, 3);
  ih->SetAddress(1, 7);
  REQUIRE(ih->GetAddress(0) == 3);
  REQUIRE(ih->GetAddress(1) == 7);
  ih->IncreaseDepth();
  REQUIRE(ih->GetAddress(0) == 3);
  REQUIRE(ih->GetAddress(1) == 7);
  REQUIRE(ih->GetAddress(2) == 3);
  REQUIRE(ih->GetAddress(3) == 7);

  REQUIRE(ih->DecreaseDepth() == true);
  REQUIRE(ih->GetAddress(0) == 3);
  REQUIRE(ih->GetAddress(1) == 7);
  delete ih;
  ih = nullptr;
}

TEST_CASE("Can't decrease depth with non-matching buddies", "[IndexHolder-dynamism]") {
  IndexHolder* ih = new IndexHolder(1);
  ih->SetAddress(0, 3);
  ih->SetAddress(1, 7);
  ih->IncreaseDepth();
  ih->SetAddress(2, 4); // buddy indexes (0, 2) have different addresses
  REQUIRE(ih->GetAddress(0) == 3);
  REQUIRE(ih->GetAddress(1) == 7);
  REQUIRE(ih->GetAddress(2) == 4);
  REQUIRE(ih->GetAddress(3) == 7);

  REQUIRE(ih->DecreaseDepth() == false); 
  // addresses have not changed because depth couldn't be decreased
  REQUIRE(ih->GetAddress(0) == 3);
  REQUIRE(ih->GetAddress(1) == 7);
  REQUIRE(ih->GetAddress(2) == 4);
  REQUIRE(ih->GetAddress(3) == 7);
  delete ih;
  ih = nullptr;
}

TEST_CASE("legacy test", "[Legacy-test-from-Y2K]") {
  int indexDepth = 1;
  IndexHolder* myIndex = new IndexHolder(indexDepth);

  myIndex->SetAddress(1, 3);
  REQUIRE(myIndex->GetAddress(1) == 3);
  // nasty interface - no hint that this is going to have no effect
  myIndex->SetAddress(2, 7);
  REQUIRE(myIndex->GetAddress(2) == -1);

  myIndex->SetAddress(0, 7);
  REQUIRE(myIndex->GetAddress(0) == 7);

  /*
    cout << "Now print myIndex\n" << flush;
    myIndex->PrintIndex();
    cout << "Done\n" << flush;
    cout << "=============================" << endl << flush;
    cout << "now expand...\n";
   */
  myIndex->IncreaseDepth();
  REQUIRE(myIndex->GetAddress(0) == 7);
  REQUIRE(myIndex->GetAddress(1) == 3);
  REQUIRE(myIndex->GetAddress(2) == 7);
  REQUIRE(myIndex->GetAddress(3) == 3);
  /*
    myIndex->PrintIndex();
    cout << "=============================" << endl << flush;
    cout << "Now try to decrease the index size...\n";
   */
  REQUIRE(myIndex->DecreaseDepth() == true);
/*
    if (myIndex->DecreaseDepth()){
    cout << "SUCCESS! Now print the shrunken myIndex\n" << flush;
    myIndex->PrintIndex();
    }
    cout << "depth : " << myIndex->GetDepth() << '\n';
    cout << "addresses : " << myIndex->GetNumberOfAddresses() << '\n';
    int input;
    cout << "value? ";
    cin >> input;
    cout << input << " points to " << myIndex->GetAddress(input) << endl;
  */
  REQUIRE(myIndex->GetAddress(0) == 7);
  REQUIRE(myIndex->GetAddress(1) == 3);
  REQUIRE(myIndex->GetAddress(2) == -1);
  REQUIRE(myIndex->GetAddress(3) == -1);

  // TODO don't like opening the files just anywhere, how about a test subdir somewhere?
  int fd = open("testfile", O_RDWR | O_CREAT | O_TRUNC, 0600);
  REQUIRE (myIndex->Write(fd)); /* {
    std::cout << "Wrote file ok\n";
  } else {
    std::cout << "Else it is fuct (write)\n";
  }
  */

  // From this next block, it appears that we can delete an indexholder,
  // but if we persisted it to disk then we can read it back to memory again
  delete myIndex;
  myIndex = new IndexHolder();
  REQUIRE(myIndex->GetAddress(0) == -1);
  REQUIRE(myIndex->GetAddress(1) == -1);
  //cout << "now load " << endl;
  REQUIRE (myIndex->Load(fd)); /*{
    cout << "Load file ok\n";
  } else {
    cout << "Else it is fuct (load)\n";
  }
  */
  REQUIRE(myIndex->GetAddress(0) == 7);
  REQUIRE(myIndex->GetAddress(1) == 3);
  close(fd);
}
