#pragma once

#include <map>
#include <iostream>
// blocks
#include "ctree/signal.hpp"
#include "ctree/node.h"
#include "ciCMS/cfg/Configurator.h"
#include "ciCMS/cfg/ctree/Builder.h"
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
    typedef std::function<void*(const std::string&)> ObjectFetcherFunc;

  private:
    std::map<std::string, void*> signals;
    ObjectFetcherFunc objectFetcherFunc;

  public:
    void setObjectFetcher(ObjectFetcherFunc func){ this->objectFetcherFunc = func; }

    template <typename Signature>
    ctree::Signal<Signature>* getSignal(const std::string& id) {
      auto p = this->signals[id];

      if (p != NULL) {
        return (ctree::Signal<Signature>*)p;
      }

      auto pp = new ctree::Signal<Signature>();
      this->signals[id] = pp;
      return pp;
    }

    template<typename ObjT>
    ObjT* getObject(const std::string& id) {
      return this->objectFetcherFunc ? (ObjT*)this->objectFetcherFunc(id) : NULL;
    }

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

      m.with("drawEmit", [this, &obj](const std::string& v){
        auto pSignal = this->getSignal<void()>(v);
        std::cout << "drawEmit signal: " << pSignal << std::endl;
        obj.drawSignal.connect([pSignal](){ pSignal->emit(); });
      });
    }

    void cfg(syphonClient& obj, const std::map<string, string>& data){
      Model m;
      m.set(data);
      m.with("server", [&obj](const std::string& v){ obj.set("", v); CI_LOG_I("Set syphon client to: " << v); });
    }

    void cfg(SyphonClientRenderer& obj, const std::map<string, string>& data){
      Model m;
      m.set(data);

      m.with("client", [this, &obj](const std::string& v){
        auto p = this->getObject<syphonClient>(v);

        if (!p) {
          std::cerr << "could not find syphonClient object: " << v << std::endl;
          return;
        }

        obj.setSyphonClient(p);
      });

      m.with("drawOn", [this, &obj](const std::string& v){
        auto pSignal = this->getSignal<void()>(v);
        std::cout << "drawOn signal: " << pSignal << std::endl;
        pSignal->connect([&obj](){ obj.draw(); });
      });
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
