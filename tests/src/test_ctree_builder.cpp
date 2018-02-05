#ifdef CICMS_CTREE

#include "catch.hpp"
#include <iostream>

#include "ciCMS/cfg/ctree/Builder.h"
#include "ciCMS/cfg/Configurator.h"
#include "ciCMS/Model.h"

using namespace cms;

// some custom test-classes
class Namer {
  public:
    string name;
};

class Ager {
  public:
    int age;
};

// our custom configurator for our test-classes
class Configer : public cms::cfg::Configurator {
  public:

    Configer() {
    }

    Configer(ModelCollection& mc) : cms::cfg::Configurator(mc) {
    }

    // using cms::cfg::ctree::cfgWithModel;
    void cfg(cms::cfg::ctree::Node& n, const std::map<string, string>& data){
      // TODO; take name from name attribute or otherwise default to last part
      // (splitting by period (.) of the id)
    }

    void cfg(Namer& obj, const std::map<string, string>& data){
      Model m;
      m.set(data);
      m.with("name", [&obj](const std::string& v){ obj.name = v; });
    }

    void cfg(Ager& obj, const std::map<string, string>& data){
      Model m;
      m.set(data);
      m.withInt("age", [&obj](const int& v){ obj.age = v; });
    }


    // overwrite Configurator's version, because that one only knows about
    // the cfg methods in the Configurator class
    template<typename T>
    void cfgWithModel(T& c, Model& model){
      this->apply(model, [this, &c](ModelBase& mod){
        this->cfg(c, mod.attributes());
      });
    }
};


TEST_CASE("cms::cfg::ctree::Builder", ""){
  SECTION("typical_usage"){
    // prepare your builder instance and populate with data
    cms::cfg::ctree::Builder<Configer> builder;
    builder.getModelCollection().loadJsonFromFile(ci::app::getAssetPath("test_ctree_builder.json"));

    // configure your builder by registering instantiators
    // (map classes that can be "build" to a type identifier)
    builder.addDefaultInstantiator<Namer>("Namer");
    builder.addDefaultInstantiator<Ager>("Ager");

    // build an item from the json data (identify by "id")
    auto namer = builder.build<Namer>("typical_usage.Namer");
    REQUIRE(namer->name == "root");
    // check that our object was built with two child objects
    REQUIRE(builder.select(namer)->getNode()->size() == 2);

    // fetch an "child" item from the just created hierarchy
    // of which names was the root element
    auto ager1 = builder.select(namer)->get<Ager>("Ager");
    REQUIRE(ager1->age == 10);
    REQUIRE(builder.select(ager1)->getNode()->size() == 1);

    // fetch child of child (grandchild of namer)
    REQUIRE(builder.select(ager1)->get<Ager>("SubAger")->age == 3);

    // fetch another child item
    auto ager2 = builder.select(namer)->get<Ager>("Ager2");
    REQUIRE(ager2->age == 20);
    REQUIRE(builder.select(ager2)->getNode()->size() == 0);

    // try (and fail) to fetch some non-existing child items
    REQUIRE(builder.select(namer)->get<Ager>("Ager3") == NULL);
    REQUIRE(builder.select(namer)->get<Ager>("foobar") == NULL);


    // build a new set of objects
    auto ager3 = builder.build<Ager>("typical_usage.Ager3");
    REQUIRE(ager3->age == 30);
    REQUIRE(builder.select(ager3)->getNode()->size() == 1);
    REQUIRE(builder.select(ager3)->get<Namer>("Namer")->name == "child_of_30");

    // attach new nodes to our original hierarchy
    builder.select(namer)->attach(ager3);

    // verify that that names now has a third child
    REQUIRE(builder.select(namer)->getNode()->size() == 3);
    REQUIRE(builder.select(namer)->get<Ager>("Ager3") != NULL);
    REQUIRE(builder.select(namer)->get<Ager>("Ager3") == ager3);

    // fetch sub item various level deep
    REQUIRE(builder.select(namer)->get<Namer>("Ager3.Namer")->name == "child_of_30");
    REQUIRE(builder.select(namer)->get<Namer>("Ager3.Namer") == builder.select(ager3)->get<Namer>("Namer"));
  }
}

#endif // CICMS_CTREE
