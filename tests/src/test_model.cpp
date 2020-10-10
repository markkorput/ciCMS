#include "catch.hpp"
#include "ciCMS/cfg/utils.h"
#include "ciCMS/Model.h"

using namespace cms;

TEST_CASE("cms::Model", ""){
    SECTION("each"){
        Model model;
        std::vector<string> result;
        model.set("name", "John");
        model.set("age", "32");


        REQUIRE(cfg::join(result, ",") == "");
        model.each([&result](const string& attr, const string& val){
            result.push_back(attr+"="+val);
        });

        REQUIRE(cfg::join(result, ",") == "age=32,name=John");
    }

    SECTION("each lock; modifying operations during i"){
        Model model;

        model.set("name", "John");
        model.set("age", "32");
        REQUIRE(model.size() == 2);
        REQUIRE(model.get("name_copy") == "");
        REQUIRE(model.get("age_copy") == "");

        std::vector<string> result;
        REQUIRE(cfg::join(result, ",") == "");

        model.each([&model, &result](const string& attr, const string& val){
            // add copy attribute
            model.set(attr+"_copy", val);
            model.set(attr, val+"_updated");
            // get number of attributes (the above new attribute should not be added yet)
            result.push_back(attr+"="+model.get(attr)+"(size:"+std::to_string(model.size())+")");
        });

        // during the iterations, the model didn't change
        REQUIRE(cfg::join(result, ",") == "age=32(size:2),name=John(size:2)");
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
        cfg::split(strs, "attrA,attrC", ',');

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

    SECTION("getInt"){
        Model model;
        REQUIRE(model.getInt("x") == 0);
        REQUIRE(model.getInt("x", 101) == 101);
        model.set("x", "5");
        REQUIRE(model.getInt("x") == 5);
        REQUIRE(model.getInt("x", 101) == 5);
        model.set("x", "Not-A-Number");
        REQUIRE(model.getInt("x") == 0);
        REQUIRE(model.getInt("x", 101) == 101);
    }

    SECTION("getFloat"){
        Model model;
        REQUIRE(model.getFloat("x") == 0.f);
        REQUIRE(model.getFloat("x", 101.0f) == 101.f);
        model.set("x", "5.5");
        REQUIRE(model.getFloat("x") == 5.5f);
        REQUIRE(model.getFloat("x", 101.f) == 5.5f);
        model.set("x", "Not-A-Number");
        REQUIRE(model.getFloat("x") == 0.f);
        REQUIRE(model.getFloat("x", 101.f) == 101.f);
    }

    SECTION("setBool getBool"){
        Model model;
        // unset attribute
        REQUIRE(model.getBool("x") == false);
        REQUIRE(model.getBool("x", true) == true);
        
        model.set("x", "true");
        REQUIRE(model.getBool("x") == true);
        REQUIRE(model.getBool("x", false) == true);
        
        model.set("x", "false");
        REQUIRE(model.getBool("x") == false);
        REQUIRE(model.getBool("x", true) == false);

        model.setBool("x", true);
        REQUIRE(model.getBool("x") == true);
        REQUIRE(model.getBool("x", false) == true);

        model.set("x", "Not-A-Bool");
        REQUIRE(model.getBool("x") == false);
        REQUIRE(model.getBool("x", true) == false);
    }

    SECTION("with(bool)"){
        Model model;
        bool target = true;

        model.with("foo", [&target](const bool& val){ target = val; });
        REQUIRE(target == true);

        model.set("foo", "false");
        REQUIRE(target == true);
        model.with("foo", [&target](const bool& val){ target = val; });
        REQUIRE(target == false);

        model.set("foo", "true");
        REQUIRE(target == false);
        model.with("foo", [&target](const bool& val){ target = val; });
        REQUIRE(target == true);
    }

    SECTION("getVec2"){
        Model m;
        REQUIRE(m.getVec2("attr") == glm::vec2(0.0f));
        m.set("someColor", "255,0,0");
        REQUIRE(m.getVec2("someColor") == glm::vec2(0.0f, 0.0f));
        m.set("someColor", "23");
        REQUIRE(m.getVec2("someColor") == glm::vec2(23.0f,23.0f));
        m.set("someColor", "0,100");
        REQUIRE(m.getVec2("someColor") == glm::vec2(0.0f, 100.0f));
    }

    SECTION("getVec3"){
        Model m;
        REQUIRE(m.getVec3("attr") == glm::vec3(0.0f));
        m.set("someColor", "255,0,0");
        REQUIRE(m.getVec3("someColor") == glm::vec3(255.0f, 0.0f, 0.0f));
        m.set("someColor", "23");
        REQUIRE(m.getVec3("someColor") == glm::vec3(23.0f,23.0f,23.0f));
        m.set("someColor", "0,100");
        REQUIRE(m.getVec3("someColor") == glm::vec3(0.0f, 0.0f, 0.0f));
        m.set("someColor", "0,100");
        REQUIRE(m.getVec3("someColor", glm::vec3(0.0f, 0.0f, 1.0f)) == glm::vec3(0.0f, 0.0f, 1.0f));
    }

    SECTION("with(vec3)"){
        Model model;
        glm::vec3 target = glm::vec3(0.0f);

        model.with("foo", [&target](const glm::vec3& val){ target = val; });
        REQUIRE(target == glm::vec3(0.0f));

        model.set("foo", "10,20,30");
        model.with("foo", [&target](const glm::vec3& val){ target = val; });
        REQUIRE(target == glm::vec3(10.0f,20.0f,30.0f));
    }

    SECTION("getColor"){
        Model m;
        REQUIRE(m.getColor("someColor") == ci::ColorAf(1.0f, 1.0f, 1.0f, 1.0f));
        m.set("someColor", "255,0,0");
        REQUIRE(m.getColor("someColor") == ci::ColorAf(1.0f, 0.0f, 0.0f, 1.0f));
        m.set("someColor", "0,100,0,100");
        REQUIRE(m.getColor("someColor") == ci::ColorAf(0.0f, 100.0f/255.0f, 0.0f, 100.0f/255.0f));
        m.set("someColor", "0,100");
        REQUIRE(m.getColor("someColor") == ci::ColorAf(1.0f, 1.0f, 1.0f, 1.0f));
        m.set("someColor", "0,100");
        REQUIRE(m.getColor("someColor", ci::ColorAf(0.0f, 0.0f, 1.0f, 1.0f)) == ci::ColorAf(0.0f, 0.0f, 1.0f, 1.0f));
        m.set("someColor", "10");
        REQUIRE(m.getColor("someColor") == ci::ColorAf(10.0f/255.0f, 10.0f/255.0f, 10.0f/255.0f, 1.0f));
    }

    SECTION(".transform"){
        // setup
        Model m;
        m.set("name", "john");
        string result;

        // transform
        m.transform([&result](ModelBase& model){
            result = model.get("name");
        }, &result);

        // called with current value
        REQUIRE(result == "john");

        // registers change listener
        m.set("name", "bob");
        REQUIRE(result == "bob");

        // stop transform
        m.stopTransform(&result
        );

        // unregisters change listener
        m.set("name", "henk");
        REQUIRE(result == "bob");
    }
}
