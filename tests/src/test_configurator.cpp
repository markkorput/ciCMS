// #include "catch.hpp"
//
// #include "ciCMS/cfg/Configurator.h"
// #include "ciCMS/cfg/ConfigurableFacade.h"
// #include "ciCMS/ModelCollection.h"
//
// using namespace cms;
// using namespace cms::cfg;
//
// class CustomObject {
//   public:
//     std::string name;
//     int age;
//     float score;
//     CustomObject() : name(""), age(0), score(0.0f){}
// };
//
// class CustomFlagObject {
//   public:
//     bool flag;
//     CustomFlagObject() : flag(false){}
// };
//
// class CfgFacade : public ConfigurableFacade<CustomObject> {
//   public:
//     // constructor registers two configurable attributes
//     CfgFacade(){
//       this->add<string>("name", [](CustomObject &obj, const std::string &v){
//         obj.name = v;
//       });
//
//       this->add<int>("age", [](CustomObject &obj, const int &v){
//         obj.age = v;
//       });
//     }
//
//     // custom cfg method applies registered configurable attributes
//     // and then performs some more custom config logic
//     void cfg(CustomObject& obj, const std::map<string, string>& data) override {
//       // apply attributes registered inconstructor
//       ConfigurableFacade<CustomObject>::cfg(obj, data);
//       // apply custom configuration logic
//       reader(data)->withFloat("score", [&obj](float v){ obj.score = v; });
//     }
// };
//
// class CfgFlagFacade : public ConfigurableFacade<CustomFlagObject> {
//   public:
//     // constructor registers two configurable attributes
//     CfgFlagFacade(){
//       this->add<bool>("flag", [](CustomFlagObject &obj, const bool &v){
//         obj.flag = v;
//       });
//     }
// };
//
// class Cfg : public Configurator, public CfgFacade, public CfgFlagFacade {
//   public:
//
//     using Configurator::cfg;
//     using CfgFacade::cfg;
//     using CfgFlagFacade::cfg;
//
//     // overwrite Configurator's version, because that one only knows about
//     // the cfg methods in the Configurator class
//     template<typename T>
//     void cfgWithModel(T& c, Model& model){
//       this->apply(model, [this, &c](ModelBase& mod){
//         this->cfg(c, mod.attributes());
//       });
//     }
// };
//
// TEST_CASE("", "[cms::cfg::Configurator]"){
//   SECTION("default_model_collection"){
//     Configurator configurator;
//     REQUIRE(configurator.getModelCollection().size() == 0);
//   }
//
//   SECTION("given_model_collection"){
//     ModelCollection mc;
//     Configurator configurator(mc);
//     REQUIRE(&mc == &configurator.getModelCollection());
//   }
//
//   SECTION("has_cfg_method_for_itself"){
//     std::map<string, string> data;
//     data["active"] = "true";
//     Configurator configurator;
//     REQUIRE(configurator.isActive() == false);
//     configurator.cfg(configurator, data);
//     REQUIRE(configurator.isActive() == true);
//   }
//
//   SECTION("setActive"){
//     Configurator configurator;
//     REQUIRE(configurator.isActive() == false);
//     configurator.setActive(true);
//     REQUIRE(configurator.isActive() == true);
//   }
//
//   SECTION("active_configurator"){
//     Configurator configurator;
//     configurator.setActive(true);
//     auto model = configurator.getModelCollection().findById("model", true);
//     configurator.cfgWithModel(configurator, *model);
//     REQUIRE(configurator.isActive() == true);
//     model->set("active", "false");
//     REQUIRE(configurator.isActive() == false);
//     model->set("active", "true");
//     REQUIRE(configurator.isActive() == true);
//   }
//
//   SECTION("active_configurator_with_configurable_facades"){
//     CustomObject obj;
//     Cfg cfg;
//
//     std::map<string, string> data;
//     data["name"] = "Jane Doe";
//     data["age"] = "31";
//     data["score"] = "10.5";
//
//     cfg.cfg(obj, data);
//     REQUIRE(obj.name == "Jane Doe");
//     REQUIRE(obj.age == 31);
//     REQUIRE(obj.score == 10.5f);
//
//     data["name"] = "Janet Doe";
//     data["age"] = "32";
//     data["score"] = "11.5";
//     REQUIRE(obj.name == "Jane Doe");
//     REQUIRE(obj.age == 31);
//     REQUIRE(obj.score == 10.5f);
//
//     cfg.cfg(obj, data);
//     REQUIRE(obj.name == "Janet Doe");
//     REQUIRE(obj.age == 32);
//     REQUIRE(obj.score == 11.5f);
//
//     Model m;
//     m.set(data);
//
//     cfg.setActive(true);
//     cfg.cfgWithModel(obj, m);
//     REQUIRE(obj.name == "Janet Doe");
//     REQUIRE(obj.age == 32);
//     REQUIRE(obj.score == 11.5f);
//
//     m.set("name", "Janine Doe");
//     REQUIRE(obj.name == "Janine Doe");
//     REQUIRE(obj.age == 32);
//     REQUIRE(obj.score == 11.5f);
//
//     data["age"] = "33";
//     data["score"] = "20.3";
//     REQUIRE(obj.name == "Janine Doe");
//     REQUIRE(obj.age == 32);
//     REQUIRE(obj.score == 11.5f);
//
//     m.set(data);
//     REQUIRE(obj.name == "Janet Doe");
//     REQUIRE(obj.age == 33);
//     REQUIRE(obj.score == 20.3f);
//
//     CustomFlagObject flagObj;
//     Model flagModel;
//     cfg.cfgWithModel(flagObj, flagModel);
//     REQUIRE(flagObj.flag == false);
//     flagModel.set("flag", "true");
//     REQUIRE(flagObj.flag == true);
//   }
//
//   SECTION("getObject setObjectFetcher"){
//     Cfg cfg;
//     REQUIRE(cfg.getObject<Cfg>("foo.bar") == NULL);
//     cfg.setObjectFetcher([&cfg](const std::string& id){ return &cfg; });
//     REQUIRE(cfg.getObjectPointer("foo.bar") == &cfg);
//     REQUIRE(cfg.getObject<Cfg>("everything.returns.cfg") == &cfg);
//   }
//
//   SECTION("withObject") {
//     std::cerr << "TODO" << std::endl;
//   }
//
//   SECTION("getObjects") {
//     std::cerr << "TODO" << std::endl;
//   }
//
//   SECTION("withObjects") {
//     std::cerr << "TODO" << std::endl;
//   }
//
//   SECTION("withObjects_with_id") {
//
//   }
//
//   SECTION("compileScript"){
//     Configurator cfg;
//     {
//       auto toggleFunc = cfg.compileScript("toggle:turnedOn");
//       cfg.getState<bool>("turnedOn")->operator=(false);
//       REQUIRE(cfg.getState<bool>("turnedOn")->val() == false);
//       toggleFunc();
//       REQUIRE(cfg.getState<bool>("turnedOn")->val() == true);
//       toggleFunc();
//       REQUIRE(cfg.getState<bool>("turnedOn")->val() == false);
//       toggleFunc(); toggleFunc();
//       REQUIRE(cfg.getState<bool>("turnedOn")->val() == false);
//     }
//
//     {
//       auto func = cfg.compileScript("+1:someInt");
//       cfg.getState<int>("someInt")->operator=(1);
//       REQUIRE(cfg.getState<int>("someInt")->val() == 1);
//       func();
//       REQUIRE(cfg.getState<int>("someInt")->val() == 2);
//       func();func();
//       REQUIRE(cfg.getState<int>("someInt")->val() == 4);
//     }
//
//     {
//       auto func = cfg.compileScript("-3:someInt");
//       REQUIRE(cfg.getState<int>("someInt")->val() == 4);
//       func();
//       REQUIRE(cfg.getState<int>("someInt")->val() == 1);
//       func();func();
//       REQUIRE(cfg.getState<int>("someInt")->val() == -5);
//     }
//
//     {
//       auto func = cfg.compileScript("+4.6:someFloat");
//       cfg.getState<float>("someFloat")->operator=(1.0f);
//       REQUIRE(cfg.getState<float>("someFloat")->val() == 1.0f);
//       func();
//       REQUIRE(cfg.getState<float>("someFloat")->val() == 5.6f);
//       func();func();
//       REQUIRE(std::abs(cfg.getState<float>("someFloat")->val()-14.8f) < 0.0001f);
//     }
//
//     {
//       auto func = cfg.compileScript("-3.2:someFloat");
//       cfg.getState<float>("someFloat")->operator=(1.0f);
//       func();
//       REQUIRE(cfg.getState<float>("someFloat")->val() == -2.2f);
//       func();func();
//       REQUIRE(std::abs(cfg.getState<float>("someFloat")->val()+8.6f) < 0.0001f);
//     }
//
//     {
//       int count = 0;
//       cfg.getSignal<void()>("foobar")->connect([&count](){ count += 2; });
//       REQUIRE(count == 0);
//
//       auto func = cfg.compileScript("emit:foobar");
//       func();
//       REQUIRE(count == 2);
//       func(); func();
//       REQUIRE(count == 6);
//     }
//
//     {
//       int count1=0, count2=0;
//       cfg.getSignal<void()>("signaller")->connect([&count1](){ count1 += 1; });
//       cfg.getState<bool>("toggler")->push([&count2](const bool& v){ count2 += 1; });
//
//       REQUIRE(count1 == 0);
//       REQUIRE(count2 == 0);
//
//       auto func = cfg.compileScript("emit:signaller;toggle:toggler");
//       func();
//       REQUIRE(count1 == 1);
//       REQUIRE(count2 == 1);
//       func(); func();
//       REQUIRE(count1 == 3);
//       REQUIRE(count2 == 3);
//     }
//   }
// }
