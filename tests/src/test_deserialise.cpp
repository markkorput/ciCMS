#include "catch.hpp"
#include "ciCMS/Model.h"
#include "ciCMS/deserialise.h"

using namespace std;
using namespace cms;

TEST_CASE("cms::deserialise", ""){
    SECTION("deserialiseBool"){
        // true values
        REQUIRE(deserialiseBool("true", true) == true);
        REQUIRE(deserialiseBool("true", false) == true);
        REQUIRE(deserialiseBool("TrUe", false) == true);
        REQUIRE(deserialiseBool("tRUe", false) == true);
        REQUIRE(deserialiseBool("1", true) == true);
        REQUIRE(deserialiseBool("1", false) == true);

        // false values
        REQUIRE(deserialiseBool("false", true) == false);
        REQUIRE(deserialiseBool("false", false) == false);
        REQUIRE(deserialiseBool("FALSE", true) == false);
        REQUIRE(deserialiseBool("faLSe", false) == false);

        REQUIRE(deserialiseBool("0", true) == false);
        REQUIRE(deserialiseBool("0", false) == false);

        // invalid values
        REQUIRE(deserialiseBool("abc", true) == true);
        REQUIRE(deserialiseBool("abc", false) == false);
    }
}

TEST_CASE("cms::serde", ""){
    SECTION("vec2"){
        auto assertValues = [](const std::string& val, const glm::vec2& fallback, const glm::vec2& expectedResult){
            glm::vec2 result;
            serde::vec2(result, val, fallback);
            REQUIRE(result == expectedResult);
        };

        glm::vec2 fallback(99, 99);

        assertValues("11.44,88.66", fallback, glm::vec2(11.44f, 88.66f));
        assertValues("33.44, 55.66", fallback, glm::vec2(33.44f, 55.66f));
        assertValues("a0,0", fallback, fallback);
        assertValues("1,b2", fallback, fallback);
        assertValues("abcd", fallback, fallback);
    }
}