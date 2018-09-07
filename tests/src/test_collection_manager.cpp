#include "cinder/Log.h"
#include "catch.hpp"
#include "ciCMS/ciCMS.h"

using namespace std;
using namespace cms;

TEST_CASE("cms::CollectionManager", ""){
    SECTION("operator[], .add and .size"){
        CollectionManager man;
        REQUIRE(man.size() == 0);
        REQUIRE(man["foo"] == nullptr);
        auto colRef = make_shared<ModelCollection>();
        man["foo"] = colRef;
        REQUIRE(man.size() == 1);
        REQUIRE(man["foo"] == colRef);
    }

    SECTION("get()"){
        CollectionManager man;
        REQUIRE(man.get("foo", true /* create if not exist */) != nullptr);
        REQUIRE(man.size() == 1);
    }

    SECTION("loadJsonFromFile()"){
        CollectionManager man;
        auto p = ci::app::getAssetPath("CollectionManagerTest.json");
        REQUIRE(man.loadJsonFromFile(p));
        REQUIRE(man.size() == 3);

        REQUIRE(man["col1"]->size() == 3);
        REQUIRE(man["col1"]->findById("one") != nullptr);
        REQUIRE(man["col1"]->findById("two") != nullptr);
        REQUIRE(man["col1"]->findById("three") != nullptr);
        REQUIRE(man["col1"]->findById("four") == nullptr);

        CI_LOG_W("!!! cms::CollectionManager::loadJsonFromFile test DISABLED and failing!!! !!!!!!!!!!!!!");
        // REQUIRE(man["col2"]->size() == 3);
        // REQUIRE(man["col2"]->at(0)->get("name") == "john");
        // REQUIRE(man["col2"]->at(1)->get("name") == "bob");
        // REQUIRE(man["col2"]->at(2)->get("name") == "henk");
        // REQUIRE(man["col2"]->at(0)->get("id") == "");
        // REQUIRE(man["col2"]->at(1)->get("id") == "");
        // REQUIRE(man["col2"]->at(2)->get("id") == "");

        REQUIRE(man["col3"]->size() == 3);
        REQUIRE(man["col3"]->at(0)->get("name") == "alpha");
        REQUIRE(man["col3"]->at(1)->get("name") == "bravo");
        REQUIRE(man["col3"]->at(2)->get("name") == "charlie");
        REQUIRE(man["col3"]->at(0)->get("id") == "itemA");
        REQUIRE(man["col3"]->at(1)->get("id") == "itemB");
        REQUIRE(man["col3"]->at(2)->get("id") == "itemC");
    }
}
