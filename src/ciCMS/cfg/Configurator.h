#pragma once

#include <iostream>
#include <regex>
#include "boost/algorithm/string.hpp"
#include "ciCMS/ModelCollection.h"
#include "ctree/signal.hpp"
#include "Cfg.h"
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
      }

    public: // getters and setters

      bool isActive() const { return this->bActive; }
      void setActive(bool active) {
        this->bActive = active;
        if (active) std::cout << "!!! Configurator::setActive; configurator set to active = TRUE, use for development only!!!\n" << std::endl;
      }

      ModelCollection& getModelCollection() { return *this->modelCollection; }

      void setObjectFetcher(ObjectFetcherFunc func){
        this->objectFetcherFunc = func;
        if (this->cfgRef) this->cfgRef->setObjectFetcher(func);
      }

      void notifyNewObject(void* obj, const CfgData& data) {
        this->getCfg()->notifyNewObject(data.getId(), obj);
      }

      inline void* getObjectPointer(const std::string& id) {
        return getCfg()->getObjectPointer(id);
      }

      template<typename ObjT>
      ObjT* getObject(const std::string& id) {
        return getCfg()->getObject<ObjT>(id);
      }

      template<typename ObjT>
      size_t getObjects(std::vector<ObjT*> target, const std::string& ids, const std::string& delimiter=",") {
        return getCfg()->getObjects<ObjT>(target, ids, delimiter);
      }

      template<typename ObjT>
      void withObject(const std::string& id, std::function<void(ObjT&)> func) {
        getCfg()->withObjects<ObjT>(id, func);
      }

      template<typename ObjT>
      void withObjects(const std::string& ids, std::function<void(ObjT&)> func, std::string delimiter=",") {
        getCfg()->withObjects<ObjT>(ids, func, delimiter);
      }

      template<typename ObjT>
      void withObjects(const std::string& ids, std::function<void(ObjT&, const std::string& objectId)> func, std::string delimiter=",") {
        getCfg()->withObjects<ObjT>(ids, func, delimiter);
      }

      template<typename ObjT>
      void withObjectByAttr(const std::string& id, std::function<void(ObjT&)> func) {
        getCfg()->withObjectByAttr<ObjT>(id, func);
      }

      template<typename ObjT>
      void withObjectsByAttr(const std::string& id, std::function<void(ObjT&)> func) {
        getCfg()->withObjectsByAttr<ObjT>(id, func);
      }

      template <typename Signature>
      ::ctree::Signal<Signature>* getSignal(const std::string& id) {
        return getCfg()->getSignal<Signature>(id);
      }

      template <typename Signature>
      cms::State<Signature>* getState(const std::string& id) {
        return getCfg()->getState<Signature>(id);
      }

    public: // helper methods

      CompiledScriptFunc compileScript(const std::string& script) {
        return getCfg()->compileScript(script);
      }

      CfgRef getCfg() {
        if (cfgRef == nullptr) cfgRef = std::make_shared<Cfg>(signals, states, objectFetcherFunc);
        return cfgRef;
      }

    public: // cfgs

      void apply(Model& model, Model::ModelTransformFunctor func, void* activeCallbackOwner = NULL){
        this->getCfg()->setAttributes(model.attributes());
        model.transform(func, activeCallbackOwner, this->bActive);
      }

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

      static const std::shared_ptr<CfgReader> read(const CfgDataRaw& data) {
        return CfgReader::read(data);
      }

    private: // attributes
      bool bActive, bPrivateModelCollection;
      ModelCollection* modelCollection;
      ObjectFetcherFunc objectFetcherFunc;
      std::map<std::string, void*> signals;
      std::map<std::string, void*> states;
      CfgRef cfgRef = nullptr;

      // object callbacks
      std::vector<ObjCallback> objCallbacks;
  };
}}
