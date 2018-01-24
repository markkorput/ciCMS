#define CATCH_CONFIG_MAIN

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp> // for is_any_of
#include "cinder/Log.h"
#include "catch.hpp"
#include "ciCMS/ciCMS.h"

using namespace std;
using namespace cms;

TEST_CASE("cms::Model", ""){

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

    SECTION("getBool"){
        Model model;
        REQUIRE(model.getBool("x") == false);
        REQUIRE(model.getBool("x", true) == true);
        model.set("x", "true");
        REQUIRE(model.getBool("x") == true);
        REQUIRE(model.getBool("x", false) == true);
        model.set("x", "Not-A-Bool");
        REQUIRE(model.getBool("x") == false);
        REQUIRE(model.getBool("x", true) == true);
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
        REQUIRE(m.getVec2("attr") == glm::vec2(0.0f, 0.0f));
        m.set("someColor", "255,0,0");
        REQUIRE(m.getVec2("someColor") == glm::vec2(0.0f, 0.0f));
        m.set("someColor", "23");
        REQUIRE(m.getVec2("someColor") == glm::vec2(23.0f,23.0f));
        m.set("someColor", "0,100");
        REQUIRE(m.getVec2("someColor") == glm::vec2(0.0f, 100.0f));
    }

    SECTION("getVec3"){
        Model m;
        REQUIRE(m.getVec3("attr") == glm::vec3(0.0f, 0.0f, 0.0f));
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

TEST_CASE("cms::Collection", ""){
    class FooKlass {
        public:
            string value;
    };

    SECTION("create"){
        Collection<FooKlass> col;

        int count=0;
        auto connection = col.addSignal.connect([&count](FooKlass& model){
            count++;
        });

        REQUIRE(col.size() == 0);
        REQUIRE(count == 0);

        // create first model
        auto instanceRef = col.create();
        REQUIRE(instanceRef.use_count() == 2);
        REQUIRE(col.size() == 1);
        REQUIRE(count == 1);

        connection.disconnect();
        instanceRef = col.create();

        REQUIRE(col.size() == 2);
        REQUIRE(count == 1);
    }

    SECTION("add"){
        Collection<FooKlass> col;
        auto m = make_shared<FooKlass>();
        col.add(m);
        REQUIRE(col.size() == 1);
        REQUIRE(col.at(0) == m);
    }

    SECTION("find and remove"){
        Collection<FooKlass> col;
        auto itemRef = col.create();
        REQUIRE(col.size() == 1);

        // find
        REQUIRE(col.at(0) == itemRef);
        REQUIRE(itemRef.use_count() == 2);

        // remove by reference
        col.remove(itemRef);
        REQUIRE(itemRef.use_count() == 1); // local reference is last reference
        REQUIRE(col.size() == 0);
    }

    SECTION("remove with invalid index"){
        Collection<FooKlass> col;
        col.create();
        REQUIRE(col.size() == 1);
        REQUIRE(col.removeByIndex(1) == nullptr);
        REQUIRE(col.removeByIndex(100) == nullptr);
        REQUIRE(col.size() == 1);
    }

    SECTION("remove by index"){
        Collection<FooKlass> col;
        auto item1Ref = col.create();
        auto item2Ref = col.create();

        REQUIRE(col.size() == 2);
        REQUIRE(item2Ref.use_count() == 2);
        auto removeResultRef = col.removeByIndex(1);
        REQUIRE(removeResultRef == item2Ref);
        removeResultRef = nullptr;
        REQUIRE(col.size() == 1);
        REQUIRE(item2Ref.use_count() == 1); // last reference
    }

    SECTION("remove by pointer"){
        CI_LOG_W("TODO");
    }

    SECTION("destroy"){
        Collection<FooKlass> col;
        col.create(); col.create(); col.create();
        REQUIRE(col.size() == 3);
        col.destroy();
        REQUIRE(col.size() == 0);
    }

    SECTION("each"){
        Collection<FooKlass> col;
        col.create()->value = "#1";
        col.create()->value = "#2";
        col.create()->value = "#3";

        REQUIRE(col.size() == 3);

        string merged = "";

        col.each([&merged](shared_ptr<FooKlass> itemRef){
            merged += itemRef->value;
        });

        REQUIRE(merged == "#1#2#3");
    }

    SECTION("add while iterating"){
        CI_LOG_W("TODO");
    }

    SECTION("remove while iterating"){
        CI_LOG_W("TODO");
    }

    SECTION("previous"){
        CI_LOG_W("TODO");
    }

    SECTION("next"){
        CI_LOG_W("TODO");
    }

    SECTION("random"){
        CI_LOG_W("TODO");
    }

    SECTION("limit"){
        auto colRef = make_shared<Collection<FooKlass>>();

        // create five instance
        colRef->create();
        colRef->create();
        colRef->create();
        colRef->create();
        colRef->create();

        string removed = "";

        auto connection = colRef->removeSignal.connect([&removed](FooKlass& model){
            removed += "#"+std::to_string((long)&model);
        });

        string expected = "#"+std::to_string((long)colRef->at(4).get())+"#"+std::to_string((long)colRef->at(3).get());
        colRef->limit(3);
        REQUIRE(colRef->size() == 3); // two models removed
        REQUIRE(removed == expected); // remove callback invoked; last two models removed

        FooKlass* cid = colRef->at(2).get();
        colRef->create();
        REQUIRE(colRef->size() == 3); // nothing added (fifo is false by default)
        REQUIRE(colRef->at(2).get() == cid);

        colRef->setFifo(true);
        auto newModelRef = colRef->create();
        REQUIRE(colRef->size() == 3); // nothing added (fifo is false by default)
        REQUIRE(colRef->at(2).get() == newModelRef.get());

        connection.disconnect();

        CI_LOG_W("TODO: also feature one-time (non-active) limit");
    }

    SECTION("sync once"){
        auto colRefA = make_shared<Collection<FooKlass>>();
        auto colRefB = make_shared<Collection<FooKlass>>();

        // initialize B with one model
        colRefB->create();
        REQUIRE(colRefB->size() == 1);
        REQUIRE(colRefA->size() == 0);

        // sync operation transfers model to A
        colRefA->sync(colRefB, false /* sync once, don't monitor for changes */);
        REQUIRE(colRefB->size() == 1);
        REQUIRE(colRefA->size() == 1);
        REQUIRE(colRefA->at(0).get() == colRefB->at(0).get());

        // sync is not active; A won't receive new models from B
        colRefB->create();
        REQUIRE(colRefB->size() == 2);
        REQUIRE(colRefA->size() == 1);
        REQUIRE(colRefA->at(0).get() == colRefB->at(0).get());

        // sync is not active; A won't drop models along with B
        colRefB->removeByIndex(0);
        REQUIRE(colRefB->size() == 1);
        REQUIRE(colRefA->size() == 1);
        REQUIRE(colRefA->at(0).get() != colRefB->at(0).get());
    }

    SECTION("sync active"){
        auto colRefA = make_shared<Collection<FooKlass>>();
        auto colRefB = make_shared<Collection<FooKlass>>();

        // initialize B with one model
        colRefB->create();
        REQUIRE(colRefB->size() == 1);
        REQUIRE(colRefA->size() == 0);

        // sync operation transfers model to A
        colRefA->sync(colRefB);
        REQUIRE(colRefB->size() == 1);
        REQUIRE(colRefA->size() == 1);
        REQUIRE(colRefA->at(0).get() == colRefB->at(0).get());

        // active sync; A receives new models from B
        colRefB->create();
        REQUIRE(colRefB->size() == 2);
        REQUIRE(colRefA->size() == 2);
        REQUIRE(colRefA->at(1).get() == colRefB->at(1).get());

        // second sync source
        auto colRefC = make_shared<Collection<FooKlass>>();
        colRefC->create();
        colRefC->create();
        REQUIRE(colRefC->size() == 2);
        colRefA->sync(colRefC);
        REQUIRE(colRefA->size() == 4);

        colRefC->create();
        REQUIRE(colRefA->size() == 5);

        // active sync; A drops models along with B
        colRefB->removeByIndex(0);
        colRefB->removeByIndex(0);
        REQUIRE(colRefB->size() == 0);
        REQUIRE(colRefA->size() == 3);

        colRefC->removeByIndex(0);
        colRefC->removeByIndex(0);
        REQUIRE(colRefC->size() == 1);
        REQUIRE(colRefA->size() == 1);
    }

    SECTION("sync active with non shared_ptr source collection"){
        auto colRefA = make_shared<Collection<FooKlass>>();
        auto colRefB = make_shared<Collection<FooKlass>>();

        // initialize B with one model
        colRefB->create();
        REQUIRE(colRefB->size() == 1);
        REQUIRE(colRefA->size() == 0);

        // sync operation transfers model to A
        colRefA->sync(*colRefB.get());
        REQUIRE(colRefB->size() == 1);
        REQUIRE(colRefA->size() == 1);
        REQUIRE(colRefA->at(0).get() == colRefB->at(0).get());

        // active sync; A receives new models from B
        colRefB->create();
        REQUIRE(colRefB->size() == 2);
        REQUIRE(colRefA->size() == 2);
        REQUIRE(colRefA->at(1).get() == colRefB->at(1).get());

        // second sync source
        auto colRefC = make_shared<Collection<FooKlass>>();
        colRefC->create();
        colRefC->create();
        REQUIRE(colRefC->size() == 2);
        colRefA->sync(*colRefC.get());
        REQUIRE(colRefA->size() == 4);

        colRefC->create();
        REQUIRE(colRefA->size() == 5);

        // active sync; A drops models along with B
        colRefB->removeByIndex(0);
        colRefB->removeByIndex(0);
        REQUIRE(colRefB->size() == 0);
        REQUIRE(colRefA->size() == 3);

        colRefC->removeByIndex(0);
        colRefC->removeByIndex(0);
        REQUIRE(colRefC->size() == 1);
        REQUIRE(colRefA->size() == 1);
    }

    SECTION("filter actively using custom lambda"){
        Collection<FooKlass> col;
        col.create();
        col.create();
        col.create();
        REQUIRE(col.size() == 3);

        int filterCounter=0;

        col.filter([&filterCounter](FooKlass& instance) -> bool {
            // accept every other instance
            bool accept = ((filterCounter & 1) == 0);
            filterCounter++;
            return accept;
        });

        REQUIRE(filterCounter == 3);
        REQUIRE(col.size() == 2);

        col.create();
        REQUIRE(col.size() == 2);
        col.create();
        REQUIRE(col.size() == 3);
        col.create();
        REQUIRE(col.size() == 3);
        col.create();
        REQUIRE(col.size() == 4);
        REQUIRE(filterCounter == 7);
    }

    SECTION("reject actively using custom lambda"){
        Collection<FooKlass> col;
        col.create();
        col.create();
        col.create();
        REQUIRE(col.size() == 3);

        int filterCounter=0;

        col.reject([&filterCounter](FooKlass& instance) -> bool {
            // accept every other instance
            bool accept = ((filterCounter & 1) == 0);
            filterCounter++;
            return accept;
        });

        REQUIRE(filterCounter == 3);
        REQUIRE(col.size() == 1);

        col.create();
        REQUIRE(col.size() == 2);
        col.create();
        REQUIRE(col.size() == 2);
        col.create();
        REQUIRE(col.size() == 3);
        col.create();
        REQUIRE(col.size() == 3);
        REQUIRE(filterCounter == 7);
    }

    SECTION("combine filter sync and limit"){
        CI_LOG_W("TODO");
    }

    SECTION("transform"){
        class TransformedClass {
        public:
            string pointerString;
        };

        Collection<TransformedClass> targetCol;
        Collection<FooKlass> sourceCol;

        sourceCol.create();
        sourceCol.create();
        REQUIRE(sourceCol.size() == 2);
        REQUIRE(targetCol.size() == 0);

        targetCol.template transform<FooKlass>(sourceCol, [](FooKlass& source) -> shared_ptr<TransformedClass>{
            auto instanceRef = make_shared<TransformedClass>();
            instanceRef->pointerString = std::to_string((long)&source);
            return instanceRef;
        });

        REQUIRE(targetCol.size() == 2);
        REQUIRE(targetCol.at(0)->pointerString == std::to_string((long)sourceCol.at(0).get()));
        REQUIRE(targetCol.at(1)->pointerString == std::to_string((long)sourceCol.at(1).get()));

        sourceCol.create();
        REQUIRE(targetCol.size() == 3);
        REQUIRE(targetCol.at(0)->pointerString == std::to_string((long)sourceCol.at(0).get()));
        REQUIRE(targetCol.at(1)->pointerString == std::to_string((long)sourceCol.at(1).get()));
        REQUIRE(targetCol.at(2)->pointerString == std::to_string((long)sourceCol.at(2).get()));

        sourceCol.removeByIndex(1);
        REQUIRE(targetCol.size() == 2);
        REQUIRE(targetCol.at(0)->pointerString == std::to_string((long)sourceCol.at(0).get()));
        REQUIRE(targetCol.at(1)->pointerString == std::to_string((long)sourceCol.at(1).get()));

        targetCol.stopTransform(sourceCol);
        sourceCol.removeByIndex(0);
        REQUIRE(sourceCol.size() == 1);
        REQUIRE(targetCol.size() == 2);
        sourceCol.create();
        sourceCol.create();
        REQUIRE(sourceCol.size() == 3);
        REQUIRE(targetCol.size() == 2);
    }

    SECTION("loadJsonFromFile"){
        ModelCollection col;
        auto p = ci::app::getAssetPath("collection.json");
        REQUIRE(col.loadJsonFromFile(p));
        REQUIRE(col.size() == 3);
        REQUIRE(col.at(0)->get("id") == "1");
        REQUIRE(col.at(1)->get("id") == "2");
        REQUIRE(col.at(2)->get("id") == "3");
        REQUIRE(col.at(0)->get("name") == "John Doe");
        REQUIRE(col.at(1)->get("name") == "Jane Doe");
        REQUIRE(col.at(2)->get("name") == "Billy Doe");
        REQUIRE(col.at(0)->get("age") == "31");
        REQUIRE(col.at(1)->get("age") == "33");
        REQUIRE(col.at(2)->get("age") == "13");
        REQUIRE(col.loadJsonFromFile(p));
        // by default matches on ID
        REQUIRE(col.size() == 3);
        REQUIRE(col.at(0)->get("id") == "1");
        REQUIRE(col.at(1)->get("id") == "2");
        REQUIRE(col.at(2)->get("id") == "3");
        REQUIRE(col.at(0)->get("name") == "John Doe");
        REQUIRE(col.at(1)->get("name") == "Jane Doe");
        REQUIRE(col.at(2)->get("name") == "Billy Doe");
        REQUIRE(col.at(0)->get("age") == "31");
        REQUIRE(col.at(1)->get("age") == "33");
        REQUIRE(col.at(2)->get("age") == "13");
    }

    SECTION("toJsonString"){
        ModelCollection col;
        col.create()->set("value", "v1");
        col.create()->set("value", "v2");
        col.create()->set("falue", "f3");
        // col.writeJson(ci::app::getAssetPath("collectionWriteTarget.json"));

        auto path = ci::app::getAssetPath("collection2.json");
        auto dataSourceRef = ci::loadFile(path);

        string fileContentString( static_cast<const char*>( dataSourceRef->getBuffer()->getData() ));
        if(fileContentString.length() > dataSourceRef->getBuffer()->getSize())
            fileContentString.resize( dataSourceRef->getBuffer()->getSize() );

        REQUIRE(col.toJsonString() == fileContentString);
    }
}

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
        REQUIRE(man.size() == 2);

        REQUIRE(man["col1"]->size() == 3);
        REQUIRE(man["col1"]->findById("one") != nullptr);
        REQUIRE(man["col1"]->findById("two") != nullptr);
        REQUIRE(man["col1"]->findById("three") != nullptr);
        REQUIRE(man["col1"]->findById("four") == nullptr);

        REQUIRE(man["col2"]->size() == 3);
        REQUIRE(man["col2"]->at(0)->get("name") == "john");
        REQUIRE(man["col2"]->at(1)->get("name") == "bob");
        REQUIRE(man["col2"]->at(2)->get("name") == "henk");
    }
}

