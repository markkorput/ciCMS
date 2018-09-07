#include "catch.hpp"
#include "ciCMS/ciCMS.h"

using namespace std;
using namespace cms;

TEST_CASE("cms::ModelCollection", ""){
    SECTION("filter"){
        cms::ModelCollection col;
        shared_ptr<cms::Model> modelRef;
        modelRef = make_shared<cms::Model>();
        col.add(modelRef);
        REQUIRE(col.size() == 1);
        col.filter("name", "Doe");
        REQUIRE(col.size() == 0);
        modelRef = make_shared<cms::Model>();
        col.add(modelRef);
        REQUIRE(col.size() == 0);
        modelRef->set("name", "Doe");
        col.add(modelRef);
        REQUIRE(col.size() == 1);
    }

    SECTION("findByAttr"){
        ModelCollection col;
        auto m1 = col.create();
        m1->set("value", "10");
        auto m2 = col.create();
        m2->set("value", "20");
        auto m3 = col.create();
        m3->set("value", "30");
        REQUIRE(col.findByAttr("value", "20") == m2);
        REQUIRE(col.findByAttr("value", "40") == nullptr);
        REQUIRE(col.findByAttr("value", "50", true /* create */) != nullptr);
        REQUIRE(col.at(3)->get("value") == "50");
    }

    SECTION("findById"){
        ModelCollection col;
        auto m1 = col.create();
        m1->set("id", "10");
        auto m2 = col.create();
        m2->set("id", "20");
        auto m3 = col.create();
        m3->set("id", "30");
        REQUIRE(col.findById("20") == m2);
        REQUIRE(col.findById("40") == nullptr);
        REQUIRE(col.findById("50", true /* create */) != nullptr);
        REQUIRE(col.at(3)->get("id") == "50");
    }
}
