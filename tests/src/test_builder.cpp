#include "catch.hpp"

#include "ciCMS/cfg/Builder.h"
#include "ciCMS/cfg/Configurator.h"
// #include "ciCMS/ModelCollection.h"

using namespace cms;

class CustomNodeAddon;
class CustomNode {
  friend CustomNodeAddon;

  public:
    CustomNode(): name(""), value(0.0f){}
    string name;
    float value;
    std::vector<std::shared_ptr<CustomNode>> children;

  private:
    std::vector<CustomNodeAddon*> addons;
};

class CustomNodeAge : public CustomNode {
public:
  CustomNodeAge() : age(0){}
  int age;
};

class CustomNodeAddon {
  public:
    void addTo(CustomNode& newnode){
      // we're a friend class of CustomNode, so we can
      // directly access its private addons attribute
      newnode.addons.push_back(this);
      this->node = &newnode;
    }

    CustomNode* node;
};

class Configurator : public cms::cfg::Configurator {
  public:
    Configurator(ModelCollection& mc) : cms::cfg::Configurator(mc) {
    }
    void cfg(CustomNode& node, std::map<string, string> data){

    }

    void cfg(CustomNodeAddon& addon, std::map<string, string> data){

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

class Builder : public cms::cfg::Builder {
  private:
    std::shared_ptr<Configurator> configurator;

  public:
    Builder(){
      this->configurator = std::make_shared<Configurator>(this->getModelCollection());

      this->addInstantiator("CustomNode", [this](Model& model){
        // create instance
        auto n = new CustomNode();
        // apply configuration data
        this->configurator->cfgWithModel(*n, model);
        // return to caller
        return n;
      });

      this->addInstantiator("CustomNodeAge", "CustomNode" /* root class */, [this](Model& model){
        // create instance
        auto n = new CustomNodeAge();
        // apply configuration data
        this->configurator->cfgWithModel(*n, model);
        // return to caller
        return n;
      });

      // this is how inter-object relationships are configured, use only "root classnames"
      this->addConnector("CustomNode", "CustomNode", [](void* child, void* parent){
        ((CustomNode*)parent)->children.push_back(std::shared_ptr<CustomNode>((CustomNode*)child));
      });

      // this is how inter-object relationships are configured, use only "root classnames"
      this->addConnector("CustomNodeAddon", "CustomNode", [](void* child, void* parent){
        ((CustomNodeAddon*)child)->addTo(*(CustomNode*)parent);
      });

      this->addInstantiator("CustomNodeAddon", [this](Model& model){
        // create instance
        auto a = new CustomNodeAddon();
        // apply configuration data
        this->configurator->cfgWithModel(*a, model);
        // return to caller
        return a;
      });

      // this->addInstantiator("CustomNodeCustomAddon", "CustomNodeAddon" /* root class */, [this](const Model& model){
      //   // create instance
      //   auto n = new CustomNodeAddon();
      //   // apply configuration data
      //   this->configurator->cfgWithModel(*n, model);
      //   // return to caller
      //   return n;
      // });
    }
};

TEST_CASE("cms::cfg::Builder", ""){
  SECTION("typical_usage"){
    Builder builder;
    builder.getModelCollection().loadJsonFromFile(ci::app::getAssetPath("test_builder.json"));

    auto n1 = std::shared_ptr<CustomNode>(builder.build<CustomNode>("typical_usage.node1"));
    REQUIRE(n1->name == "root");
    REQUIRE(n1->value == 1.0f);
    REQUIRE(n1->children.size() == 2);

    REQUIRE(n1->children[0]->name == "child1");
    REQUIRE(n1->children[0]->value == 2.0f);
    REQUIRE(n1->children[0]->children.size() == 1);

    REQUIRE(n1->children[0]->children[0]->name == "grandchild1");
    REQUIRE(n1->children[0]->children[0]->value == 3.0f);
    REQUIRE(n1->children[0]->children[0]->children.size() == 0);

    REQUIRE(n1->children[1]->name == "child2");
    REQUIRE(n1->children[1]->value == 4.5f);
    REQUIRE(n1->children[1]->children.size() == 0);
  }
}
