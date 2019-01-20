#include "gtest/gtest.h"

#include "indexholder.h"

TEST(IndexHolderConstruction, DepthOfOne) {
  int depth = 1;
  IndexHolder ih(depth);
  ASSERT_EQ(ih.GetDepth(), depth);
  ASSERT_EQ(ih.GetNumberOfAddresses(), (1 << (depth)));
}

TEST(IndexHolderConstruction, DepthOfOneTryDecrease) {
  int depth = 1;
  IndexHolder ih(depth);
  ASSERT_EQ(ih.GetDepth(), depth);
  ASSERT_EQ(ih.GetNumberOfAddresses(), (1 << (depth)));
  bool decreased = ih.DecreaseDepth();
  ASSERT_EQ(decreased, false);
}

TEST(IndexHolderConstruction, DepthOfTwoTryDecrease_Stack) {
  int depth = 2;
  IndexHolder ih(depth);
  ASSERT_EQ(ih.GetDepth(), depth);
  ASSERT_EQ(ih.GetNumberOfAddresses(), (1 << (depth)));
  bool decreased = ih.DecreaseDepth();
  ASSERT_EQ(decreased, true);
}

TEST(IndexHolderConstruction, DepthOfTwoTryDecrease_Heap) {
  int depth = 2;
  IndexHolder* ih = new IndexHolder(depth);
  bool decreased = ih->DecreaseDepth();
  ASSERT_EQ(decreased, true);
  depth--;
  ASSERT_EQ(ih->GetDepth(), depth);
  ASSERT_EQ(ih->GetNumberOfAddresses(), (1 << (depth)));
  delete ih;
  ih = nullptr;
}

TEST(IndexholderDynamism, IncreaseDepthFrom1to2) {
  int depth = 1;
  IndexHolder* ih = new IndexHolder(depth);
  ih->IncreaseDepth();
  depth++;
  ASSERT_EQ(ih->GetDepth(), depth);
  ASSERT_EQ(ih->GetNumberOfAddresses(), (1 << (depth)));
  delete ih;
  ih = nullptr;
}

TEST(IndexholderDynamism, IncreaseDepthFrom1toTargetDepthThenDecrease) {
  int depth = 1;
  int targetdepth = 2;
  IndexHolder* ih = new IndexHolder(depth);
  while (depth < targetdepth) {
    ih->IncreaseDepth();
    depth++;
    ASSERT_EQ(ih->GetDepth(), depth);
    ASSERT_EQ(ih->GetNumberOfAddresses(), (1 << (depth)));
  }

  ih->DecreaseDepth();
  depth--;
  ASSERT_EQ(ih->GetDepth(), depth);
  ASSERT_EQ(ih->GetNumberOfAddresses(), (1 << (depth)));
  delete ih;
  ih = nullptr;
}


TEST(IndexholderDynamism, DecreaseDepthFrom3to2stack) {
  int depth = 3;
  IndexHolder ih(depth); 
  ASSERT_EQ(ih.GetDepth(), depth);
  ASSERT_EQ(ih.GetNumberOfAddresses(), (1 << (depth)));
  bool decreased = ih.DecreaseDepth();
  ASSERT_EQ(decreased, true);
}
 
TEST(IndexholderDynamism, DecreaseDepthFrom3to2heap) {
  int depth = 3;
  IndexHolder* ih = new IndexHolder(depth); 
  ASSERT_EQ(ih->GetDepth(), depth);
  ASSERT_EQ(ih->GetNumberOfAddresses(), (1 << (depth)));
  ih->Print();

  bool decreased = ih->DecreaseDepth();
  ASSERT_EQ(decreased, true);
  depth--;
  ASSERT_EQ(ih->GetDepth(), depth);
  ASSERT_EQ(ih->GetNumberOfAddresses(), (1 << (depth)));
  ih->Print();
  delete ih;
  ih = nullptr;
}
