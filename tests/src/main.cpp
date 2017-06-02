#define CATCH_CONFIG_MAIN

#include "cinder/Log.h"
#include <boost/algorithm/string/join.hpp>

#include "catch.hpp"
#include "ciCMS/ciCMS.h"

using namespace std;
using namespace cms;

TEST_CASE("cms::Model", "")
{
    SECTION("get and set"){
    }

    SECTION("transform"){
    }

    SECTION("transformAttribute"){
    }

    SECTION("each"){
        Model model;
        std::vector<string> result;
        model.set("name", "John");
        model.set("age", "32");

        REQUIRE(boost::algorithm::join(result, ",") == "");
        model.each([&result](const string& attr, const string& val){
            result.push_back(attr+"="+val);
        });

        REQUIRE(boost::algorithm::join(result, ",") == "age=32,name=John");
    }

    SECTION("each lock; modifying operations during i"){
        Model model;

        model.set("name", "John");
        model.set("age", "32");
        REQUIRE(model.size() == 2);
        REQUIRE(model.get("name_copy") == "");
        REQUIRE(model.get("age_copy") == "");

        std::vector<string> result;
        REQUIRE(boost::algorithm::join(result, ",") == "");

        model.each([&model, &result](const string& attr, const string& val){
            // add copy attribute
            model.set(attr+"_copy", val);
            model.set(attr, val+"_updated");
            // get number of attributes (the above new attribute should not be added yet)
            result.push_back(attr+"="+model.get(attr)+"(size:"+std::to_string(model.size())+")");
        });

        // during the iterations, the model didn't change
        REQUIRE(boost::algorithm::join(result, ",") == "age=32(size:2),name=John(size:2)");
        // immediately after the iterations finished, all changes were effected
        REQUIRE(model.size() == 4);
        REQUIRE(model.get("name") == "John_updated");
        REQUIRE(model.get("age") == "32_updated");
        REQUIRE(model.get("name_copy") == "John");
        REQUIRE(model.get("age_copy") == "32");

    }
}

TEST_CASE("cms::Collection", ""){
    SECTION("each"){
        CI_LOG_W("TODO");
    }
}