TEST_CASE("cms::QueryCollection", ""){
    class TestItem {
        public:
            string name;
            float value;
            void set(const string& newName, float val){
                name = newName;
                value = val;
            }
    };

    class TestQuery {
        private:
            string name;
            bool bValueFilterEnabled, bNameFilterEnabled;
            float minValue;

        public:
            TestQuery() : bValueFilterEnabled(false), bNameFilterEnabled(false){}

            bool isValueFilterEnabled() const { return bValueFilterEnabled; }

            float getMinValue() const { return minValue; }

            void setMinValue(float min){
                minValue = min;
                bValueFilterEnabled = true;
            }

            void setNameFilter(const string& newName){
                name = newName;
                bNameFilterEnabled = true;
            }

            const string& getNameFilter() const { return name; }

            bool isNameFilterEnabled() const { return bNameFilterEnabled; }
    };

    class TestQueryCollection : public QueryCollection<TestItem, TestQuery> {
        public:
            // database is our in-memory dummy backend
            Collection<TestItem> database;

            TestQueryCollection(){
                // populate with some content
                database.create()->set("no.1", 10);
                database.create()->set("no.2", 20);
                database.create()->set("no.3", 30);
            }

            ExecutionRef nameQuery(string name){
                auto queryRef = make_shared<TestQuery>();
                queryRef->setNameFilter(name);
                return query(queryRef);
            }

        private:

            void execute(ExecutionRef execRef){
                auto queryRef = execRef->getQuery();

                // "query" our "database"
                database.each([this, queryRef, execRef](shared_ptr<TestItem> itemRef){
                    // name condition
                    if(queryRef->isNameFilterEnabled() && itemRef->name != queryRef->getNameFilter())
                        return;

                    // value condition
                    if(queryRef->isValueFilterEnabled() && itemRef->value < queryRef->getMinValue())
                        return;

                    // all conditions met, add this item to our collection
                    execRef->add(itemRef);
                });

                execRef->finalize(true);
            }
    };

    SECTION(".query()"){
        TestQueryCollection col;
        shared_ptr<TestQueryCollection::Execution> executionRef;

        vector<TestQuery*> doneQueries;

        col.queryDoneSignal.connect([&doneQueries](shared_ptr<TestQuery> queryRef){
            doneQueries.push_back(queryRef.get());
        });

        {   // start query with call to .query
            auto queryRef = make_shared<TestQuery>();
            queryRef->setNameFilter("no.3");

            executionRef = col.query(queryRef);
            REQUIRE(executionRef != nullptr);
            REQUIRE(executionRef->getQuery() == queryRef);
            REQUIRE(executionRef->isDone()); // this one happens to be extremely fast and, well, non-async
            REQUIRE(executionRef->isSuccess());
            REQUIRE(!executionRef->isFailure()); // this one happens to be extremely fast and, well, non-async
            // execution specific results
            REQUIRE(executionRef->result.size() == 1);
            REQUIRE(executionRef->result.at(0)->name == "no.3");
            REQUIRE(executionRef->result.at(0) == col.database.at(2));

        } // end of queryRef instance scope

        // check if the query was correctly performed;
        // it should've gotten us just the database item "no.3"
        REQUIRE(col.size() == 1);
        REQUIRE(col.at(0)->name == "no.3");
        REQUIRE(col.at(0) == col.database.at(2));
        REQUIRE(doneQueries.size() == 1);
        REQUIRE(doneQueries[0] == executionRef->getQuery().get());

        {   // do another query; with a value-filter this time
            auto queryRef = make_shared<TestQuery>();
            queryRef->setMinValue(20);
            executionRef = col.query(queryRef);

            REQUIRE(executionRef->isDone()); // another fast one
            REQUIRE(executionRef->isSuccess());
            // execution specific results
            REQUIRE(executionRef->result.size() == 2);
            REQUIRE(executionRef->result.at(0)->name == "no.2");
            REQUIRE(executionRef->result.at(0) == col.database.at(1));
            REQUIRE(executionRef->result.at(1)->name == "no.3");
            REQUIRE(executionRef->result.at(1) == col.database.at(2));
        }

        // the results of this query are added to the collection;
        // our collection by default performs model based duplicate filtering
        REQUIRE(col.size() == 2);
        REQUIRE(col.at(0) == col.database.at(2));
        REQUIRE(col.at(1) == col.database.at(1));
        // REQUIRE(col.at(2) == col.database.at(2));
        REQUIRE(doneQueries.size() == 2);
        REQUIRE(doneQueries[1] == executionRef->getQuery().get());
    }

    SECTION(".query() with pass-by-reference argument"){
        // QueryType = int
        QueryCollection<TestItem, int> col;

        int result=0;

        // execRef->getQuery() returns a shared_ptr<int> == 5
        col.query(5)->whenDone([&result](QueryCollection<TestItem, int>::Execution& exec){
            result = *exec.getQuery();
        });

        REQUIRE(result == 5);
    }

    SECTION(".executeFn()"){
        TestQueryCollection col;

        // register custom execution logic ("overwrites" any existing execution method)
        col.executeFn([&col](TestQueryCollection::ExecutionRef execRef){
            // simply create a new item in the results model and give it the name of the query's name filter
            execRef->result.create()->name = execRef->getQuery()->getNameFilter();
        });

        REQUIRE(col.size() == 0);
        auto execRef = col.nameQuery("Some Name");
        REQUIRE(execRef->result.size() == 1);
        REQUIRE(execRef->result.at(0)->name == "Some Name");
        REQUIRE(col.size() == 1);
        REQUIRE(col.at(0) == execRef->result.at(0));
    }

    SECTION(".cacheCheckFn()"){
        TestQueryCollection col;

        // register custom execution logic ("overwrites" any existing execution method)
        col.executeFn([&col](TestQueryCollection::ExecutionRef execRef){
            // simply create a new item in the results model and give it the name of the query's name filter
            execRef->result.create()->name = execRef->getQuery()->getNameFilter();
        });

        REQUIRE(col.size() == 0);
        auto execRef = col.nameQuery("Some Name");
        REQUIRE(execRef->result.size() == 1);
        REQUIRE(execRef->result.at(0)->name == "Some Name");
        REQUIRE(col.size() == 1);
        REQUIRE(col.at(0) == execRef->result.at(0));
        // let's do that again
        execRef = col.nameQuery("Some Name");
        REQUIRE(execRef->result.size() == 1);
        REQUIRE(execRef->result.at(0)->name == "Some Name");
        REQUIRE(col.size() == 2);
        REQUIRE(col.at(1) == execRef->result.at(0));

        // now let's register a cache checkers which first checks which of the existing items
        // matches the query. If any, it aborts the query by finalizing it
        col.cacheCheckFn([](Collection<TestItem>& col, TestQueryCollection::ExecutionRef execRef){
            col.each([&execRef](shared_ptr<TestItem> itemRef){
                if(itemRef->name == execRef->getQuery()->getNameFilter())
                    execRef->result.add(itemRef);
            });

            // finalize the execRef so the actual "remote" query doesn't happen
            if(!execRef->result.isEmpty())
                execRef->abort();
        });

        // now let's do that a third time, this time no new item should appear
        execRef = col.nameQuery("Some Name");
        REQUIRE(execRef->isDone()); // this one happens to be extremely fast and, well, non-async
        REQUIRE(execRef->isAborted());
        REQUIRE(!execRef->isExecuted());
        REQUIRE(!execRef->isSuccess());
        REQUIRE(!execRef->isFailure());
        REQUIRE(execRef->result.size() == 2); // matched with two (both) of the cached items
        REQUIRE(col.size() == 2);
        REQUIRE(col.at(0) == execRef->result.at(0));
        REQUIRE(col.at(1) == execRef->result.at(1));
    }

    SECTION(".whenDone() .onSuccess .onFailure .whenAborted .whenExecuted"){
        TestQueryCollection col;

        string result="";

        auto execRef = col.nameQuery("no.1")
            ->whenDone([&result](TestQueryCollection::Execution& exec){
                result+="/done:"+exec.getQuery()->getNameFilter();
            })
            ->onSuccess([&result](TestQueryCollection::Execution& exec){
                result+="/success:"+exec.getQuery()->getNameFilter();
            })
            ->onFailure([&result](TestQueryCollection::Execution& exec){
                result+="/failure:"+exec.getQuery()->getNameFilter();
            })
            ->whenAborted([&result](TestQueryCollection::Execution& exec){
                result+="/aborted:"+exec.getQuery()->getNameFilter();
            })
            ->whenExecuted([&result](TestQueryCollection::Execution& exec){
                result+="/executed:"+exec.getQuery()->getNameFilter();
            });

        REQUIRE(result == "/done:no.1/success:no.1/executed:no.1");

        // register aborter
        col.cacheCheckFn([](Collection<TestItem>& col, TestQueryCollection::ExecutionRef execRef){
            execRef->abort();
        });

        // run again
        result = "";
        execRef = col.nameQuery("foobar")
            ->whenDone([&result](TestQueryCollection::Execution& exec){
                result+="/done:"+exec.getQuery()->getNameFilter();
            })
            ->onSuccess([&result](TestQueryCollection::Execution& exec){
                result+="/success:"+exec.getQuery()->getNameFilter();
            })
            ->onFailure([&result](TestQueryCollection::Execution& exec){
                result+="/failure:"+exec.getQuery()->getNameFilter();
            })
            ->whenAborted([&result](TestQueryCollection::Execution& exec){
                result+="/aborted:"+exec.getQuery()->getNameFilter();
            })
            ->whenExecuted([&result](TestQueryCollection::Execution& exec){
                result+="/executed:"+exec.getQuery()->getNameFilter();
            });

        REQUIRE(result == "/done:foobar/aborted:foobar");
    }
}
