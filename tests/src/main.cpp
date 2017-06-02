#define CATCH_CONFIG_MAIN

#include "cinder/Log.h"
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp> // for is_any_of
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

    SECTION("actively transform specific attribute"){
        Model model;
        model.set("age", "10");

        float result;

        auto signalConnection = model.transformAttribute("age", [&result](const string& value){
            result = std::stof(value) * 100.0f;
        });

        // .transform already processed the existing value
        REQUIRE(result == 1000.0f);
        // change value
        model.set("age", "25");
        // .transform registers change listener and automatically processes updated value
        REQUIRE(result == 2500.0f);

        // stop transformer
        signalConnection.disable();
        // change value
        model.set("age", "1");
        // didn't transform
        REQUIRE(result == 2500.0f);

        // start transformer again
        signalConnection.enable();
        // change value
        model.set("age", "2");
        // DID transform
        REQUIRE(result == 200.0f);
    }

    SECTION("multiple attributes with same transformer"){
        // create a model with three "float" attributes
        Model model;
        model.set("attrA", "1");
        model.set("attrB", "10");
        model.set("attrC", "100");

        int result=0;

        vector<string> strs;
        boost::split(strs,"attrA,attrC",boost::is_any_of(","));

        // register transformer that counts two-out-of-three attributes
        auto signalConnection = model.transformAttributes(strs, [&result](const string& value){
            result += std::stoi(value);
        }, &result);

        // after registering the transformer the current values of the two attributes are already transformed
        REQUIRE(result == 101);

        // change some values
        model.set("attrB", "20"); // doesn't get tranformer
        model.set("attrC", "5"); // new value does get transformed
        REQUIRE(result == 106);

        model.stopAttributeTransform(&result);
        model.set("attrC", "4"); // new value does get transformed
        REQUIRE(result == 106);
    }
}

TEST_CASE("cms::Collection", ""){
    SECTION("each"){
        CI_LOG_W("TODO");
    }
}
