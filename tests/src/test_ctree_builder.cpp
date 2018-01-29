#ifdef CICMS_CTREE

#include "catch.hpp"
#include <iostream>

#include "ciCMS/cfg/ctree/Builder.h"
#include "ciCMS/cfg/Configurator.h"
#include "ciCMS/Model.h"

using namespace cms;

class Namer {
  public:
    string name;
};

class Ager {
  public:
    int age;
};

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
    // builder instance
    cms::cfg::ctree::Builder<Configer> builder;
    builder.getModelCollection().loadJsonFromFile(ci::app::getAssetPath("test_ctree_builder.json"));

    builder.addDefaultInstantiator<Namer>("Namer");
    builder.addDefaultInstantiator<Ager>("Ager");
    // build an item from the json data
    auto namer = builder.build<Namer>("typical_usage.Namer");
    REQUIRE(namer->name == "root");
    auto ager1 = builder.from(namer)->get<Ager>("Ager");
    // std::cout << "ager1: " << ager1;
    REQUIRE(ager1->age == 10);
    auto ager2 = builder.from(namer)->get<Ager>("Ager2");
    // std::cout << "ager2: " << ager2;
    REQUIRE(ager2->age == 20);
    auto ager3 = builder.from(namer)->get<Ager>("Ager3");
    REQUIRE(ager3 == NULL);
    auto foo = builder.from(namer)->get<Ager>("foorbar");
    REQUIRE(foo == NULL);
  }
}

#endif // CICMS_CTREE
