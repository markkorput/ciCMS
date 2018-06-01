#pragma once

#include <map>
#include <iostream>
#include <regex>
// blocks
#include "ciCMS/cfg/Configurator.h"
#include "ciCMS/Model.h"
#include "ciCMS/ModelCollection.h"

// local
#include "Runner.h"
#include "Keyboard.h"
#include "components/HttpServer.h"

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
      .with("drawOn", [this, &obj](const std::string& v){
        this->onSignals<void()>(v, [&obj](){ obj.draw(); });
      })
      .with("setupEmit", [this, &obj](const std::string& v){
        auto pSignal = this->getSignal<void()>(v);
        obj.setupSignal.connect([pSignal](){ pSignal->emit(); });
      })
      .with("exitEmit", [this, &obj](const std::string& v){
        auto pSignal = this->getSignal<void()>(v);
        obj.exitSignal.connect([pSignal](){ pSignal->emit(); });
      })
      .with("drawEmit", [this, &obj](const std::string& v){
        auto pSignal = this->getSignal<void()>(v);
        obj.drawSignal.connect([pSignal](){ pSignal->emit(); });
      })
      .with("updateEmit", [this, &obj](const std::string& v){
        auto pSignal = this->getSignal<void()>(v);
        obj.updateSignal.connect([pSignal](){ pSignal->emit(); });
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

    void cfg(HttpServer& obj, const std::map<string, string>& data){
      read(data)
      .withInt("port", [&obj](const int& v){
        obj.setPort(v);
      });

      obj.start(true /* force restart */);
    }

    // cfg-by-data-id
    template<typename T>
    void cfg(T& obj, const std::string& modelId) {
      this->cfgWithModel<T>(obj, *this->getModelCollection().findById(modelId, true));
    }

    template <typename Typ>
    void withStates(const std::string& ids, std::function<void(cms::State<Typ>&, const std::string&)> func) {
      std::vector<std::string> strings;
      boost::split(strings, ids, boost::is_any_of(","));
      for(auto& id : strings) {
        auto pState = this->getState<Typ>(id);
        func(*pState, id);
      }
    }

    template <typename Signature>
    void onSignals(const std::string& signalIdentifiers, std::function<Signature> func) {
      std::vector<std::string> strings;
      boost::split(strings, signalIdentifiers, boost::is_any_of(","));
      for(auto& id : strings) {
        auto pSignal = this->getSignal<Signature>(id);
        pSignal->connect(func);
      }
    }
};
