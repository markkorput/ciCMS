#pragma once

#include <iostream>
#include <regex>
#include "ciCMS/ModelCollection.h"
#include "ctree/signal.hpp"
#include "CfgReader.hpp"
#include "ciCMS/State.h"
#include "ciCMS/deserialise.h"

namespace cms { namespace cfg {

  class Configurator {
    public: // types

      typedef Model CfgData;
      typedef std::map<string, string> CfgDataRaw;
      typedef std::function<void*(const std::string&)> ObjectFetcherFunc;
      typedef std::function<void()> CompiledScriptFunc;

      struct ObjCallback {
        std::string id;
        std::function<void(void*)> func;
      };

      // class Applier {
      //   public:
      //     Applier(Configurator* cfg, CfgData& data) : cfg(cfg), data(data){
      //     };
      //
      //     template<typename ObjT>
      //     void to(ObjT* obj){
      //       cfg->cfgWithModel(*obj, data);
      //     }
      //
      //   private:
      //     Configurator* cfg;
      //     CfgData& data;
      // };

    public: // lifecycle methods
      Configurator() : bActive(false), bPrivateModelCollection(true){
        modelCollection = new ModelCollection();
      }

      Configurator(ModelCollection& mc) : bActive(false), bPrivateModelCollection(false), modelCollection(&mc) {
      }

      ~Configurator(){
        if(bPrivateModelCollection && modelCollection){
          delete modelCollection;
          modelCollection = NULL;
        }

        std::cout << "T O D O: deallocate all items in this.signals and this.states" << std::endl;
      }

    public: // getters and setters

      bool isActive() const { return this->bActive; }
      void setActive(bool active) {
        this->bActive = active;
        if (active)
          std::cout << "!!! Configurator::setActive; configurator set to active = TRUE, use for development only!!!\n" << std::endl;
      }

      ModelCollection& getModelCollection() { return *this->modelCollection; }

      void setObjectFetcher(ObjectFetcherFunc func){
        this->objectFetcherFunc = func;
      }

      void notifyNewObject(void* obj, const CfgData& data) {
        for(int i=objCallbacks.size()-1; i>=0; i--) {
          auto oc = objCallbacks[i];
          if(oc.id == data.getId()) {
            oc.func(obj);
            objCallbacks.erase(objCallbacks.begin()+i);
          }
        }
      }

      inline void* getObjectPointer(const std::string& id) {
        return this->objectFetcherFunc ? this->objectFetcherFunc(id) : NULL;
      }

      template<typename ObjT>
      ObjT* getObject(const std::string& id) {
        return (ObjT*)this->getObjectPointer(id);
      }

      template<typename ObjT>
      void withObject(const std::string& id, std::function<void(ObjT&)> func) {
        auto p = this->getObject<ObjT>(id);

        if(p) {
          func(*p);
          return;
        }

        // register callback to get invoked later

        ObjCallback oc;
        oc.id = id;
        oc.func = [func](void* objPointer) {
          func(*(ObjT*)objPointer);
        };

        this->objCallbacks.push_back(oc);
      }


      template <typename Signature>
      ::ctree::Signal<Signature>* getSignal(const std::string& id) {
        auto p = this->signals[id];

        if (p != NULL) {
          return (::ctree::Signal<Signature>*)p;
        }

        auto pp = new ::ctree::Signal<Signature>();
        this->signals[id] = pp;
        return pp;
      }

      template <typename Signature>
      cms::State<Signature>* getState(const std::string& id) {
        auto p = this->states[id];

        if (p != NULL) {
          return (cms::State<Signature>*)p;
        }

        auto pp = new cms::State<Signature>();
        this->states[id] = pp;
        return pp;
      }

    public: // helper methods

      CompiledScriptFunc compileScript(const std::string& script) {
        std::smatch match;

        { std::regex expr("^emit:(\\w+)$");
          if( std::regex_match(script, match, expr) ) {
            auto pSignal = this->getSignal<void()>(match[1]);
            return [pSignal](){ pSignal->emit(); };
          }
        }

        { std::regex expr("^toggle:(\\w+)$");
          if( std::regex_match(script, match, expr) ) {
            auto pState = this->getState<bool>(match[1]);
            return [pState](){
              pState->operator=(!pState->val());
            };
          }
        }

        { std::regex expr("^\\+(\\d+):(\\w+)$");
          if( std::regex_match(script, match, expr) ) {
            int delta = cms::deserialiseInt(match[1], 0);
            auto pState = this->getState<int>(match[2]);

            return [pState, delta](){
              pState->operator=(pState->val()+delta);
            };
          }
        }

        { std::regex expr("^\\-(\\d+):(\\w+)$");
          if( std::regex_match(script, match, expr) ) {
            int delta = cms::deserialiseInt(match[1], 0);
            auto pState = this->getState<int>(match[2]);

            return [pState, delta](){
              pState->operator=(pState->val()-delta);
            };
          }
        }

        { std::regex expr("^\\+(\\d+\\.\\d)+:(\\w+)$");
          if( std::regex_match(script, match, expr) ) {
            auto delta = cms::deserialiseFloat(match[1], 0.0f);
            auto pState = this->getState<float>(match[2]);

            return [pState, delta](){
              pState->operator=(pState->val()+delta);
            };
          }
        }

        { std::regex expr("^\\-(\\d+\\.\\d)+:(\\w+)$");
          if( std::regex_match(script, match, expr) ) {
            auto delta = cms::deserialiseFloat(match[1], 0.0f);
            auto pState = this->getState<float>(match[2]);

            return [pState, delta](){
              pState->operator=(pState->val()-delta);
            };
          }
        }

        return [](){};
      }

    public: // cfgs

      void apply(Model& model, Model::ModelTransformFunctor func, void* activeCallbackOwner = NULL){
        model.transform(func, activeCallbackOwner, this->bActive);
      }

      // shared_ptr<Applier> apply(CfgData& data) {
      //   return std::make_shared<Applier>(this, data);
      // }

      template<typename T>
      void cfgWithModel(T& c, Model& model){
        this->apply(model, [this, &c](ModelBase& mod){
          this->cfg(c, mod.attributes());
        });
      }

      void cfg(Configurator& c, const std::map<string, string>& data){
        Model m;
        m.set(data);
        m.withBool("active", [&c](const bool& v){ c.setActive(v); });
      }

      static const CfgReader& read(const CfgDataRaw& data) {
        return CfgReader::read(data);
      }

    private: // attributes
      bool bActive, bPrivateModelCollection;
      ModelCollection* modelCollection;
      ObjectFetcherFunc objectFetcherFunc;
      std::map<std::string, void*> signals;
      std::map<std::string, void*> states;

      // object callbacks
      std::vector<ObjCallback> objCallbacks;
  };
}}
