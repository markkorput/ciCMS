#ifdef CICMS_CTREE

#include "catch.hpp"
#include <iostream>

#include "ciCMS/cfg/ctree/Builder.h"
#include "ciCMS/cfg/Configurator.h"
#include "ciCMS/Model.h"

using namespace cms;

class Object {
public:
  int number;

  void cfg(cfg::Cfg& cfg) {
    cfg
    .connectAttr<void()>("emitOn", [](){

    })
    .connectAttr<void()>("executeOn", [](){

    })
    .pushRef<int>("state", this->number);
  }
};

TEST_CASE("cms::cfg::ctree::TreeBuilder", ""){
  SECTION("reset"){
    cfg::ctree::TreeBuilder builder;
    builder.addCfgObjectInstantiator<Object>("Object");
    builder.getModelCollection().loadJsonFromFile(ci::app::getAssetPath("test_ctreebuilder.json"));

    REQUIRE(builder.getConfigurator()->getStates().size() == 0);
    REQUIRE(builder.getConfigurator()->getSignals().size() == 0);

    auto obj = builder.build<Object>("Object");
    REQUIRE(builder.getRegistry()->size() == 1);
    REQUIRE(builder.getConfigurator()->getStates().size() == 1);
    REQUIRE(builder.getConfigurator()->getSignals().size() == 2);

    builder.reset();
    REQUIRE(builder.getRegistry()->size() == 0);
    REQUIRE(builder.getConfigurator()->getStates().size() == 0);
    REQUIRE(builder.getConfigurator()->getSignals().size() == 0);
  }
}

#endif
