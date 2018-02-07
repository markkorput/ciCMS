#pragma once

#include <map>
#include <iostream>
// blocks
#include "ciCMS/cfg/Configurator.h"
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

  public: // cfg

    void cfg(cms::cfg::Configurator& cfgr, const std::map<string, string>& data){
      cms::cfg::Configurator::cfg(cfgr, data);
    }

    void cfg(Runner& obj, const std::map<string, string>& data){
      read(data)
      .with("drawEmit", [this, &obj](const std::string& v){
        auto pSignal = this->getSignal<void()>(v);
        obj.drawSignal.connect([pSignal](){ pSignal->emit(); });
      });
    }

    void cfg(syphonClient& obj, const std::map<string, string>& data){
      read(data)
      .with("server", [&obj](const std::string& v){ obj.set("", v); });
    }

    void cfg(SyphonClientRenderer& obj, const std::map<string, string>& data){
      read(data)

      .with("client", [this, &obj](const std::string& v){
        auto p = this->getObject<syphonClient>(v);

        if (!p) {
          std::cerr << "could not find syphonClient object: " << v << std::endl;
          return;
        }

        obj.setSyphonClient(p);
      })

      .with("drawOn", [this, &obj](const std::string& v){
        auto pSignal = this->getSignal<void()>(v);
        pSignal->connect([&obj](){ obj.draw(); });
      });
    }

    // cfg-by-data-id
    template<typename T>
    void cfg(T& obj, const std::string& modelId) {
      this->cfgWithModel<T>(obj, *this->getModelCollection().findById(modelId, true));
    }
};
