#include "catch.hpp"

#include "ciCMS/cfg/utils.h"

using namespace std;
using namespace cms;
using namespace cms::cfg;

TEST_CASE("cms::cfg::split"){
  SECTION("empty string"){
    vector<string> parts;
    split(parts, "", ',');
    REQUIRE(parts.size() == 0);
  }

  SECTION("non empty string"){
    vector<string> parts;
    split(parts, "ab,cd", ',');
    REQUIRE(parts.size() == 2);
    REQUIRE(parts[0] == "ab");
    REQUIRE(parts[1] == "cd");
  }
}


TEST_CASE("cms::cfg::join"){
  SECTION("empty vector"){
    vector<string> parts = {};
    string result = join(parts);
    REQUIRE(result == "");
  }

  SECTION("non empty vector"){
    vector<string> parts = {"ab", "cd", "ef"};
    string result = join(parts);
    REQUIRE(result == "ab,cd,ef");
  }
}
