#ifdef CICMS_CTREE

#include "catch.hpp"
#include <iostream>

#include "ciCMS/cfg/ctree/Builder.h"
#include "ciCMS/cfg/Configurator.h"
#include "ciCMS/Model.h"
#include "info/Interface.h"

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
    builder.getModelCollection().loadJsonFromFile(ci::app::getAssetPath("test_treebuilder.json"));

    REQUIRE(builder.getRegistry()->size() == 0);
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

  SECTION("Registry"){
    SECTION("getById for non-existing id") {
      cfg::ctree::TreeBuilder builder;
      REQUIRE(builder.getRegistry()->getById("foobar") == NULL);
    }
  }

  SECTION("addInfoObjectInstantiator"){

    class Keyboard {
      public:
        static std::shared_ptr<info::Interface> createInfoInterface() {
          return info::Interface::create<Keyboard>([](info::Builder<Keyboard>& builder){
            builder.output<char>("KeyCode")
              ->apply([](Keyboard& instance, std::function<void(const char&)> out) {
                instance.keySignal.connect([out](char keycode){
                  out(keycode);
                });
              });

            builder.output<bool>("HasKeyDown");
            
            builder.attr<bool>("enabled")
              ->apply([](Keyboard& instance, info::TypedPort<bool>& port) {

                port.onDataIn([&instance](const bool& val){
                  instance.enabled = val;
                });

              });
            
            builder.signal("AnyKey")->apply([](Keyboard& instance, info::Port& port) {
                instance.keySignal.connect([&port](char keycode){
                  port.signalOut();
                });
              });
          });
        }
    
        cinder::signals::Signal<void(char)> keySignal;
        bool enabled = false;
    };


    cfg::ctree::TreeBuilder builder;
    auto info = builder.addInfoObjectInstantiator<Keyboard>("Keyboard");
    // builder.getModelCollection().loadJsonFromFile(ci::app::getAssetPath("info_keyboard.json"));

    // verify we can extract outputs information from info interface
    std::vector<std::string> ids = {"KeyCode", "HasKeyDown", "enabled"};
    for(int i=0; i<ids.size(); i++) {
      REQUIRE(ids[i] == info.getPorts()[i]->getId());
    }

    std::vector<std::string> types = {"c" /* char */, "b" /* bool */};
    for(int i=0; i<types.size(); i++) {
      REQUIRE(types[i] == info.getPorts()[i]->getType());
    }
  }
}

#endif
