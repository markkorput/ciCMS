#pragma once

#include <map>
#include <iostream>
#include <regex>
// blocks
#include "ciCMS/cfg/Configurator.h"
#include "ciCMS/Model.h"
#include "ciCMS/ModelCollection.h"
#include "cinderSyphon.h"
// local
#include "Runner.h"
#include "Keyboard.h"
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
      })
      .with("drawState", [this, &obj](const std::string& v){
        auto pState = this->getState<bool>(v);
        pState->operator=(true);
        // obj.drawSignal.connect([pSignal](){ pSignal->emit(); });
        pState->push(obj.drawState);
      });
    }

    void cfg(Keyboard& obj, const std::map<string, string>& data){
      read(data)
      .withRegex("^key:(.)$", [this, &obj](const std::smatch& match, const std::string& v){
        // compile lambda func based on script in attribute value
        auto func = this->compileScript(v);
        // register func as action to be performed when the specified key is pressed
        obj.onKeyDown(match[1], func);
      });
    }


    void cfg(syphonClient& obj, const std::map<string, string>& data){
      read(data)
      .with("server", [this, &data, &obj](const std::string& v){
        auto readr = this->read(data);
        if (readr.has("channel")) {
          obj.set(v, readr.at("channel"));
        } else {
          obj.set("", v);
        }
      });
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
