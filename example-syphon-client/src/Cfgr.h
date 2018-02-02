#pragma once

// blocks
#include "ciCMS/cfg/Configurator.h"
#include "ciCMS/cfg/ctree/Node.h"
#include "ciCMS/Model.h"
#include "ciCMS/ModelCollection.h"
#include "cinderSyphon.h"
// local
#include "Runner.h"
#include "SyphonClientRenderer.h"

using namespace cms;

// our custom configurator for our test-classes
class Cfgr : public cms::cfg::Configurator {
  public:

    Cfgr() {
    }

    Cfgr(ModelCollection& mc) : cms::cfg::Configurator(mc) {
    }

    // using cms::cfg::ctree::cfgWithModel;
    void cfg(cms::cfg::ctree::Node& n, const std::map<string, string>& data){
      // TODO; take name from name attribute or otherwise default to last part
      // (splitting by period (.) of the id)
    }

    void cfg(Runner& obj, const std::map<string, string>& data){
      Model m;
      m.set(data);
      // m.with("name", [&obj](const std::string& v){ obj.name = v; });
    }

    void cfg(syphonClient& obj, const std::map<string, string>& data){
      Model m;
      m.set(data);
      // m.with("name", [&obj](const std::string& v){ obj.name = v; });
    }

    void cfg(SyphonClientRenderer& obj, const std::map<string, string>& data){
      Model m;
      m.set(data);
      // m.with("name", [&obj](const std::string& v){ obj.name = v; });
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
