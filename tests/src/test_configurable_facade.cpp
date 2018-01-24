#include "catch.hpp"

#include "ciCMS/cfg/ConfigurableFacade.h"
#include "ciCMS/ModelCollection.h"

using namespace cms;
using namespace cms::cfg;

class CustomObject {
public:
  CustomObject(): name(""), age(0){}
  string name;
  int age;
};

TEST_CASE("cms::cfg::ConfigurableFacade", ""){
  SECTION("default_facade"){
    class CustomObjectCfgEmpty : public ConfigurableFacade<CustomObject> {
    };

    CustomObject obj;
    CustomObjectCfgEmpty cfg;
    std::map<string,string> data;
    data["name"] = "John Doe";
    data["age"] = "333";
    cfg.cfg(obj, data);
    REQUIRE(obj.name == "");
    REQUIRE(obj.age == 0);
  }

  SECTION("custom_cfg_method"){
    class CustomObjectCfgMethod : public ConfigurableFacade<CustomObject> {
    public:
      void cfg(CustomObject& obj, const std::map<string, string>& data){
        Model m;
        m.set(data);
        m.with("name", [&obj](const string& v){ obj.name = v; });
        m.withInt("age", [&obj](const int& v){ obj.age = v; });
      }
    };

    CustomObject obj;
    CustomObjectCfgMethod cfg;
    std::map<string,string> data;
    data["name"] = "John Doe";
    data["age"] = "333";
    cfg.cfg(obj, data);
    REQUIRE(obj.name == "John Doe");
    REQUIRE(obj.age == 333);
  }

  SECTION("custom_attributes"){
    class CustomObjectCfgAttrs : public ConfigurableFacade<CustomObject> {
    public:
      CustomObjectCfgAttrs(){
        this->add<string>("name", [](CustomObject &obj, const std::string &v){
          obj.name = v;
        });

        this->add<int>("age", [](CustomObject &obj, const int &v){
          obj.age = v;
        });
      }
    };

    CustomObject obj;
    CustomObjectCfgAttrs cfg;
    std::map<string,string> data;
    data["name"] = "John Doe";
    data["age"] = "333";
    cfg.cfg(obj, data);
    REQUIRE(obj.name == "John Doe");
    REQUIRE(obj.age == 333);
  }
}
