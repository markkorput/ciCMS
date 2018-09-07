#include "catch.hpp"
#include <map>

#include "ciCMS/cfg/Cfg.h"
#include "ctree/signal.hpp"
#include "ciCMS/ModelBase.h"
#include "ciCMS/State.h"

using namespace std;
using namespace cms;
using namespace cms::cfg;

TEST_CASE("cms::cfg::Cfg", ""){
  SECTION(".ctor(signalsMap, statesMap)"){
    // create our Cfg instance with external signals and states maps
    map<string, void*> signals;
    map<string, void*> states;
    Cfg cfg(signals, states);

    // add a state to external map
    auto state1 = new cms::State<int>();
    states["state1"] = (void*)state1;
    (*state1) = 3;
    // verify it's available from our Cfg instance
    REQUIRE(state1->val() == 3);
    REQUIRE(cfg.getState<int>("state1").val() == 3);

    // fetch (generate) some more state instances
    cfg.getState<float>("state2");
    cfg.getState<string>("state3");

    // verify our original states map grew
    REQUIRE(states.size() == 3);

    auto signal1 = new ctree::Signal<void()>();
    signals["signal1"] = (void*)signal1;
    signal1->connect([state1](){ state1->set(state1->val()+1); });
    REQUIRE(state1->val() == 3);
    cfg.getSignal<void()>("signal1").emit();
    REQUIRE(state1->val() == 4);

    cfg.getSignal<void(int)>("signal2");
    cfg.getSignal<void(float)>("signal3");

    // verify our original signals map grew
    REQUIRE(signals.size() == 3);
  }

  SECTION(".set"){

    Cfg cfg({ {"name", "Bob"}, {"age", "34"}, {"male", "true"} });

    string name="";
    int age = 0;
    bool male = false;

    cfg
    .set("name", name)
    .set("age", age)
    .set("male", male);

    REQUIRE(name == "Bob");
    REQUIRE(age == 34);
    REQUIRE(male);
  }

  SECTION(".connect(str, func)") {
    Cfg cfg;
    int signal1Counter = 0;
    cfg.connect<void()>("signal1", [&signal1Counter](){ signal1Counter += 1; });

    REQUIRE(signal1Counter == 0);

    cfg.getSignal<void()>("signal1").emit();
    REQUIRE(signal1Counter == 1);

    cfg.getSignal<void()>("signal1").emit();
    cfg.getSignal<void()>("signal1").emit();
    REQUIRE(signal1Counter == 3);
  }

  // SECTION(".connect(signal)") {
  //   Cfg cfg;
  //
  //   ctree::Signal<void()> signalY;
  //   cfg.connect<void()>("signalX", signalY);
  //
  //   int counter = 0;
  //   signalY.connect([&counter](){ counter += 1; });
  //   REQUIRE(counter == 0);
  //   cfg.getSignal<void()>("signalX").emit();
  //   REQUIRE(counter == 1);
  // }

  SECTION(".getSignal"){
    std::cout << "TODO";
  }

  SECTION(".getState") {
    std::cout << "TODO";
  }

  SECTION(".withState") {
    std::cout << "TODO";
  }

  SECTION(".withSignal") {
    std::cout << "TODO";
  }

  SECTION(".withObjects") {
    std::cout << "TODO";
  }
}
