#include "catch.hpp"
#include <map>


#include "ciCMS/ModelBase.h"

using namespace std;
using namespace cms;
// using namespace cms::cfg;
//
// class ConfigurableObject {
//   public:
//     std::string name;
//     int age;
//     float score;
//     CustomObject() : name(""), age(0), score(0.0f){}
//
//     void cfg(Cfg& cfg) {
//       cfg.apply("name", this->name);
//       cfg.apply("age", this->age);
//       cfg.apply("score", this->score);
//     }
// };

class Cfg {
public:
  Cfg(const map<string, string> &data) { model.set(data); }

  Cfg& set(string attr, string& var) { var = model.get(attr); return *this; }
  Cfg& set(string attr, int& var) { var = model.getInt(attr); return *this; }
  Cfg& set(string attr, bool& var) { var = model.getBool(attr); return *this; }


  // string get(string attr, string defaultVal="") { return model.get(attr, defaultVal); }

  // Cfg

private:
  ModelBase model;

};

TEST_CASE("cms::cfg::Cfg", ""){
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
}
