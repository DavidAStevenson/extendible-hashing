#include <catch2/catch.hpp>

#include <unistd.h>
#include <fcntl.h>

#include "indexholder.h"


////

// bool DecreaseDepth();

// int GetDepth();
// int GetNumberOfAddresses();

//void SetAddress(int index, int address);
//int GetAddress(int index);

//void Print();
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

TEST_CASE("Create an indexholder with depth = 2", "[IndexHolder construction]") {
  int depth = 2;
  IndexHolder* ih = new IndexHolder(depth);

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
}

TEST_CASE("Decrease index depth", "[IndexHolder-dynamism]") {
  int depth = 3;
  IndexHolder* ih = new IndexHolder(depth);
  REQUIRE(ih->GetDepth() == depth);
  REQUIRE(ih->GetNumberOfAddresses() == (1 << (depth)));
  //ih->Print();

  REQUIRE(ih->DecreaseDepth() == true);
  depth--;
  REQUIRE(ih->GetDepth() == depth);
  REQUIRE(ih->GetNumberOfAddresses() == (1 << (depth)));
  //ih->Print();
  delete ih;
  ih = nullptr;
}

/*
TEST_CASE("Increase/Decrease index depth", "[IndexHolder-dynamism]") {
  int depth = 2;
  IndexHolder* ih = new IndexHolder(depth);

  ih->IncreaseDepth();
  depth++;
  REQUIRE(ih->GetDepth() == depth);
  REQUIRE(ih->GetNumberOfAddresses() == (1 << (depth)));

  // Calling this is like crossing the streams - issues lurk
  REQUIRE(ih->DecreaseDepth() == true);
  depth--;
  REQUIRE(ih->GetDepth() == depth);
  REQUIRE(ih->GetNumberOfAddresses() == (1 << (depth)));
//  ih->DecreaseDepth();
//  REQUIRE(ih->GetDepth() == 7);
//  REQUIRE(ih->GetNumberOfAddresses() == (1 << 7));
  ih->IncreaseDepth();
  depth++;
  REQUIRE(ih->GetDepth() == depth);
  REQUIRE(ih->GetNumberOfAddresses() == (1 << (depth)));
}
*/

TEST_CASE("Run legacy test", "[Pending refactoring]") {
  int indexDepth = 1;
  IndexHolder* myIndex = new IndexHolder(indexDepth);
  // TODO - no idea what these args mean...
  myIndex->SetAddress(1, 3);
  myIndex->SetAddress(2, 7);
  myIndex->Print();
  /*
    cout << "Now print myIndex\n" << flush;
    myIndex->PrintIndex();
    cout << "Done\n" << flush;
    cout << "=============================" << endl << flush;
    cout << "now expand...\n";
    myIndex->IncreaseDepth();
    cout << "Now print the expanded myIndex\n" << flush;
    myIndex->PrintIndex();
    cout << "=============================" << endl << flush;
    cout << "Now try to decrease the index size...\n";
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
  myIndex->Print();
  //cout << "now load " << endl;
  REQUIRE (myIndex->Load(fd)); /*{
    cout << "Load file ok\n";
  } else {
    cout << "Else it is fuct (load)\n";
  }
  */
  myIndex->Print();
  close(fd);
}
