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
  SECTION("constructor(signalsMap, statesMap)"){
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
    REQUIRE(cfg.getState<int>("state1")->val() == 3);

    // fetch (generate) some more state instances
    cfg.getState<float>("state2");
    cfg.getState<string>("state3");

    // verify our original states map grew
    REQUIRE(states.size() == 3);

    auto signal1 = new ctree::Signal<void()>();
    signals["signal1"] = (void*)signal1;
    signal1->connect([state1](){ state1->set(state1->val()+1); });
    REQUIRE(state1->val() == 3);
    cfg.getSignal<void()>("signal1")->emit();
    REQUIRE(state1->val() == 4);

    cfg.getSignal<void(int)>("signal2");
    cfg.getSignal<void(float)>("signal3");

    // verify our original signals map grew
    REQUIRE(signals.size() == 3);
  }

  SECTION("destructor cleans up created states and signals"){
    map<string, void*> states, signals, objects;
    states["state1"] = new cms::State<int>();
    signals["signal1"] = new ::ctree::Signal<void()>();

    {
      Cfg cfg(signals, states, [&objects](const string& id){ return objects[id]; });

      cfg.getSignal<void()>("signal2");
      cfg.getSignal<void()>("signal3");
      REQUIRE(signals.size() == 3);

      cfg.getState<int>("state2");
      cfg.getState<bool>("state3");
      cfg.getState<string>("state4");
      REQUIRE(states.size() == 4);
    } // and of cfg scope; destructor invoked

    REQUIRE(states.size() == 1);
    REQUIRE(signals.size() == 1);
  }

  SECTION(".withData"){
    map<string, void*> states, signals, objects;
    Cfg cfg(signals, states, [&objects](const string& id){ return objects[id]; });
    string name = "Bob";

    map<string, string> attrs = {{"name", "Joe"}};
    cfg.withData(attrs).set("name", name);
    REQUIRE(name == "Joe");

    attrs = {{"name", "Ash"}};
    cfg.withData(attrs).set("name", name);
    REQUIRE(name == "Ash");
  }

  SECTION(".set"){
    map<string, string> data = { {"name", "Bob"}, {"age", "34"}, {"male", "true"} };
    Cfg cfg(data);

    string name="";
    int age = 0;
    bool male = false;

    cfg
    .set("name", name)
    .setInt("age", age)
    .setBool("male", male);

    // REQUIRE(name == "Bob");
    REQUIRE(age == 34);
    REQUIRE(male);
  }

  SECTION(".connect(str, func)") {
    Cfg cfg;
    int signalCounter = 0;
    cfg.connect<void()>("signal1,signal2", [&signalCounter](){ signalCounter += 1; });

    REQUIRE(signalCounter == 0);

    cfg.getSignal<void()>("signal1")->emit();
    REQUIRE(signalCounter == 1);

    cfg.getSignal<void()>("signal2")->emit();
    REQUIRE(signalCounter == 2);

    cfg.getSignal<void()>("signal1")->emit();
    cfg.getSignal<void()>("signal1")->emit();
    cfg.getSignal<void()>("signal2")->emit();
    REQUIRE(signalCounter == 5);
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
  //   cfg.getSignal<void()>("signalX")->emit();
  //   REQUIRE(counter == 1);
  // }

  SECTION(".getSignal"){
    Cfg cfg;
    int counter = 0;
    cfg.connect<void()>("signal2", [&counter](){ counter += 1; });

    REQUIRE(counter == 0);

    cfg.getSignal<void()>("signal1")->connect([&cfg](){ cfg.getSignal<void()>("signal2")->emit(); });
    cfg.getSignal<void()>("signal1")->emit();

    REQUIRE(counter == 1);
    cfg.getSignal<void()>("signal2")->emit();
    REQUIRE(counter == 2);
  }

  SECTION(".getState") {
    Cfg cfg;
    cfg.getState<int>("var1")->set(3);
    REQUIRE(cfg.getState<int>("var1")->val() == 3);
  }

  SECTION(".withState") {
    std::cout << "TODO";
  }

  SECTION(".withSignal") {
    std::cout << "TODO";
  }

  SECTION("getObject getObjectPointer"){
    map<string, void*> states, signals, objects;
    objects["states"] = &states;
    Cfg cfg(signals, states, [&objects](const string& id){ return objects[id]; });

    REQUIRE(cfg.getObject<Cfg>("foo.bar") == NULL);
    REQUIRE(cfg.getObjectPointer("states") == &states);
    REQUIRE((cfg.getObject<map<string, void*>>("states") == &states));
    REQUIRE((cfg.getObject<map<string, void*>>("foostates") == NULL));
  }

  SECTION("getObjects") {
    map<string, void*> states, signals, objects;
    objects["states"] = &states;
    objects["signals"] = &signals;
    Cfg cfg(signals, states, [&objects](const string& id){ return objects[id]; });

    vector<map<string, void*>*> result;
    REQUIRE((cfg.getObjects<map<string, void*>>(result, "signals,states,foobar") == 2));
    REQUIRE(result.size() == 2);
    REQUIRE(result[0] == &signals);
    REQUIRE(result[1] == &states);
  }

  SECTION("withObject") {
    map<string, void*> states, signals, objects;
    objects["objects"] = &objects;
    Cfg cfg(signals, states, [&objects](const string& id){ return objects[id]; });

    int counter = 0;
    cfg.withObject<map<string, void*>>("objects", [&counter](map<string, void*>& objs) {
      counter += objs.size();
    });

    REQUIRE(counter == 1);

    cfg.withObject<map<string, void*>>("states", [&counter](map<string, void*>& result) {
      counter += 1;
    });

    REQUIRE(counter == 1);
  }

  SECTION("withObjects") {
    map<string, void*> states, signals, objects;
    objects["objects"] = &objects;
    objects["states"] = &states;
    Cfg cfg(signals, states, [&objects](const string& id){ return objects[id]; });

    int counter = 0;
    cfg.withObjects<map<string, void*>>("objects,states,signals", [&counter](map<string, void*>& result) {
      counter += 100 + result.size();
    });

    REQUIRE(counter == 202);
  }

  SECTION("withObjects:with_object_id_in_callback") {
    map<string, void*> states, signals, objects;
    objects["objects"] = &objects;
    objects["states"] = &states;
    Cfg cfg(signals, states, [&objects](const string& id){ return objects[id]; });

    int counter = 0;
    cfg.withObjects<map<string, void*>>("objects,states,signals", [&counter](map<string, void*>& obj, const string& objId) {
      counter += 100 + obj.size() * 10 + objId.size();
    });

    REQUIRE(counter == 233); // 100 + 2 * 10 + 7   +   100 + 0 * 10 + 6
  }

  SECTION("withObject:async") {
    map<string, void*> states, signals, objects;
    Cfg cfg(signals, states, [&objects](const string& id){ return objects[id]; });

    int counter = 0;
    cfg.withObject<int>("CoolNewObject", [&counter](int& obj) {
      counter += obj;
    });

    REQUIRE(counter == 0);
    auto obj = new int(8);
    objects["CoolNewObject"] = obj;
    REQUIRE(counter == 0);
    cfg.notifyNewObject("CoolNewObject", obj);
    REQUIRE(counter == 8);
  }

  SECTION("withObjects:async") {
    std::cerr << "TODO" << std::endl;
  }

  SECTION("compileScript:toggle"){
    Cfg cfg;
    auto toggleFunc = cfg.compileScript("toggle:turnedOn");
    cfg.getState<bool>("turnedOn")->set(false);
    REQUIRE(cfg.getState<bool>("turnedOn")->val() == false);
    toggleFunc();
    REQUIRE(cfg.getState<bool>("turnedOn")->val() == true);
    toggleFunc();
    REQUIRE(cfg.getState<bool>("turnedOn")->val() == false);
    toggleFunc(); toggleFunc();
    REQUIRE(cfg.getState<bool>("turnedOn")->val() == false);
  }

  SECTION("compileScript:+1") {
    Cfg cfg;
    auto func = cfg.compileScript("+1:someInt");
    cfg.getState<int>("someInt")->set(1);
    REQUIRE(cfg.getState<int>("someInt")->val() == 1);
    func();
    REQUIRE(cfg.getState<int>("someInt")->val() == 2);
    func();func();
    REQUIRE(cfg.getState<int>("someInt")->val() == 4);
  }

  SECTION("compileScript:-3") {
    Cfg cfg;
    auto func = cfg.compileScript("-3:someInt");
    cfg.getState<int>("someInt")->set(0);
    REQUIRE(cfg.getState<int>("someInt")->val() == 0);
    func();
    REQUIRE(cfg.getState<int>("someInt")->val() == -3);
    func();
    func();
    REQUIRE(cfg.getState<int>("someInt")->val() == -9);
  }

  SECTION("compileScript:+4.6") {
    Cfg cfg;
    auto func = cfg.compileScript("+4.6:someFloat");
    cfg.getState<float>("someFloat")->operator=(1.0f);
    REQUIRE(cfg.getState<float>("someFloat")->val() == 1.0f);
    func();
    REQUIRE(cfg.getState<float>("someFloat")->val() == 5.6f);
    func();func();
    REQUIRE(std::abs(cfg.getState<float>("someFloat")->val()-14.8f) < 0.0001f);
  }

  SECTION("compileScript:-3.2") {
    Cfg cfg;
    auto func = cfg.compileScript("-3.2:someFloat");
    cfg.getState<float>("someFloat")->operator=(1.0f);
    func();
    REQUIRE(cfg.getState<float>("someFloat")->val() == -2.2f);
    func();func();
    REQUIRE(std::abs(cfg.getState<float>("someFloat")->val()+8.6f) < 0.0001f);
  }

  SECTION("compileScript:emit") {
    Cfg cfg;
    int count = 0;
    cfg.getSignal<void()>("foobar")->connect([&count](){ count += 2; });
    REQUIRE(count == 0);

    auto func = cfg.compileScript("emit:foobar");
    func();
    REQUIRE(count == 2);
    func(); func();
    REQUIRE(count == 6);
  }

  SECTION("compileScript:emit;toggle") {
    Cfg cfg;
    int count1=0, count2=0;
    cfg.getSignal<void()>("signaller")->connect([&count1](){ count1 += 1; });
    cfg.getState<bool>("toggler")->push([&count2](const bool& v){ count2 += 1; });

    REQUIRE(count1 == 0);
    REQUIRE(count2 == 0);

    auto func = cfg.compileScript("emit:signaller;toggle:toggler");
    func();
    REQUIRE(count1 == 1);
    REQUIRE(count2 == 1);
    func(); func();
    REQUIRE(count1 == 3);
    REQUIRE(count2 == 3);
  }

  SECTION("push") {
    Cfg cfg;
    int someInt = 44;

    cfg.getState<int>("someInt")->set(33);
    cfg.push("someInt", someInt);
    REQUIRE(someInt == 33);

    cfg.getState<int>("someInt")->set(55);
    REQUIRE(someInt == 55);
  }

  SECTION("pushRef") {
    map<string, void*> states, signals, objects;
    Cfg cfg(signals, states, [&objects](const string& id){ return objects[id]; });


    map<string, string> attrs = {{"ageState", "ResultAge"}};
    cfg.setAttributes(attrs);

    cfg.getState<int>("ageState")->set(50); // NOT USED
    cfg.getState<int>("ResultAge")->set(60); // <-- USED

    int someInt = 40;
    cfg.pushRef("ageState", someInt);
    REQUIRE(someInt == 60);

    cfg.getState<int>("ResultAge")->set(70);
    REQUIRE(someInt == 70);
  }
}
