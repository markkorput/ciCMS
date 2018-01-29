#include "catch.hpp"
#include <iostream>

#include "ciCMS/cfg/ctree/Builder.h"


// using namespace cms;

#ifdef CICMS_CTREE

TEST_CASE("cms::cfg::ctree::Builder", ""){
  SECTION("typical_usage"){
    // builder instance
    cms::cfg::ctree::Builder;
    
  }
}

#endif // CICMS_CTREE
