#include <catch2/catch.hpp>

#include <unistd.h>
#include <fcntl.h>

#include "indexholder.h"

TEST_CASE("Create an indexholder (default depth)", "[IndexHolder construction]") {
  IndexHolder* ih = new IndexHolder();

  REQUIRE(ih->GetDepth() == 1);

  delete ih;
  ih = nullptr;	
}

TEST_CASE("Create an indexholder with depth = 2", "[IndexHolder construction]") {
  IndexHolder* ih = new IndexHolder(2);

  REQUIRE(ih->GetDepth() == 2);

  delete ih;
  ih = nullptr;	
}

TEST_CASE("Increase/Decrease index depth - basic", "[IndexHolder-dynamism]") {
  IndexHolder* ih = new IndexHolder();
  REQUIRE(ih->GetNumberOfAddresses() == (1 << 1));
  REQUIRE(ih->GetDepth() == 1);
  ih->IncreaseDepth();
  REQUIRE(ih->GetNumberOfAddresses() == (1 << 2));
  REQUIRE(ih->GetDepth() == 2);
  ih->DecreaseDepth();
  REQUIRE(ih->GetNumberOfAddresses() == (1 << 1));
  REQUIRE(ih->GetDepth() == 1);
}

TEST_CASE("Increase/Decrease index depth", "[IndexHolder-dynamism]") {
  IndexHolder* ih = new IndexHolder(8);
  ih->IncreaseDepth();
  REQUIRE(ih->GetDepth() == 9);
  REQUIRE(ih->GetNumberOfAddresses() == (1 << 9));
//  ih->DecreaseDepth();
  REQUIRE(ih->GetDepth() == 8);
//  ih->DecreaseDepth();
//  REQUIRE(ih->GetDepth() == 7);
//  REQUIRE(ih->GetNumberOfAddresses() == (1 << 7));
}


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
