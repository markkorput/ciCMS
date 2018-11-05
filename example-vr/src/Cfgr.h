#pragma once

#include "ciCMS/cfg/Configurator.h"
#include "ciCMS/Model.h"
#include "ciCMS/ModelCollection.h"

using namespace cms;

// our custom configurator for our test-classes
class Cfgr : public cms::cfg::Configurator {
  public:
    Cfgr() {
    }

    Cfgr(ModelCollection& mc) : cms::cfg::Configurator(mc) {
    }

  public: // cfg

    template <typename Typ>
    void cfg(Typ& subject, const std::string& modelId) {
      auto model = getModelCollection().findById(modelId, true);
      this->cfg(subject, model->attributes());
    }

    void cfg(cms::cfg::Configurator& cfgr, const std::map<string, string>& data){
      cms::cfg::Configurator::cfg(cfgr, data);
    }
};
