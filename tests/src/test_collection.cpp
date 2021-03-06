#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp> // for is_any_of
#include "cinder/Log.h"
#include "catch.hpp"
#include "ciCMS/ciCMS.h"

using namespace std;
using namespace cms;

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

    SECTION("loadJsonFromFile_missing_file"){
      ModelCollection col;
      auto p = ci::app::getAssetPath("foobar.json");
      REQUIRE(!col.loadJsonFromFile(p));
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
