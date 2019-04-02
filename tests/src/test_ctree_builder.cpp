#ifdef CICMS_CTREE

#include "catch.hpp"
#include <iostream>

#include "ciCMS/cfg/ctree/Builder.h"
#include "ciCMS/cfg/Configurator.h"
#include "ciCMS/Model.h"

using namespace cms;
#define STRINGIFY(...) #__VA_ARGS__

// some custom test-classes
class Namer {
  public:
    string name;
    string realtimeName() { return realtimeNameFunc ? realtimeNameFunc() : name; }
    std::function<std::string()> realtimeNameFunc = nullptr;

    ctree::Signal<void(const std::string&)> nameSignal;
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

TEST_CASE("cms::cfg::ctree::Builder With Configurator", ""){
  SECTION("typical_usage"){
    // prepare your builder instance and populate with data
    cms::cfg::ctree::Builder<Configer> builder;
    // builder.getModelCollection().loadJsonFromFile(ci::app::getAssetPath("test_ctree_builder.json"));
    builder.getModelCollection().loadJson(STRINGIFY([
      {"id":"typical_usage.Namer", "type":"Namer", "name":"root"},
      {"id":"typical_usage.Namer.Ager", "age":"10"},
      {"id":"typical_usage.Namer.Ager.SubAger", "type":"Ager", "age":"3"},
      {"id":"typical_usage.Namer.Ager2", "type":"Ager", "age":"20"},
      {"id":"typical_usage.Ager3", "type":"Ager", "age":"30"},
      {"id":"typical_usage.Ager3.Namer", "name":"child_of_30"}
    ]));

    // configure your builder by registering instantiators
    // (map classes that can be "build" to a type identifier)
    builder.addDefaultInstantiator<Namer>("Namer");
    builder.addDefaultInstantiator<Ager>("Ager");

    // build an item from the json data (identify by "id")
    auto namer = builder.build<Namer>("typical_usage.Namer");
    REQUIRE(namer->name == "root");
    // check that our object was built with two child objects
    REQUIRE(builder.select(namer)->getNode()->size() == 2);
    REQUIRE(builder.select(namer)->getName() == "Namer");

    // fetch a "child" item from the just created hierarchy
    // of which names was the root element
    auto ager1 = builder.select(namer)->get<Ager>("Ager");
    REQUIRE(ager1->age == 10);
    REQUIRE(builder.select(ager1)->getNode()->size() == 1);
    REQUIRE(builder.select(ager1)->getName() == "Ager");

    // fetch child of child (grandchild of namer)
    REQUIRE(builder.select(ager1)->get<Ager>("SubAger")->age == 3);

    // fetch another child item
    auto ager2 = builder.select(namer)->get<Ager>("Ager2");
    REQUIRE(ager2->age == 20);
    REQUIRE(builder.select(ager2)->getNode()->size() == 0);
    REQUIRE(builder.select(ager2)->getName() == "Ager2");

    // try (and fail) to fetch some non-existing child items
    REQUIRE(builder.select(namer)->get<Ager>("Ager3") == NULL);
    REQUIRE(builder.select(namer)->get<Ager>("foobar") == NULL);

    // build a new set of objects
    auto ager3 = builder.build<Ager>("typical_usage.Ager3");
    REQUIRE(ager3->age == 30);
    REQUIRE(builder.select(ager3)->getNode()->size() == 1);
    REQUIRE(builder.select(ager3)->get<Namer>("Namer")->name == "child_of_30");
    REQUIRE(builder.select(ager3)->getName() == "Ager3"); // node name

    // attach new nodes to our original hierarchy
    builder.select(namer)->attach(ager3);

    // verify that that names now has a third child
    REQUIRE(builder.select(namer)->getNode()->size() == 3);
    // REQUIRE(builder.select(namer)->get<Ager>("Ager3") != NULL);
    REQUIRE(builder.select(namer)->get<Ager>("Ager3") == ager3);

    // fetch sub item various level deep
    REQUIRE(builder.select(namer)->get<Namer>("Ager3.Namer")->name == "child_of_30");
    REQUIRE(builder.select(namer)->get<Namer>("Ager3.Namer") == builder.select(ager3)->get<Namer>("Namer"));
  }
}

class ConfigurableAger {
  public:
    int age;

    void cfg(cfg::Cfg& cfg) {
      cfg.setInt("age", age);
    }
};

// Configurable object which relies on an external Configurable Object
class ConfigurableNamer {
  private:
    std::function<std::string()> realtimeNameFunc = nullptr;


  public:
    ctree::Signal<void(const std::string&)> nameSignal;
    std::vector<std::string> nameEmitHistory;

    string name;
    string realtimeName() { return realtimeNameFunc ? realtimeNameFunc() : name; }

