#ifdef CICMS_CTREE

#include "catch.hpp"
#include <iostream>

#include "ciCMS/cfg/ctree/Builder.h"
#include "ciCMS/cfg/Configurator.h"
#include "ciCMS/Model.h"
#include "ciCMS/cfg/info/Interface.h"

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

    class InfoKeyboard {
      public:
        static cfg::info::Interface* createInfoInterface() {
          auto info = new cfg::info::Interface();

          { // create output with type char
            auto& output = info->output<char>("KeyCode");
            info->output<bool>("HasKeyDown");

            // invoke output when a key is pressed
            // this->onKeyDown([&output](char keycode){ output.push(keycode); });
            info->withInstance<InfoKeyboard>([&output](InfoKeyboard& instance) {
              instance.onKeyDown([&output](char keycode){ output.push(keycode); });
            });

            // info.behaviour([](InfoKeyboard& instance, ))
          }

          return info;
        }

      protected:

        void onKeyDown(std::function<void(char)> func) {
          // if(key.size() != 1) return;
          // char chr = key[0];

          // this->connections.push_back(ci::app::getWindow()->getSignalKeyDown().connect([func](ci::app::KeyEvent& event){
          //   func(event.getChar());
          // }));

          this->connections.push_back(this->keySignal.connect(func));
        }

      private:
        std::vector<ci::signals::Connection> connections;
      public:
        ctree::Signal<void(char)> keySignal;
    };

    cfg::ctree::TreeBuilder builder;
    auto info = builder.addInfoObjectInstantiator<InfoKeyboard>("Keyboard");
    // builder.getModelCollection().loadJsonFromFile(ci::app::getAssetPath("info_keyboard.json"));

    // verify we can extract outputs information from info interface
    std::vector<std::string> ids = {"KeyCode", "HasKeyDown"};
    for(int i=0; i<ids.size(); i++) {
      REQUIRE(ids[i] == info.getOutputs()[i]->getId());
    }

    std::vector<std::string> types = {"c" /* char */, "b" /* bool */};
    for(int i=0; i<types.size(); i++) {
      REQUIRE(types[i] == info.getOutputs()[i]->getType());
    }
  }
}

#endif
