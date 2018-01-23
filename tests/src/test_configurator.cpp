#include "catch.hpp"

#include "ciCMS/cfg/Configurator.h"
#include "ciCMS/ModelCollection.h"

using namespace cms;
using namespace cms::cfg;

TEST_CASE("cms::cfg::Configurator", ""){
  SECTION("default_model_collection"){
    Configurator configurator;
    REQUIRE(configurator.getModelCollection().size() == 0);
  }

  SECTION("given_model_collection"){
    ModelCollection mc;
    Configurator configurator(mc);
    REQUIRE(&mc == &configurator.getModelCollection());
  }

  SECTION("has_cfg_method_for_itself"){
    std::map<string, string> data;
    data["active"] = "true";
    Configurator configurator;
    REQUIRE(configurator.isActive() == false);
    configurator.cfg(configurator, data);
    REQUIRE(configurator.isActive() == true);
  }

  SECTION("setActive"){
    Configurator configurator;
    REQUIRE(configurator.isActive() == false);
    configurator.setActive(true);
    REQUIRE(configurator.isActive() == true);
  }
}