    void cfg(cfg::Cfg& cfg) {
      cfg
      .withSignalByAttr<void(const std::string&)>("nameEmit", [this](auto &sig) {
        nameSignal.connect([this, &sig](const std::string& v) { 
          this->nameEmitHistory.push_back(v);
          sig.emit(v);
        });
      })
      .connectAttr<void(const std::string&)>("nameOn", [this](const std::string& val){
        this->name = val;
        // this->nameSignal.emit(this->name);
      })
      .set("name", name)
      .withObjectByAttr<ConfigurableAger>("AgerPostfix", [this](ConfigurableAger& ager) {
        // configure name with configure-time value of the AgerPostfix object
        this->name += " ["+std::to_string(ager.age)+"]";
        // create dynamic "link" which always a name with up-to-date AgerPostfix value
        this->realtimeNameFunc = [this, &ager]() {
          return (this->name + " ["+std::to_string(ager.age)+"]");
        };
      });
    }
};


TEST_CASE("cms::cfg::ctree::Builder With configurable objects", ""){
    SECTION("typical_usage"){
      // prepare a builder and populate with data
      cms::cfg::ctree::Builder<Configer> builder;
      // builder.getModelCollection().loadJsonFromFile(ci::app::getAssetPath("test_ctree_builder.json"));
      builder.getModelCollection().loadJson(STRINGIFY([
        {"id":"ConfigurableObjects.ConfigurableNamer", "name":"Bob", "nameOn":"setName"},
        {"id":"ConfigurableObjects.ConfigurableNamer.ConfigurableAger", "age":"46"}
      ]));

      // configure your builder by registering instantiators
      // (map classes that can be "build" to a type identifier)
      builder.addCfgObjectInstantiator<ConfigurableNamer>("ConfigurableNamer");
      builder.addCfgObjectInstantiator<ConfigurableAger>("ConfigurableAger");

      // build an item from the json data (identify by "id")
      auto namer = builder.build<ConfigurableNamer>("ConfigurableObjects.ConfigurableNamer");
      REQUIRE(namer != NULL);
      REQUIRE(namer->name == "Bob");
      REQUIRE(builder.select(namer)->getNode()->size() == 1);

      auto ager = builder.select(namer)->get<ConfigurableAger>("ConfigurableAger");
      REQUIRE(ager != NULL);
      REQUIRE(ager->age == 46);
    }
};

// Configurator which relies on an external Configurable Object
class Configer2 : public cms::cfg::Configurator {
public:

  Configer2() {
  }

  Configer2(ModelCollection& mc) : cms::cfg::Configurator(mc) {
  }

  // // using cms::cfg::ctree::cfgWithModel;
  // void cfg(cms::cfg::ctree::Node& n, const std::map<string, string>& data){
  //   // TODO; take name from name attribute or otherwise default to last part
  //   // (splitting by period (.) of the id)
  // }

  void cfg(Namer& obj, const std::map<string, string>& data){
    Model m;
    m.set(data);

    auto reader = read(data);
    auto fnameRef = std::make_shared<string>();
    fnameRef->operator=(reader->get("firstname", "<No firstname>"));

    // set firstname for now
    obj.name = *fnameRef;

    // complete the name with the last name when the Lastname object becomes available
    this->withObjectByAttr<ConfigurableNamer>("Lastname", [&obj, fnameRef](ConfigurableNamer& lastnamer) {
      // configure-time value
      obj.name = (*fnameRef) + " " + lastnamer.name;
      // dynamic realtime value
      obj.realtimeNameFunc = [&obj, &lastnamer](){ return obj.name + " " + lastnamer.realtimeName(); };
    });

    read(data)
    ->with("nameEmit", [this, &obj](const std::string& v){
      auto sig = this->getSignal<void(const std::string&)>(v);
      obj.nameSignal.connect([sig](const std::string& v) { sig->emit(v); });
    })
    .with("nameOn", [this, &obj](const std::string& v){
      auto sig = this->getSignal<void(const std::string&)>(v);
      sig->connect([&obj](const std::string& v){ obj.name = "nameOn: "+v; });
    });
  }
};

TEST_CASE("cms::cfg::ctree::Builder With both configurator and configurable objects that rely on each other", ""){
  SECTION("typical usage") {
    // prepare a builder and populate with data
    cms::cfg::ctree::Builder<Configer2> builder;
    // builder.getModelCollection().loadJsonFromFile(ci::app::getAssetPath("test_ctree_builder.json"));
    builder.getModelCollection().loadJson(STRINGIFY([
      {"id":"MixedObjects.Namer", "firstname":"John", "Lastname": "MixedObjects.Namer.Lastname", "nameOn": "changeName"},
      {"id":"MixedObjects.Namer.Lastname", "type": "ConfigurableNamer", "name":"Doe", "AgerPostfix": "MixedObjects.Namer.ConfigurableAger", "nameEmit": "changeName"},
      {"id":"MixedObjects.Namer.ConfigurableAger", "age":"88"}
    ]));

    // configure your builder by registering instantiators
    // (map classes that can be "build" to a type identifier)
    builder.addConfiguratorObjectInstantiator<Namer>("Namer");
    builder.addCfgObjectInstantiator<ConfigurableNamer>("ConfigurableNamer");
    builder.addCfgObjectInstantiator<ConfigurableAger>("ConfigurableAger");

    auto namer = builder.build<Namer>("MixedObjects.Namer");
    REQUIRE(namer->name == "John Doe");
    auto lastname = builder.select(namer)->get<ConfigurableNamer>("Lastname");
    REQUIRE(lastname->name == "Doe [88]");
    lastname->nameSignal.emit("Newman");
    REQUIRE(lastname->nameEmitHistory.size() == 1);
    REQUIRE(lastname->nameEmitHistory[0] == "Newman");
    REQUIRE(namer->name == "Newman");
  }
}
#endif // CICMS_CTREE
