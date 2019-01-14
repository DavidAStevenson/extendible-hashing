#include <catch2/catch.hpp>

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
